# Package script - PowerShell version
$ErrorActionPreference = "Stop"

$ProjectDir = "$PSScriptRoot"
$BinDir = "$ProjectDir\bin"
$DistDir = "$ProjectDir\dist\game"
$AssetsDir = "$ProjectDir\assets"
$ExeName = "game.exe"
$Timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$ZipName = "sfmlgame-$Timestamp.zip"

Write-Host "=== Step 1: Verify exe ==="
$ExePath = "$BinDir\$ExeName"
if (-not (Test-Path $ExePath)) {
    Write-Host "ERROR: $ExePath not found!"
    exit 1
}
Write-Host "  $ExeName : OK"

Write-Host ""
Write-Host "=== Step 2: Collect DLLs ==="
# System DLLs to skip
$SystemDlls = @(
    "KERNEL32.DLL", "ADVAPI32.DLL", "SHELL32.DLL", "USER32.DLL",
    "GDI32.DLL", "OPENGL32.DLL", "WINMM.DLL", "WS2_32.DLL",
    "OLE32.DLL", "COMCTL32.DLL", "CRYPT32.DLL", "BCRYPT.DLL",
    "MSVCRT.DLL", "UCRTBASE.DLL", "SETUPAPI.DLL", "IPHLPAPI.DLL",
    "POWRPROF.DLL", "SHLWAPI.DLL", "KERNELBASE.DLL", "RPCRT4.DLL",
    "IMM32.DLL", "OLEAUT32.DLL", "SECHOST.DLL", "SSPICLI.DLL",
    "VCRUNTIME140.DLL", "VCRUNTIME140D.DLL", "MSVCP140.DLL", "MSVCP140D.DLL",
    "API-MS-WIN-*.DLL", "EXT-MS-WIN-*.DLL"
)

$NeededDlls = @()

# Check common SFML deps
$SearchDirs = @($BinDir, "C:\msys64\ucrt64\bin")
$KnownDlls = @(
    "libsfml-graphics-3.dll", "libsfml-window-3.dll", "libsfml-system-3.dll",
    "libsfml-audio-3.dll", "libsfml-network-3.dll",
    "libfreetype-6.dll", "libopenal-1.dll",
    "libogg-0.dll",
    "libvorbis-0.dll", "libvorbisenc-2.dll", "libvorbisfile-3.dll",
    "libFLAC.dll",
    "libpng16-16.dll",
    "libbz2-1.dll",
    "libzstd.dll",
    "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll"
)

foreach ($dll in $KnownDlls) {
    foreach ($dir in $SearchDirs) {
        $path = "$dir\$dll"
        if (Test-Path $path) {
            $NeededDlls += $path
            break
        }
    }
}

Write-Host ""
Write-Host "=== Step 3: Prepare dist/ ==="
if (Test-Path $DistDir) {
    Remove-Item -Recurse -Force $DistDir
}
New-Item -ItemType Directory -Force -Path $DistDir | Out-Null

Write-Host "=== Step 4: Copy executable ==="
Copy-Item $ExePath "$DistDir\"
Write-Host "  + $ExeName"

Write-Host "=== Step 5: Copy DLLs ==="
foreach ($dll in $NeededDlls) {
    $name = Split-Path $dll -Leaf
    Copy-Item $dll "$DistDir\"
    Write-Host "  + $name"
}

Write-Host "=== Step 6: Copy assets ==="
Copy-Item -Recurse $AssetsDir "$DistDir\assets"
Write-Host "  assets/: OK"

Write-Host ""
Write-Host "=== Step 7: Create zip ==="
$ZipPath = "$ProjectDir\dist\$ZipName"
Compress-Archive -Path $DistDir -DestinationPath $ZipPath -Force
Write-Host "  -> dist\$ZipName"

Write-Host ""
Write-Host "=== Package Complete ==="
Write-Host "Folder:  dist\game\"
Write-Host "Archive: dist\$ZipName"
Write-Host "Run: dist\game\game.exe"
