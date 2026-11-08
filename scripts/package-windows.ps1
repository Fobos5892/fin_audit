<#
.SYNOPSIS
    Builds FamilyBudget, collects Qt runtime via windeployqt and compiles the Inno Setup installer.

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File scripts\package-windows.ps1
    powershell -ExecutionPolicy Bypass -File scripts\package-windows.ps1 -SkipBuild
#>
param(
    [string]$Preset = "windows-mingw-release",
    [string]$QtDir = "C:\Qt\6.11.1\mingw_64",
    [string]$MinGWDir = "C:\Qt\Tools\mingw1310_64",
    [switch]$SkipBuild,
    [switch]$SkipInstaller
)

$ErrorActionPreference = "Stop"

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildDir = Join-Path $root "build\$Preset"
$distDir = Join-Path $root "build\dist\FinAudit"
$installerOut = Join-Path $root "build\installer"
$issFile = Join-Path $root "installer\fin_audit.iss"

# windeployqt needs the compiler runtime on PATH to bundle libgcc/libstdc++.
$env:PATH = "$MinGWDir\bin;$QtDir\bin;C:\Qt\Tools\Ninja;$env:PATH"

function Get-ProjectVersion {
    $cmake = Get-Content (Join-Path $root "CMakeLists.txt") -Raw
    if ($cmake -match 'VERSION\s+(\d+\.\d+\.\d+)') { return $Matches[1] }
    return "1.0.0"
}

function Get-PngSize([string]$path) {
    $bytes = [IO.File]::ReadAllBytes($path)
    $w = ($bytes[16] -shl 24) -bor ($bytes[17] -shl 16) -bor ($bytes[18] -shl 8) -bor $bytes[19]
    $h = ($bytes[20] -shl 24) -bor ($bytes[21] -shl 16) -bor ($bytes[22] -shl 8) -bor $bytes[23]
    return @{ Width = $w; Height = $h; Bytes = $bytes }
}

function New-IcoFromPngs([string[]]$pngPaths, [string]$outPath) {
    $images = $pngPaths | ForEach-Object { Get-PngSize $_ }
    $ms = New-Object IO.MemoryStream
    $bw = New-Object IO.BinaryWriter($ms)
    $bw.Write([UInt16]0)
    $bw.Write([UInt16]1)
    $bw.Write([UInt16]$images.Count)
    $offset = 6 + 16 * $images.Count
    foreach ($img in $images) {
        # ICO stores 256 px as 0.
        $bw.Write([byte]($img.Width -band 0xFF))
        $bw.Write([byte]($img.Height -band 0xFF))
        $bw.Write([byte]0)
        $bw.Write([byte]0)
        $bw.Write([UInt16]1)
        $bw.Write([UInt16]32)
        $bw.Write([UInt32]$img.Bytes.Length)
        $bw.Write([UInt32]$offset)
        $offset += $img.Bytes.Length
    }
    foreach ($img in $images) { $bw.Write($img.Bytes) }
    $bw.Flush()
    [IO.File]::WriteAllBytes($outPath, $ms.ToArray())
    $bw.Dispose()
}

function Find-Iscc {
    $candidates = @(
        "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
        "$env:ProgramFiles\Inno Setup 6\ISCC.exe",
        "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe"
    )
    foreach ($c in $candidates) { if (Test-Path $c) { return $c } }
    $cmd = Get-Command iscc -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    return $null
}

$version = Get-ProjectVersion
Write-Host "FamilyBudget $version" -ForegroundColor Cyan

if (-not $SkipBuild) {
    Write-Host "==> Building ($Preset)" -ForegroundColor Cyan
    Push-Location $root
    try {
        cmake --preset $Preset
        if ($LASTEXITCODE -ne 0) { throw "cmake configure failed" }
        cmake --build --preset $Preset
        if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }
    } finally {
        Pop-Location
    }
}

$exe = Join-Path $buildDir "FamilyBudget.exe"
if (-not (Test-Path $exe)) { throw "Executable not found: $exe" }

Write-Host "==> Collecting files into $distDir" -ForegroundColor Cyan
if (Test-Path $distDir) { Remove-Item $distDir -Recurse -Force }
New-Item -ItemType Directory -Path $distDir | Out-Null
Copy-Item $exe $distDir

$windeployqt = Join-Path $QtDir "bin\windeployqt.exe"
if (-not (Test-Path $windeployqt)) { throw "windeployqt not found: $windeployqt" }
& $windeployqt --qmldir (Join-Path $root "qml") --no-system-d3d-compiler (Join-Path $distDir "FamilyBudget.exe")
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed" }

$icons = Join-Path $root "resources\icons"
New-IcoFromPngs @(
    (Join-Path $icons "app_icon_32.png"),
    (Join-Path $icons "app_icon_48.png"),
    (Join-Path $icons "app_icon_64.png"),
    (Join-Path $icons "app_icon_128.png"),
    (Join-Path $icons "app_icon_256.png")
) (Join-Path $distDir "app.ico")

Write-Host "Portable build ready: $distDir" -ForegroundColor Green

if ($SkipInstaller) { return }

$iscc = Find-Iscc
if (-not $iscc) {
    Write-Warning "Inno Setup 6 not found. Install it and re-run with -SkipBuild:"
    Write-Warning "  winget install JRSoftware.InnoSetup"
    return
}

Write-Host "==> Compiling installer" -ForegroundColor Cyan
New-Item -ItemType Directory -Path $installerOut -Force | Out-Null
& $iscc "/DAppVersion=$version" "/DSourceDir=$distDir" "/DOutputDir=$installerOut" $issFile
if ($LASTEXITCODE -ne 0) { throw "ISCC failed" }

Write-Host "Installer ready: $installerOut\FinAudit-Setup-$version.exe" -ForegroundColor Green
