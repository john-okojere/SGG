param(
    [string]$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$BuildDir = "",
    [string]$Configuration = "Release",
    [string]$QtBinDir = "",
    [string[]]$DependencyRoots = @(),
    [string]$Version = "",
    [switch]$SkipBuild,
    [switch]$SkipPortablePackage
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version 2.0

function Fail($Message) {
    throw "[SkyGrid MSI Package] $Message"
}

function Info($Message) {
    Write-Host "[SkyGrid MSI Package] $Message"
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

function New-WixId([string]$Prefix, [string]$Text) {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text.ToLowerInvariant())
    $hash = [System.Security.Cryptography.SHA1]::Create().ComputeHash($bytes)
    $hex = -join ($hash[0..9] | ForEach-Object { $_.ToString("x2") })
    return "$Prefix$hex"
}

function ConvertTo-WixPath([string]$Path) {
    return $Path.Replace("\", "\\")
}

function Get-RelativePathCompat([string]$BasePath, [string]$TargetPath) {
    $baseFull = [System.IO.Path]::GetFullPath($BasePath)
    $targetFull = [System.IO.Path]::GetFullPath($TargetPath)
    if (-not $baseFull.EndsWith([System.IO.Path]::DirectorySeparatorChar)) {
        $baseFull += [System.IO.Path]::DirectorySeparatorChar
    }
    $baseUri = New-Object System.Uri($baseFull)
    $targetUri = New-Object System.Uri($targetFull)
    $relativeUri = $baseUri.MakeRelativeUri($targetUri)
    return [System.Uri]::UnescapeDataString($relativeUri.ToString()).Replace("/", "\")
}

function Get-CMakeProjectVersion([string]$CMakeFile) {
    $content = Get-Content -LiteralPath $CMakeFile -Raw
    $match = [regex]::Match($content, "project\s*\(\s*SkyGridGCS[\s\S]*?VERSION\s+([0-9]+(?:\.[0-9]+){0,3})", [System.Text.RegularExpressions.RegexOptions]::IgnoreCase)
    if ($match.Success) {
        $parts = $match.Groups[1].Value.Split(".")
        while ($parts.Count -lt 3) {
            $parts += "0"
        }
        return ($parts[0..2] -join ".")
    }
    return "0.1.0"
}

function Add-DirectoryTreeXml([System.Xml.XmlDocument]$Doc, [System.Xml.XmlElement]$ParentElement, [System.IO.DirectoryInfo]$Directory, [hashtable]$DirectoryIds, [string]$StageDir) {
    $directoryId = $DirectoryIds[$Directory.FullName]
    $element = $Doc.CreateElement("Directory", $Doc.DocumentElement.NamespaceURI)
    $element.SetAttribute("Id", $directoryId)
    $element.SetAttribute("Name", $Directory.Name)
    [void]$ParentElement.AppendChild($element)

    foreach ($child in Get-ChildItem -LiteralPath $Directory.FullName -Directory | Sort-Object FullName) {
        Add-DirectoryTreeXml -Doc $Doc -ParentElement $element -Directory $child -DirectoryIds $DirectoryIds -StageDir $StageDir
    }
}

function Add-FileComponents([System.Xml.XmlDocument]$Doc, [System.Xml.XmlElement]$Fragment, [System.Xml.XmlElement]$ComponentGroup, [string]$StageDir, [hashtable]$DirectoryIds) {
    $files = Get-ChildItem -LiteralPath $StageDir -Recurse -File | Sort-Object FullName
    foreach ($file in $files) {
        $relative = Get-RelativePathCompat -BasePath $StageDir -TargetPath $file.FullName
        $componentId = New-WixId -Prefix "cmp_" -Text $relative
        $fileId = New-WixId -Prefix "fil_" -Text $relative
        $parentDir = $file.Directory.FullName
        $directoryId = if ($parentDir -eq $StageDir) { "INSTALLFOLDER" } else { $DirectoryIds[$parentDir] }

        $component = $Doc.CreateElement("Component", $Doc.DocumentElement.NamespaceURI)
        $component.SetAttribute("Id", $componentId)
        $component.SetAttribute("Directory", $directoryId)
        $component.SetAttribute("Guid", "*")

        $fileElement = $Doc.CreateElement("File", $Doc.DocumentElement.NamespaceURI)
        $fileElement.SetAttribute("Id", $fileId)
        $fileElement.SetAttribute("Source", $file.FullName)
        $fileElement.SetAttribute("KeyPath", "yes")
        [void]$component.AppendChild($fileElement)
        [void]$Fragment.AppendChild($component)

        $ref = $Doc.CreateElement("ComponentRef", $Doc.DocumentElement.NamespaceURI)
        $ref.SetAttribute("Id", $componentId)
        [void]$ComponentGroup.AppendChild($ref)
    }
}

function New-WixSource([string]$StageDir, [string]$OutputPath, [string]$Version) {
    $namespace = "http://wixtoolset.org/schemas/v4/wxs"
    $doc = New-Object System.Xml.XmlDocument
    $doc.PreserveWhitespace = $true
    $wix = $doc.CreateElement("Wix", $namespace)
    [void]$doc.AppendChild($wix)

    $package = $doc.CreateElement("Package", $namespace)
    $package.SetAttribute("Name", "SkyGrid GCS")
    $package.SetAttribute("Manufacturer", "SkyGrid")
    $package.SetAttribute("Version", $Version)
    $package.SetAttribute("UpgradeCode", "{7E4E0B62-5C0D-4D87-A14E-5A92040F8712}")
    $package.SetAttribute("Scope", "perMachine")
    [void]$wix.AppendChild($package)

    $majorUpgrade = $doc.CreateElement("MajorUpgrade", $namespace)
    $majorUpgrade.SetAttribute("DowngradeErrorMessage", "A newer version of SkyGrid GCS is already installed.")
    [void]$package.AppendChild($majorUpgrade)

    $media = $doc.CreateElement("MediaTemplate", $namespace)
    $media.SetAttribute("EmbedCab", "yes")
    [void]$package.AppendChild($media)

    $programFiles = $doc.CreateElement("StandardDirectory", $namespace)
    $programFiles.SetAttribute("Id", "ProgramFiles64Folder")
    [void]$package.AppendChild($programFiles)

    $companyDir = $doc.CreateElement("Directory", $namespace)
    $companyDir.SetAttribute("Id", "SKYGRIDFOLDER")
    $companyDir.SetAttribute("Name", "SkyGrid")
    [void]$programFiles.AppendChild($companyDir)

    $installDir = $doc.CreateElement("Directory", $namespace)
    $installDir.SetAttribute("Id", "INSTALLFOLDER")
    $installDir.SetAttribute("Name", "SkyGrid GCS")
    [void]$companyDir.AppendChild($installDir)

    $directoryIds = @{}
    foreach ($dir in Get-ChildItem -LiteralPath $StageDir -Recurse -Directory | Sort-Object FullName) {
        $relative = Get-RelativePathCompat -BasePath $StageDir -TargetPath $dir.FullName
        $directoryIds[$dir.FullName] = New-WixId -Prefix "dir_" -Text $relative
    }
    foreach ($dir in Get-ChildItem -LiteralPath $StageDir -Directory | Sort-Object FullName) {
        Add-DirectoryTreeXml -Doc $doc -ParentElement $installDir -Directory $dir -DirectoryIds $directoryIds -StageDir $StageDir
    }

    $programMenu = $doc.CreateElement("StandardDirectory", $namespace)
    $programMenu.SetAttribute("Id", "ProgramMenuFolder")
    [void]$package.AppendChild($programMenu)

    $appMenu = $doc.CreateElement("Directory", $namespace)
    $appMenu.SetAttribute("Id", "ApplicationProgramsFolder")
    $appMenu.SetAttribute("Name", "SkyGrid GCS")
    [void]$programMenu.AppendChild($appMenu)

    $shortcutComponent = $doc.CreateElement("Component", $namespace)
    $shortcutComponent.SetAttribute("Id", "ApplicationShortcutComponent")
    $shortcutComponent.SetAttribute("Guid", "{3B553D5E-B4E3-4F7F-A33F-0C6C6F5B8D73}")
    [void]$appMenu.AppendChild($shortcutComponent)

    $shortcut = $doc.CreateElement("Shortcut", $namespace)
    $shortcut.SetAttribute("Id", "ApplicationStartMenuShortcut")
    $shortcut.SetAttribute("Name", "SkyGrid GCS")
    $shortcut.SetAttribute("Target", "[INSTALLFOLDER]SkyGridGCS.exe")
    $shortcut.SetAttribute("WorkingDirectory", "INSTALLFOLDER")
    [void]$shortcutComponent.AppendChild($shortcut)

    $removeFolder = $doc.CreateElement("RemoveFolder", $namespace)
    $removeFolder.SetAttribute("Id", "ApplicationProgramsFolder")
    $removeFolder.SetAttribute("On", "uninstall")
    [void]$shortcutComponent.AppendChild($removeFolder)

    $registry = $doc.CreateElement("RegistryValue", $namespace)
    $registry.SetAttribute("Root", "HKCU")
    $registry.SetAttribute("Key", "Software\SkyGrid\SkyGridGCS")
    $registry.SetAttribute("Name", "installed")
    $registry.SetAttribute("Type", "integer")
    $registry.SetAttribute("Value", "1")
    $registry.SetAttribute("KeyPath", "yes")
    [void]$shortcutComponent.AppendChild($registry)

    $fragment = $doc.CreateElement("Fragment", $namespace)
    [void]$wix.AppendChild($fragment)

    $componentGroup = $doc.CreateElement("ComponentGroup", $namespace)
    $componentGroup.SetAttribute("Id", "AppFiles")
    [void]$fragment.AppendChild($componentGroup)
    Add-FileComponents -Doc $doc -Fragment $fragment -ComponentGroup $componentGroup -StageDir $StageDir -DirectoryIds $directoryIds

    $feature = $doc.CreateElement("Feature", $namespace)
    $feature.SetAttribute("Id", "MainFeature")
    $feature.SetAttribute("Title", "SkyGrid GCS")
    $feature.SetAttribute("Level", "1")
    [void]$package.AppendChild($feature)

    $componentGroupRef = $doc.CreateElement("ComponentGroupRef", $namespace)
    $componentGroupRef.SetAttribute("Id", "AppFiles")
    [void]$feature.AppendChild($componentGroupRef)

    $shortcutRef = $doc.CreateElement("ComponentRef", $namespace)
    $shortcutRef.SetAttribute("Id", "ApplicationShortcutComponent")
    [void]$feature.AppendChild($shortcutRef)

    $doc.Save($OutputPath)
}

if ([System.Environment]::OSVersion.Platform -ne [System.PlatformID]::Win32NT) {
    Fail "This MSI packager only supports Windows hosts."
}

$RepoRoot = Resolve-ExistingPath -Path $RepoRoot -Label "RepoRoot"
if (-not $BuildDir) {
    $BuildDir = Join-Path $RepoRoot "build-windows-release"
}
if (-not $SkipPortablePackage) {
    $BuildDir = Resolve-ExistingPath -Path $BuildDir -Label "BuildDir"
}
if (-not $Version) {
    $Version = Get-CMakeProjectVersion -CMakeFile (Join-Path $RepoRoot "CMakeLists.txt")
}

$portableScript = Join-Path $RepoRoot "tools\package_windows.ps1"
$distRoot = Join-Path $RepoRoot "dist\windows"
$stageDir = Join-Path $distRoot "SkyGridGCS"
$msiWorkDir = Join-Path $distRoot "msi"
$wxsPath = Join-Path $msiWorkDir "SkyGridGCS.wxs"
$msiPath = Join-Path $distRoot "SkyGridGCS_Setup.msi"

if (-not $SkipPortablePackage) {
    $portableArgs = @(
        "-NoProfile",
        "-ExecutionPolicy", "Bypass",
        "-File", $portableScript,
        "-RepoRoot", $RepoRoot,
        "-BuildDir", $BuildDir,
        "-Configuration", $Configuration,
        "-SkipZip"
    )
    if ($SkipBuild) {
        $portableArgs += "-SkipBuild"
    }
    if ($QtBinDir) {
        $portableArgs += @("-QtBinDir", $QtBinDir)
    }
    if ($DependencyRoots.Count -gt 0) {
        $portableArgs += "-DependencyRoots"
        $portableArgs += $DependencyRoots
    }

    Info "Creating portable staging folder for MSI input."
    & powershell @portableArgs
    if ($LASTEXITCODE -ne 0) {
        Fail "Portable staging failed."
    }
}

$stageDir = Resolve-ExistingPath -Path $stageDir -Label "Portable stage directory"
Resolve-ExistingPath -Path (Join-Path $stageDir "SkyGridGCS.exe") -Label "Staged SkyGridGCS.exe" | Out-Null

New-Item -ItemType Directory -Path $msiWorkDir -Force | Out-Null
if (Test-Path -LiteralPath $msiPath -PathType Leaf) {
    Remove-Item -LiteralPath $msiPath -Force
}

Info "Generating WiX source: $wxsPath"
New-WixSource -StageDir $stageDir -OutputPath $wxsPath -Version $Version

Info "Building MSI: $msiPath"
dotnet tool restore
if ($LASTEXITCODE -ne 0) {
    Fail "dotnet tool restore failed. Run 'dotnet tool install wix' from the repo if WiX is missing."
}

dotnet tool run wix build $wxsPath -arch x64 -out $msiPath
if ($LASTEXITCODE -ne 0) {
    Fail "WiX MSI build failed."
}

Info "MSI created: $msiPath"
