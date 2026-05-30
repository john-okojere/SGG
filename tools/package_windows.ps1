param(
    [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$BuildDir = "",
    [string]$Configuration = "Release",
    [string]$QtBinDir = "",
    [string[]]$DependencyRoots = @(),
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

function Fail($Message) {
    throw "[SkyGrid Windows Package] $Message"
}

function Info($Message) {
    Write-Host "[SkyGrid Windows Package] $Message"
}

function Find-CommandPath($Name, $HintDir) {
    if ($HintDir) {
        $candidate = Join-Path $HintDir $Name
        if (Test-Path $candidate) {
            return (Resolve-Path $candidate).Path
        }
    }
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }
    return ""
}

function Add-Root([System.Collections.Generic.List[string]]$Roots, [string]$Path) {
    if (-not $Path) { return }
    if (-not (Test-Path $Path)) { return }
    $resolved = (Resolve-Path $Path).Path
    if (-not $Roots.Contains($resolved)) {
        $Roots.Add($resolved)
    }
}

function Copy-FirstMatchingDll($Patterns, $Roots, $StageDir, [switch]$Required, $Label) {
    $copied = @()
    foreach ($pattern in $Patterns) {
        foreach ($root in $Roots) {
            $matches = Get-ChildItem -Path $root -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue |
                Where-Object { $_.FullName -notmatch "\\debug\\" -and $_.FullName -notmatch "\\Debug\\" }
            foreach ($match in $matches) {
                $dest = Join-Path $StageDir $match.Name
                if (-not (Test-Path $dest)) {
                    Copy-Item $match.FullName $dest
                    Info "Copied $($match.Name)"
                }
                $copied += $match.FullName
                break
            }
            if ($copied.Count -gt 0) { break }
        }
        if ($copied.Count -gt 0) { break }
    }
    if ($Required -and $copied.Count -eq 0) {
        Fail "Could not find required $Label DLL. Pass -DependencyRoots with the MAVSDK/runtime install folders."
    }
    return $copied
}

function Copy-AllMatchingDlls($Patterns, $Roots, $StageDir) {
    foreach ($pattern in $Patterns) {
        foreach ($root in $Roots) {
            $matches = Get-ChildItem -Path $root -Recurse -File -Filter $pattern -ErrorAction SilentlyContinue |
                Where-Object { $_.FullName -notmatch "\\debug\\" -and $_.FullName -notmatch "\\Debug\\" }
            foreach ($match in $matches) {
                $dest = Join-Path $StageDir $match.Name
                if (-not (Test-Path $dest)) {
                    Copy-Item $match.FullName $dest
                    Info "Copied $($match.Name)"
                }
            }
        }
    }
}

$RepoRoot = (Resolve-Path $RepoRoot).Path
if (-not $Configuration) {
    $Configuration = "Release"
}
if (-not $BuildDir) {
    $BuildDir = Join-Path $RepoRoot "build-windows-release"
}

$windeployqt = Find-CommandPath "windeployqt.exe" $QtBinDir
if (-not $windeployqt) {
    Fail "windeployqt.exe was not found. Add the Qt MSVC bin folder to PATH or pass -QtBinDir."
}

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
$exePath = $exeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $exePath) {
    Fail "SkyGridGCS.exe was not found in $BuildDir."
}
$exePath = (Resolve-Path $exePath).Path

$distRoot = Join-Path $RepoRoot "dist\windows"
$stageDir = Join-Path $distRoot "SkyGridGCS"
$zipPath = Join-Path $distRoot "SkyGridGCS_Windows_Portable.zip"

if (Test-Path $stageDir) {
    Remove-Item $stageDir -Recurse -Force
}
New-Item -ItemType Directory -Path $stageDir -Force | Out-Null
Copy-Item $exePath (Join-Path $stageDir "SkyGridGCS.exe")

$iniSource = Join-Path $RepoRoot "release\windows\SkyGridGCS.ini"
if (Test-Path $iniSource) {
    Copy-Item $iniSource (Join-Path $stageDir "SkyGridGCS.ini")
}
$readmeSource = Join-Path $RepoRoot "release\windows\README_WINDOWS_RELEASE.txt"
if (Test-Path $readmeSource) {
    Copy-Item $readmeSource (Join-Path $stageDir "README_WINDOWS_RELEASE.txt")
}

