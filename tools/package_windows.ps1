param(
    [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$BuildDir = "",
    [string]$Configuration = "Release",
    [string]$QtBinDir = "",
    [string[]]$DependencyRoots = @(),
    [switch]$SkipBuild,
    [switch]$SkipZip
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

function Fail($Message) {
    throw "[SkyGrid Windows Package] $Message"
}

function Info($Message) {
    Write-Host "[SkyGrid Windows Package] $Message"
}

function Test-PathAny([string]$Path, [string]$PathType = "Any") {
    if (-not $Path) {
        return $false
    }
    if (Test-Path -Path $Path -PathType $PathType -ErrorAction SilentlyContinue) {
        return $true
    }
    return (Test-Path -LiteralPath $Path -PathType $PathType -ErrorAction SilentlyContinue)
}

function Resolve-PathIfExists([string]$Path) {
    if (-not $Path) {
        return ""
    }
    $resolved = Resolve-Path -Path $Path -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($resolved) {
        return $resolved.Path
    }
    $resolved = Resolve-Path -LiteralPath $Path -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($resolved) {
        return $resolved.Path
    }
    return ""
}

function Resolve-ExistingPath([string]$Path, [string]$Label) {
    $resolved = Resolve-PathIfExists $Path
    if (-not $resolved) {
        Fail "$Label path does not exist: $Path"
    }
    return $resolved
}

function Find-CommandPath([string[]]$Names, [string]$HintDir) {
    $resolvedHintDir = Resolve-PathIfExists $HintDir
    if ($resolvedHintDir) {
        foreach ($name in $Names) {
            $candidate = Join-Path $resolvedHintDir $name
            if (Test-PathAny -Path $candidate -PathType "Leaf") {
                return (Resolve-Path -LiteralPath $candidate).Path
            }
        }
    }

    foreach ($name in $Names) {
        $command = Get-Command $name -CommandType Application -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($command -and $command.Source) {
            return $command.Source
        }
    }
    return ""
}

function Add-Root([System.Collections.Generic.List[string]]$Roots, [string]$Path) {
    $resolved = Resolve-PathIfExists $Path
    if (-not $resolved) {
        return
    }
    if (Test-PathAny -Path $resolved -PathType "Leaf") {
        $resolved = Split-Path -Path $resolved -Parent
    }
    if (-not (Test-PathAny -Path $resolved -PathType "Container")) {
        return
    }
    if (-not $Roots.Contains($resolved)) {
        $Roots.Add($resolved)
    }
}

function Format-RootList([System.Collections.Generic.List[string]]$Roots) {
    if ($Roots.Count -eq 0) {
        return "`n - <none>"
    }
    return ($Roots | ForEach-Object { "`n - $_" }) -join ""
}

function Copy-DllToStage([string]$DllPath, [string]$StageDir) {
    $dest = Join-Path $StageDir (Split-Path $DllPath -Leaf)
    if (-not (Test-PathAny -Path $dest -PathType "Leaf")) {
        Copy-Item -LiteralPath $DllPath -Destination $dest
        Info "Copied $(Split-Path $DllPath -Leaf)"
    }
}

function Copy-StageAliasIfPresent([string]$StageDir, [string]$SourceName, [string]$AliasName) {
    $sourcePath = Join-Path $StageDir $SourceName
    $aliasPath = Join-Path $StageDir $AliasName
    if ((Test-PathAny -Path $sourcePath -PathType "Leaf") -and -not (Test-PathAny -Path $aliasPath -PathType "Leaf")) {
        Copy-Item -LiteralPath $sourcePath -Destination $aliasPath
        Info "Created alias $AliasName -> $SourceName"
    }
}

function Ensure-GlibRuntimeAliases([string]$StageDir) {
    $aliases = @(
        @{ source = "librsvg-2-2.dll"; alias = "rsvg-2-2.dll" },
        @{ source = "libcairo-2.dll"; alias = "cairo-2.dll" },
        @{ source = "libglib-2.0-0.dll"; alias = "glib-2.0-0.dll" },
        @{ source = "libgobject-2.0-0.dll"; alias = "gobject-2.0-0.dll" },
        @{ source = "libgio-2.0-0.dll"; alias = "gio-2.0-0.dll" },
        @{ source = "libgmodule-2.0-0.dll"; alias = "gmodule-2.0-0.dll" },
        @{ source = "libgthread-2.0-0.dll"; alias = "gthread-2.0-0.dll" },
        @{ source = "libgdk_pixbuf-2.0-0.dll"; alias = "gdk_pixbuf-2.0-0.dll" }
    )
    foreach ($entry in $aliases) {
        Copy-StageAliasIfPresent -StageDir $StageDir -SourceName $entry["source"] -AliasName $entry["alias"]
    }
}

function Copy-FirstMatchingDll($Patterns, $Roots, $StageDir, [switch]$Required, $Label) {
    foreach ($pattern in $Patterns) {
        foreach ($root in $Roots) {
            if (-not (Test-PathAny -Path $root -PathType "Container")) {
                continue
            }
            $match = Get-ChildItem -LiteralPath $root -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue |
                Where-Object { $_.FullName -notmatch "\\debug\\" -and $_.FullName -notmatch "\\Debug\\" } |
                Select-Object -First 1
            if ($match) {
                Copy-DllToStage -DllPath $match.FullName -StageDir $StageDir
                return @($match.FullName)
            }
        }
    }
    if ($Required) {
        Fail "Could not find required $Label DLL. Roots searched:$(Format-RootList -Roots $Roots)`nPass -DependencyRoots with folders containing this runtime."
    }
    return @()
}

function Copy-AllMatchingDlls($Patterns, $Roots, $StageDir) {
    foreach ($pattern in $Patterns) {
        foreach ($root in $Roots) {
            if (-not (Test-PathAny -Path $root -PathType "Container")) {
                continue
            }
            $matches = Get-ChildItem -LiteralPath $root -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue |
                Where-Object { $_.FullName -notmatch "\\debug\\" -and $_.FullName -notmatch "\\Debug\\" }
            foreach ($match in $matches) {
                Copy-DllToStage -DllPath $match.FullName -StageDir $StageDir
            }
        }
    }
}

function Copy-AllDllsFromDirectories([string[]]$Directories, [string]$StageDir) {
    $seen = @{}
    $copiedCount = 0
    foreach ($dir in $Directories) {
        $resolved = Resolve-PathIfExists $dir
        if (-not $resolved) {
            continue
        }
        $key = $resolved.ToLowerInvariant()
        if ($seen.ContainsKey($key)) {
            continue
        }
        $seen[$key] = $true
        if (-not (Test-PathAny -Path $resolved -PathType "Container")) {
            continue
        }
        $matches = Get-ChildItem -LiteralPath $resolved -File -Filter "*.dll" -ErrorAction SilentlyContinue |
            Where-Object {
                $_.FullName -notmatch "\\debug\\" -and
                $_.FullName -notmatch "\\Debug\\" -and
                $_.Name -notmatch "^(api-ms-win|ext-ms-win|ucrtbase)\."
            }
        foreach ($match in $matches) {
            $dest = Join-Path $StageDir $match.Name
            if (-not (Test-PathAny -Path $dest -PathType "Leaf")) {
                Copy-DllToStage -DllPath $match.FullName -StageDir $StageDir
                $copiedCount += 1
            }
        }
    }
    if ($copiedCount -gt 0) {
        Info "Copied $copiedCount additional runtime DLL(s) from detected dependency directories."
    }
}

function Assert-StageFile([string]$StageDir, [string]$RelativePath, [string]$Label) {
    $candidate = Join-Path $StageDir $RelativePath
    if (-not (Test-PathAny -Path $candidate -PathType "Leaf")) {
        Fail "$Label is missing after packaging: $RelativePath"
    }
}

function Test-StageAnyMatch([string]$StageDir, [string[]]$Patterns) {
    foreach ($pattern in $Patterns) {
        $glob = Join-Path $StageDir $pattern
        if (Get-ChildItem -Path $glob -File -ErrorAction SilentlyContinue | Select-Object -First 1) {
            return $true
        }
    }
    return $false
}

function Assert-StageAnyMatch([string]$StageDir, [string[]]$Patterns, [string]$Label) {
    if (Test-StageAnyMatch -StageDir $StageDir -Patterns $Patterns) {
        return
    }
    Fail "$Label was not found in staged output. Checked patterns: $($Patterns -join ', ')"
}

function Get-MsvcRuntimeSearchRoots() {
    $runtimeRoots = [System.Collections.Generic.List[string]]::new()

    Add-Root $runtimeRoots $env:VCToolsRedistDir
    if ($env:VCINSTALLDIR) {
        Add-Root $runtimeRoots (Join-Path $env:VCINSTALLDIR "Redist")
        Add-Root $runtimeRoots (Join-Path $env:VCINSTALLDIR "Redist\MSVC")
    }
    if ($env:VSINSTALLDIR) {
        Add-Root $runtimeRoots (Join-Path $env:VSINSTALLDIR "VC\Redist")
        Add-Root $runtimeRoots (Join-Path $env:VSINSTALLDIR "VC\Redist\MSVC")
    }

    $vswhere = Find-CommandPath -Names @("vswhere.exe") -HintDir ""
    if ($vswhere) {
        $vsInstallPath = (& $vswhere -latest -products * -property installationPath 2>$null | Select-Object -First 1)
        if ($vsInstallPath) {
            $vsInstallPath = $vsInstallPath.ToString().Trim()
            if ($vsInstallPath) {
                Add-Root $runtimeRoots (Join-Path $vsInstallPath "VC\Redist")
                Add-Root $runtimeRoots (Join-Path $vsInstallPath "VC\Redist\MSVC")
            }
        }
    }

    Add-Root $runtimeRoots "C:\Program Files\Microsoft Visual Studio"
    Add-Root $runtimeRoots "C:\Program Files (x86)\Microsoft Visual Studio"

    return $runtimeRoots
}

function Ensure-MsvcRuntimeDlls([string]$StageDir) {
    $runtimePatterns = @("vcruntime*.dll", "msvcp*.dll")
    if (Test-StageAnyMatch -StageDir $StageDir -Patterns $runtimePatterns) {
        return
    }

    Info "MSVC runtime DLLs not found after windeployqt; attempting manual copy from Visual Studio redist folders."
    $runtimeRoots = Get-MsvcRuntimeSearchRoots
    if ($runtimeRoots.Count -eq 0) {
        return
    }

    $crtRoots = [System.Collections.Generic.List[string]]::new()
    foreach ($root in $runtimeRoots) {
        $crtDirs = Get-ChildItem -LiteralPath $root -Recurse -Directory -ErrorAction SilentlyContinue |
            Where-Object {
                $_.FullName -match "\\x64\\Microsoft\.VC\d+\.CRT$" -and
                $_.FullName -notmatch "\\debug_nonredist\\" -and
                $_.FullName -notmatch "\\Debug\\"
            } |
            Sort-Object -Property FullName -Descending
        foreach ($dir in $crtDirs) {
            Add-Root $crtRoots $dir.FullName
        }
    }
    if ($crtRoots.Count -eq 0) {
        $crtRoots = $runtimeRoots
    }

    Copy-FirstMatchingDll `
        -Patterns @("vcruntime140_1.dll", "vcruntime140.dll", "vcruntime*.dll") `
        -Roots $crtRoots `
        -StageDir $StageDir `
        -Required `
        -Label "MSVC vcruntime"

    Copy-FirstMatchingDll `
        -Patterns @("msvcp140.dll", "msvcp*.dll") `
        -Roots $crtRoots `
        -StageDir $StageDir `
        -Required `
        -Label "MSVC msvcp"

    Copy-FirstMatchingDll `
        -Patterns @("concrt140.dll", "concrt*.dll") `
        -Roots $crtRoots `
        -StageDir $StageDir `
        -Label "MSVC concrt"
}

function Add-MavsdkHintRoots([System.Collections.Generic.List[string]]$Roots, [string]$MavsdkDir) {
    $resolved = Resolve-PathIfExists $MavsdkDir
    if (-not $resolved) {
        return
    }

    $baseCandidates = @(
        $resolved,
        (Join-Path $resolved ".."),
        (Join-Path $resolved "..\.."),
        (Join-Path $resolved "..\..\..")
    )
    $suffixes = @(
        "",
        "bin",
        "lib",
        "Release",
        "Release\bin",
        "Debug",
        "Debug\bin"
    )

    foreach ($base in $baseCandidates) {
        foreach ($suffix in $suffixes) {
            if ($suffix) {
                Add-Root $Roots (Join-Path $base $suffix)
            } else {
                Add-Root $Roots $base
            }
        }
    }
}

function Add-DxCompilerHintRoots([System.Collections.Generic.List[string]]$Roots) {
    Add-Root $Roots $env:WindowsSdkDir
    if ($env:WindowsSdkDir) {
        Add-Root $Roots (Join-Path $env:WindowsSdkDir "Redist\D3D\x64")
        Add-Root $Roots (Join-Path $env:WindowsSdkDir "Redist\D3D")
    }
    Add-Root $Roots "C:\Program Files (x86)\Windows Kits\10\Redist\D3D\x64"
    Add-Root $Roots "C:\Program Files (x86)\Windows Kits\10\Redist\D3D"
    Add-Root $Roots "C:\Program Files (x86)\Windows Kits\10\bin"
}

if ([System.Environment]::OSVersion.Platform -ne [System.PlatformID]::Win32NT) {
    Fail "This packager only supports Windows hosts."
}

$RepoRoot = Resolve-ExistingPath -Path $RepoRoot -Label "RepoRoot"
if (-not $Configuration) {
    $Configuration = "Release"
}
if (-not $BuildDir) {
    $BuildDir = Join-Path $RepoRoot "build-windows-release"
}
$BuildDir = Resolve-ExistingPath -Path $BuildDir -Label "BuildDir"

$windeployqt = Find-CommandPath -Names @("windeployqt.exe", "windeployqt6.exe") -HintDir $QtBinDir
if (-not $windeployqt) {
    Fail "windeployqt was not found. Add the Qt MSVC bin folder to PATH or pass -QtBinDir."
}

Info "RepoRoot: $RepoRoot"
Info "BuildDir: $BuildDir"
Info "Configuration: $Configuration"

if (-not $SkipBuild) {
    Info "Building SkyGridGCS ($Configuration)"
    cmake --build $BuildDir --config $Configuration --target SkyGridGCS
    if ($LASTEXITCODE -ne 0) {
        Fail "Build failed."
    }
}

$exeCandidates = @(
    (Join-Path $BuildDir "bin\$Configuration\SkyGridGCS.exe"),
    (Join-Path $BuildDir "bin\SkyGridGCS.exe"),
    (Join-Path $BuildDir "$Configuration\SkyGridGCS.exe"),
    (Join-Path $BuildDir "SkyGridGCS.exe")
)
$exePath = $exeCandidates | Where-Object { Test-PathAny -Path $_ -PathType "Leaf" } | Select-Object -First 1
if (-not $exePath) {
    $exePath = Get-ChildItem -LiteralPath $BuildDir -Recurse -File -Filter "SkyGridGCS.exe" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -notmatch "\\debug\\" -and $_.FullName -notmatch "\\Debug\\" } |
        Select-Object -First 1 -ExpandProperty FullName
}
if (-not $exePath) {
    $formattedCandidates = ($exeCandidates | ForEach-Object { "`n - $_" }) -join ""
    Fail "SkyGridGCS.exe was not found in $BuildDir. Checked: $formattedCandidates"
}
$exePath = Resolve-ExistingPath -Path $exePath -Label "SkyGridGCS executable"

$distRoot = Join-Path $RepoRoot "dist\windows"
$stageDir = Join-Path $distRoot "SkyGridGCS"
$zipPath = Join-Path $distRoot "SkyGridGCS_Windows_Portable.zip"

New-Item -ItemType Directory -Path $distRoot -Force | Out-Null
if (Test-PathAny -Path $stageDir -PathType "Container") {
    Remove-Item -LiteralPath $stageDir -Recurse -Force
}
New-Item -ItemType Directory -Path $stageDir -Force | Out-Null
Copy-Item -LiteralPath $exePath -Destination (Join-Path $stageDir "SkyGridGCS.exe")

$iniSource = Join-Path $RepoRoot "release\windows\SkyGridGCS.ini"
if (Test-PathAny -Path $iniSource -PathType "Leaf") {
    Copy-Item -LiteralPath $iniSource -Destination (Join-Path $stageDir "SkyGridGCS.ini")
}
$readmeSource = Join-Path $RepoRoot "release\windows\README_WINDOWS_RELEASE.txt"
if (Test-PathAny -Path $readmeSource -PathType "Leaf") {
    Copy-Item -LiteralPath $readmeSource -Destination (Join-Path $stageDir "README_WINDOWS_RELEASE.txt")
}

$qmlDir = Resolve-ExistingPath -Path (Join-Path $RepoRoot "qml") -Label "QML source directory"
Info "Running windeployqt"
& $windeployqt --release --compiler-runtime --qmldir $qmlDir (Join-Path $stageDir "SkyGridGCS.exe")
if ($LASTEXITCODE -ne 0) {
    Fail "windeployqt failed."
}

Assert-StageFile -StageDir $stageDir -RelativePath "Qt6Core.dll" -Label "Qt6Core.dll"
Assert-StageFile -StageDir $stageDir -RelativePath "platforms\qwindows.dll" -Label "platforms\\qwindows.dll"
Assert-StageAnyMatch -StageDir $stageDir -Patterns @("qml\QtQuick\qmldir", "qml\QtQuick.2\qmldir") -Label "Qt QML modules"
Assert-StageAnyMatch -StageDir $stageDir -Patterns @("platforms\*.dll", "styles\*.dll", "imageformats\*.dll", "iconengines\*.dll", "sqldrivers\*.dll", "tls\*.dll") -Label "Qt plugins"
Assert-StageAnyMatch -StageDir $stageDir -Patterns @("translations\qt_*.qm", "translations\qtbase_*.qm") -Label "Qt translations"
Ensure-MsvcRuntimeDlls -StageDir $stageDir
Assert-StageAnyMatch -StageDir $stageDir -Patterns @("vcruntime*.dll", "msvcp*.dll") -Label "MSVC runtime DLLs"

$roots = [System.Collections.Generic.List[string]]::new()
foreach ($root in $DependencyRoots) { Add-Root $roots $root }
Add-Root $roots $env:MAVSDK_ROOT
Add-Root $roots $env:MAVSDK_DIR
Add-MavsdkHintRoots -Roots $roots -MavsdkDir $env:MAVSDK_DIR
Add-DxCompilerHintRoots -Roots $roots
Add-Root $roots $env:VCPKG_INSTALLED_DIR
Add-Root $roots (Split-Path $exePath -Parent)
Add-Root $roots (Join-Path $BuildDir "bin")
Add-Root $roots (Join-Path $BuildDir "bin\$Configuration")

Info "Dependency roots:$(Format-RootList -Roots $roots)"

$mavsdkCopied = Copy-FirstMatchingDll `
    -Patterns @("mavsdk*.dll", "libmavsdk*.dll") `
    -Roots $roots `
    -StageDir $stageDir `
    -Required `
    -Label "MAVSDK"

$rsvgCopied = Copy-FirstMatchingDll `
    -Patterns @("librsvg*.dll", "rsvg*.dll") `
    -Roots $roots `
    -StageDir $stageDir `
    -Required `
    -Label "librsvg"

$cairoCopied = Copy-FirstMatchingDll `
    -Patterns @("libcairo*.dll", "cairo*.dll") `
    -Roots $roots `
    -StageDir $stageDir `
    -Required `
    -Label "cairo"

$glibCopied = Copy-FirstMatchingDll `
    -Patterns @("glib-2*.dll", "libglib-2*.dll") `
    -Roots $roots `
    -StageDir $stageDir `
    -Required `
    -Label "glib-2.0"

$seedDirs = @()
$seedDllPaths = @()
$seedDllPaths += @($mavsdkCopied)
$seedDllPaths += @($rsvgCopied)
$seedDllPaths += @($cairoCopied)
$seedDllPaths += @($glibCopied)
foreach ($dllPath in $seedDllPaths) {
    if (-not $dllPath) {
        continue
    }
    $seedDirs += Split-Path -Path $dllPath -Parent
}
if ($seedDirs.Count -gt 0) {
    Info "Copying complete runtime DLL sets from detected dependency directories."
    Copy-AllDllsFromDirectories -Directories $seedDirs -StageDir $stageDir
}
if ($DependencyRoots.Count -gt 0) {
    Info "Copying top-level DLLs from explicit dependency roots."
    Copy-AllDllsFromDirectories -Directories $DependencyRoots -StageDir $stageDir
}

$runtimePatterns = @(
    "librsvg*.dll",
    "rsvg*.dll",
    "libcairo*.dll",
    "cairo*.dll",
    "glib-2*.dll",
    "libglib-2*.dll",
    "gobject-2*.dll",
    "libgobject-2*.dll",
    "gio-2*.dll",
    "libgio-2*.dll",
    "gmodule-2*.dll",
    "libgmodule-2*.dll",
    "gthread-2*.dll",
    "libgthread-2*.dll",
    "gdk_pixbuf-2*.dll",
    "libgdk_pixbuf-2*.dll",
    "gdk-pixbuf-2*.dll",
    "libgdk-pixbuf-2*.dll",
    "intl*.dll",
    "libintl*.dll",
    "iconv*.dll",
    "libiconv*.dll",
    "ffi*.dll",
    "libffi*.dll",
    "pango*.dll",
    "libpango*.dll",
    "harfbuzz*.dll",
    "libharfbuzz*.dll",
    "freetype*.dll",
    "libfreetype*.dll",
    "fontconfig*.dll",
    "libfontconfig*.dll",
    "pixman*.dll",
    "libpixman*.dll",
    "png*.dll",
    "libpng*.dll",
    "zlib*.dll",
    "libzlib*.dll",
    "bz2*.dll",
    "libbz2*.dll",
    "brotli*.dll",
    "libbrotli*.dll",
    "dxcompiler*.dll",
    "dxil*.dll",
    "xml2*.dll",
    "libxml2*.dll"
)
Copy-AllMatchingDlls -Patterns $runtimePatterns -Roots $roots -StageDir $stageDir
Ensure-GlibRuntimeAliases -StageDir $stageDir

if ($env:ARCGIS_RUNTIME_QT_PATH) {
    if (Test-PathAny -Path $env:ARCGIS_RUNTIME_QT_PATH -PathType "Container") {
        Info "Copying optional ArcGIS Runtime files"
        $arcGisRoot = Resolve-ExistingPath -Path $env:ARCGIS_RUNTIME_QT_PATH -Label "ARCGIS_RUNTIME_QT_PATH"
        Copy-AllMatchingDlls -Patterns @("RuntimeCoreNet*.dll", "ArcGISRuntime*.dll") -Roots @($arcGisRoot) -StageDir $stageDir
        $arcGisQml = @(
            (Join-Path $arcGisRoot "qml"),
            (Join-Path $arcGisRoot "sdk\qml"),
            (Join-Path $arcGisRoot "Resources\qml")
        ) | Where-Object { Test-PathAny -Path (Join-Path $_ "Esri\ArcGISRuntime\qmldir") -PathType "Leaf" } | Select-Object -First 1
        if ($arcGisQml) {
            $target = Join-Path $stageDir "qml\Esri"
            New-Item -ItemType Directory -Path (Split-Path $target -Parent) -Force | Out-Null
            Copy-Item -LiteralPath (Join-Path $arcGisQml "Esri") -Destination $target -Recurse -Force
        } else {
            Info "ARCGIS_RUNTIME_QT_PATH set but no Esri QML import was found; skipping ArcGIS QML copy."
        }
    } else {
        Info "ARCGIS_RUNTIME_QT_PATH is set but does not exist: $env:ARCGIS_RUNTIME_QT_PATH"
    }
}

$dumpbin = Find-CommandPath -Names @("dumpbin.exe") -HintDir ""
if ($dumpbin) {
    Info "Dependency summary from dumpbin:"
    & $dumpbin /DEPENDENTS (Join-Path $stageDir "SkyGridGCS.exe") | Select-String "\.dll"
} else {
    Info "dumpbin.exe not found; skipping dependency summary."
}

if ($SkipZip) {
    Info "Portable folder created: $stageDir"
    Info "Skipping portable zip because -SkipZip was specified."
} else {
    if (Test-PathAny -Path $zipPath -PathType "Leaf") {
        Remove-Item -LiteralPath $zipPath -Force
    }
    $archiveEntries = Get-ChildItem -LiteralPath $stageDir -Force | Select-Object -ExpandProperty FullName
    if (-not $archiveEntries) {
        Fail "Staging directory is empty: $stageDir"
    }
    Compress-Archive -LiteralPath $archiveEntries -DestinationPath $zipPath -Force
    Info "Portable release created: $zipPath"
}
