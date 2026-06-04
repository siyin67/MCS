# Package script - PowerShell version
$ErrorActionPreference = "Stop"

$ProjectDir = "$PSScriptRoot"
$BinDir = "$ProjectDir\bin"
$DistDir = "$ProjectDir\dist\game"
$AssetsDir = "$ProjectDir\assets"
$ExeName = "game.exe"
$Timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$ZipName = "sfmlgame-$Timestamp.zip"

Write-Host "=== Step 0: Build ==="
$BuildScript = Join-Path $ProjectDir "build.ps1"
if (Test-Path $BuildScript) {
    & $BuildScript
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
else {
    Write-Host "  build.ps1 not found, using existing exe"
}

Write-Host ""
Write-Host "=== Step 1: Verify exe ==="
$ExePath = "$BinDir\$ExeName"
if (-not (Test-Path $ExePath)) {
    Write-Host "ERROR: $ExePath not found!"
    exit 1
}
Write-Host "  $ExeName : OK"

# ============================================================================
# Auto-detect ALL MSYS2 / MinGW installations
# ============================================================================
Write-Host ""
Write-Host "=== Step 2: Detect MSYS2/MinGW ==="

function Find-AllMsys2Bins {
    $found = @()
    $candidates = @()

    # 1. Common paths
    foreach ($base in @("C:\msys64", "D:\msys64", "E:\msys64", "C:\mingw64", "D:\mingw64", "E:\mingw64")) {
        foreach ($env in @("ucrt64\bin", "mingw64\bin", "clang64\bin", "bin")) {
            $candidates += (Join-Path $base $env)
        }
    }
    # 2. Environment variable
    if ($env:MSYS2_PATH) {
        $candidates += "$env:MSYS2_PATH\ucrt64\bin"
        $candidates += "$env:MSYS2_PATH\mingw64\bin"
    }
    # 3. PATH entries
    foreach ($entry in $env:PATH -split ';') {
        if ($entry -match "(mingw|msys|ucrt64)") {
            $candidates += $entry.TrimEnd('\')
        }
    }
    # 4. Use gcc location
    $gcc = Get-Command gcc -ErrorAction SilentlyContinue
    if ($gcc) {
        $candidates += (Split-Path $gcc.Source -Parent)
    }

    $seen = @{}
    foreach ($c in $candidates) {
        if (-not $c) { continue }
        $normalized = (Resolve-Path $c -ErrorAction SilentlyContinue).Path
        if (-not $normalized) { continue }
        if ($seen.ContainsKey($normalized)) { continue }
        $seen[$normalized] = $true

        if ((Test-Path "$normalized\libgcc_s_seh-1.dll") -or (Test-Path "$normalized\objdump.exe")) {
            $found += $normalized
        }
    }
    return $found
}

$AllMsys2Bins = Find-AllMsys2Bins

if (-not $AllMsys2Bins) {
    Write-Host "  ERROR: No MinGW/MSYS2 installation detected."
    Write-Host "  Make sure MSYS2 is installed at C:\msys64 or MinGW-w64 is on PATH."
    exit 1
}
Write-Host "  Found $($AllMsys2Bins.Count) MinGW location(s):"
foreach ($b in $AllMsys2Bins) { Write-Host "    $b" }

# objdump: pick first available
$Objdump = $null
foreach ($b in $AllMsys2Bins) {
    if (Test-Path "$b\objdump.exe") { $Objdump = "$b\objdump.exe"; break }
}
if ($Objdump) {
    Write-Host "  objdump: $Objdump"
}
else {
    Write-Host "  objdump: NOT FOUND (will use static DLL list)"
}

# Search paths: project bin + all MSYS2 bins
$SearchDirs = @($BinDir) + $AllMsys2Bins

# ============================================================================
# Known required DLL list (searched in all MSYS2 bins)
# ============================================================================
$CriticalDlls = @(
    # GCC/MinGW runtime (must have)
    "libgcc_s_seh-1.dll", "libstdc++-6.dll", "libwinpthread-1.dll",
    # SFML (from bin/)
    "libsfml-system-3.dll", "libsfml-window-3.dll", "libsfml-graphics-3.dll",
    "libsfml-audio-3.dll", "libsfml-network-3.dll",
    # Audio codec chain (from MSYS2)
    "libFLAC.dll", "libogg-0.dll", "libvorbis-0.dll", "libvorbisenc-2.dll",
    "libvorbisfile-3.dll", "libopenal-1.dll",
    # Graphics / font chain (from MSYS2)
    "libfreetype-6.dll", "libharfbuzz-0.dll", "libpng16-16.dll",
    "libbz2-1.dll", "zlib1.dll", "libbrotlidec.dll", "libbrotlicommon.dll",
    # Deep transitive (depth 5+ from harfbuzz → glib → intl → iconv)
    "libintl-8.dll", "libiconv-2.dll", "libpcre2-8-0.dll", "libglib-2.0-0.dll",
    "libgraphite2.dll"
)

# ============================================================================
# System DLL filter
# ============================================================================
$SysDllPatterns = @(
    "KERNEL32", "USER32", "GDI32", "ADVAPI32", "SHELL32", "OPENGL32",
    "WINMM", "WS2_32", "OLE32", "COMCTL32", "CRYPT32", "BCRYPT",
    "MSVCRT", "UCRTBASE", "KERNELBASE", "RPCRT4", "IMM32", "OLEAUT32",
    "SHLWAPI", "SETUPAPI", "IPHLPAPI", "POWRPROF", "SECHOST", "SSPICLI",
    "WTSAPI32", "NTDLL", "VERSION", "DWrite", "USP10", "WININET",
    "CFGMGR32", "DNSAPI", "NSI", "dbghelp", "dxgi", "d3d11", "dcomp",
    "WINTRUST", "MSASN1", "CRYPTBASE", "gdi32full", "win32u", "ucrtbase",
    "bcryptprimitives", "NETAPI32", "combase", "AVICAP32", "AVIFIL32",
    "MSACM32", "MSVFW32", "WINSPOOL", "COMDLG32", "HID", "MSIMG32",
    "PSAPI", "WLDAP32", "NETUTILS", "LOGONCLI", "SAMCLI", "Secur32",
    "SPOOLSS", "MPR", "ACTIVEDS", "adsldpc", "WINSTA", "CLBCatQ",
    "dhcpcsvc", "DPAPI", "DSROLE", "dxva2", "GLU32", "ktmw32",
    "LZ32", "MSCTF", "MSVFW32", "NTDSAPI", "PROPSYS", "RSTRTMGR",
    "SensApi", "Srvcli", "wevtapi", "Wkscli", "wtsapi32"
)

function Test-SystemDll($dllName) {
    $lower = $dllName.ToLower()
    if ($lower.StartsWith("api-ms-win") -or $lower.StartsWith("ext-ms-win")) { return $true }
    foreach ($p in $SysDllPatterns) {
        if ($lower -eq ($p.ToLower() + ".dll")) { return $true }
    }
    return $false
}

# ============================================================================
# Step 3: Scan DLL dependencies
# ============================================================================
Write-Host ""
Write-Host "=== Step 3: Scan DLL dependencies ==="

$NeededDlls = @{}    # basename -> fullpath, to avoid duplicates
$Scanned = @{}       # track scanned files

function Add-Dll($fullPath, $source) {
    $name = Split-Path $fullPath -Leaf
    if (-not $NeededDlls.ContainsKey($name)) {
        $NeededDlls[$name] = $fullPath
        Write-Host "    + $name  ($source)"
    }
}

function Resolve-One($filePath, $depth) {
    if ($depth -gt 6) { return }
    if ($Scanned.ContainsKey($filePath)) { return }
    $Scanned[$filePath] = $true

    if (-not $Objdump) { return }

    $raw = & $Objdump -p $filePath 2>&1
    $lines = if ($raw -is [string]) { @($raw) } else { $raw }
    $dlls = $lines | Select-String "DLL Name" | ForEach-Object { ($_.Line -split "DLL Name:")[-1].Trim() }

    foreach ($dll in $dlls) {
        if (Test-SystemDll $dll) { continue }

        $found = $null
        foreach ($dir in $SearchDirs) {
            $p = Join-Path $dir $dll
            if (Test-Path $p) { $found = $p; break }
        }
        if ($found) {
            Add-Dll $found "objdump depth=$depth"
            Resolve-One $found ($depth + 1)
        }
        else {
            Write-Host "    ? $dll not found (maybe optional)"
        }
    }
}

Write-Host "  Scanning dependencies..."
# 1. objdump-based recursive scan (if available)
Resolve-One $ExePath 0
foreach ($dllFile in Get-ChildItem "$BinDir\*.dll" -ErrorAction SilentlyContinue) {
    Resolve-One $dllFile.FullName 0
}

# 2. Always add all SFML DLLs from bin/
foreach ($dll in Get-ChildItem "$BinDir\*.dll" -ErrorAction SilentlyContinue) {
    Add-Dll $dll.FullName "SFML bin/"
}

# 3. Fallback: add critical DLLs from any found MSYS2 bin
foreach ($dllName in $CriticalDlls) {
    if ($NeededDlls.ContainsKey($dllName)) { continue }
    foreach ($dir in $SearchDirs) {
        $p = Join-Path $dir $dllName
        if (Test-Path $p) { Add-Dll $p "critical list"; break }
    }
}

if ($NeededDlls.Count -eq 0) {
    Write-Host "  ERROR: No DLLs found! Check MSYS2 installation."
    exit 1
}
Write-Host "  Total: $($NeededDlls.Count) DLL(s)"

# ============================================================================
# Step 4-7: Prepare dist, copy everything
# ============================================================================
Write-Host ""
Write-Host "=== Step 4: Prepare dist/ ==="
if (Test-Path $DistDir) { Remove-Item -Recurse -Force $DistDir }
New-Item -ItemType Directory -Force -Path $DistDir | Out-Null

Write-Host "=== Step 5: Copy executable ==="
Copy-Item $ExePath "$DistDir\"
Write-Host "  + $ExeName"

Write-Host "=== Step 6: Copy DLLs ==="
foreach ($entry in $NeededDlls.GetEnumerator()) {
    Copy-Item $entry.Value "$DistDir\"
    Write-Host "  + $($entry.Key)"
}

Write-Host "=== Step 7: Copy assets ==="
Copy-Item -Recurse $AssetsDir "$DistDir\assets"
Copy-Item "$ProjectDir\README.md" "$DistDir\" -ErrorAction SilentlyContinue
Write-Host "  assets/: OK"

# ============================================================================
# Step 8: Verify
# ============================================================================
Write-Host ""
Write-Host "=== Step 8: Verify package ==="
if ($Objdump) {
    $missing = @()
    $exeInDist = "$DistDir\$ExeName"
    $raw = & $Objdump -p $exeInDist 2>&1
    $lines = if ($raw -is [string]) { @($raw) } else { $raw }
    $verDeps = $lines | Select-String "DLL Name" | ForEach-Object { ($_.Line -split "DLL Name:")[-1].Trim() }
    foreach ($dll in $verDeps) {
        if (Test-SystemDll $dll) { continue }
        if (-not (Test-Path "$DistDir\$dll")) {
            $missing += $dll
        }
    }
    if ($missing.Count -eq 0) {
        Write-Host "  OK - All required DLLs present!"
    }
    else {
        Write-Host "  MISSING ($($missing.Count)):"
        foreach ($d in $missing) { Write-Host "    ! $d" }
    }
}
else {
    Write-Host "  Skipped (objdump not available)"
}

# ============================================================================
# Step 9: Zip
# ============================================================================
Write-Host ""
Write-Host "=== Step 9: Create zip ==="
$ZipPath = "$ProjectDir\dist\$ZipName"
Compress-Archive -Path $DistDir -DestinationPath $ZipPath -Force
Write-Host "  -> dist\$ZipName"

Write-Host ""
Write-Host "=== Package Complete ==="
Write-Host "Folder:  dist\game\"
Write-Host "Archive: dist\$ZipName"
Write-Host "Run:     dist\game\game.exe"
Write-Host ""
Write-Host "Send the ZIP file to the other computer."
Write-Host "The recipient MUST extract ALL files (exe + DLLs + assets) into the same folder."