Info "Running windeployqt"
& $windeployqt --release --compiler-runtime --qmldir (Join-Path $RepoRoot "qml") (Join-Path $stageDir "SkyGridGCS.exe")
if ($LASTEXITCODE -ne 0) {
    Fail "windeployqt failed."
}

if (-not (Test-Path (Join-Path $stageDir "Qt6Core.dll"))) {
    Fail "Qt6Core.dll is missing after windeployqt."
}
if (-not (Test-Path (Join-Path $stageDir "platforms\qwindows.dll"))) {
    Fail "platforms\qwindows.dll is missing after windeployqt."
}

$roots = [System.Collections.Generic.List[string]]::new()
foreach ($root in $DependencyRoots) { Add-Root $roots $root }
Add-Root $roots $env:MAVSDK_ROOT
Add-Root $roots $env:MAVSDK_DIR
Add-Root $roots $env:VCPKG_INSTALLED_DIR
Add-Root $roots (Split-Path $exePath -Parent)
Add-Root $roots (Join-Path $BuildDir "bin")
Add-Root $roots (Join-Path $BuildDir "bin\$Configuration")

$mavsdkCopied = Copy-FirstMatchingDll `
    -Patterns @("mavsdk*.dll", "libmavsdk*.dll") `
    -Roots $roots `
    -StageDir $stageDir `
    -Required `
    -Label "MAVSDK"

$runtimePatterns = @(
    "librsvg*.dll",
    "rsvg*.dll",
    "libcairo*.dll",
    "cairo*.dll",
    "glib-2*.dll",
    "gobject-2*.dll",
    "gio-2*.dll",
    "gmodule-2*.dll",
    "gthread-2*.dll",
    "gdk_pixbuf-2*.dll",
    "gdk-pixbuf-2*.dll",
    "intl*.dll",
    "iconv*.dll",
    "ffi*.dll",
    "pango*.dll",
    "harfbuzz*.dll",
    "freetype*.dll",
    "fontconfig*.dll",
    "pixman*.dll",
    "png*.dll",
    "zlib*.dll",
    "bz2*.dll",
    "brotli*.dll",
    "xml2*.dll"
)
Copy-AllMatchingDlls -Patterns $runtimePatterns -Roots $roots -StageDir $stageDir

if ($env:ARCGIS_RUNTIME_QT_PATH -and (Test-Path $env:ARCGIS_RUNTIME_QT_PATH)) {
    Info "Copying optional ArcGIS Runtime files"
    Copy-AllMatchingDlls -Patterns @("RuntimeCoreNet*.dll", "ArcGISRuntime*.dll") -Roots @($env:ARCGIS_RUNTIME_QT_PATH) -StageDir $stageDir
    $arcGisQml = @(
        (Join-Path $env:ARCGIS_RUNTIME_QT_PATH "qml"),
        (Join-Path $env:ARCGIS_RUNTIME_QT_PATH "sdk\qml"),
        (Join-Path $env:ARCGIS_RUNTIME_QT_PATH "Resources\qml")
    ) | Where-Object { Test-Path (Join-Path $_ "Esri\ArcGISRuntime\qmldir") } | Select-Object -First 1
    if ($arcGisQml) {
        $target = Join-Path $stageDir "qml\Esri"
        New-Item -ItemType Directory -Path (Split-Path $target -Parent) -Force | Out-Null
        Copy-Item (Join-Path $arcGisQml "Esri") $target -Recurse -Force
    }
}

$dumpbin = Find-CommandPath "dumpbin.exe" ""
if ($dumpbin) {
    Info "Dependency summary from dumpbin:"
    & $dumpbin /DEPENDENTS (Join-Path $stageDir "SkyGridGCS.exe") | Select-String "\.dll"
} else {
    Info "dumpbin.exe not found; skipping dependency summary."
}

if (Test-Path $zipPath) {
    Remove-Item $zipPath -Force
}
Compress-Archive -Path (Join-Path $stageDir "*") -DestinationPath $zipPath -Force
Info "Portable release created: $zipPath"
