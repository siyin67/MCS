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

Write-Host ""
Write-Host "=== Step 2: Scan DLL dependencies ==="

$SearchDirs = @($BinDir, "C:\msys64\ucrt64\bin")
$SysDlls = @("KERNEL32.dll", "USER32.dll", "GDI32.dll", "ADVAPI32.dll", "SHELL32.dll",
    "OPENGL32.dll", "WINMM.dll", "WS2_32.dll", "OLE32.dll", "COMCTL32.dll", "CRYPT32.dll",
    "BCRYPT.dll", "MSVCRT.dll", "UCRTBASE.dll", "KERNELBASE.dll", "RPCRT4.dll", "IMM32.dll",
    "OLEAUT32.dll", "SHLWAPI.dll", "SETUPAPI.dll", "IPHLPAPI.dll", "POWRPROF.dll", "SECHOST.dll",
    "SSPICLI.dll", "WTSAPI32.dll", "NTDLL.dll", "VERSION.dll", "DWrite.dll", "USP10.dll", "WININET.dll")

$NeededDlls = @()
$Scanned = @{}  # track already-scanned DLLs to avoid infinite recursion

$Objdump = "C:\msys64\ucrt64\bin\objdump.exe"

function Resolve-DllDeps($Exe, $Depth) {
    if ($Depth -gt 3) { return }  # max recursion depth
    if ($Scanned.ContainsKey($Exe)) { return }
    $Scanned[$Exe] = $true

    $deps = & $Objdump -p $Exe 2>$null | Select-String "DLL Name" | ForEach-Object { ($_ -split "DLL Name:")[-1].Trim() }

    foreach ($dll in $deps) {
        $lower = $dll.ToLower()
        $isSys = $SysDlls | Where-Object { $lower -eq $_ }
        if ($isSys -or $lower.StartsWith("api-ms-win") -or $lower.StartsWith("ext-ms-win")) {
            continue
        }

        $found = $null
        foreach ($dir in $SearchDirs) {
            $p = Join-Path $dir $dll
            if (Test-Path $p) { $found = $p; break }
        }
        if (-not $found) {
            Write-Host "  WARNING: $dll not found in search paths"
            continue
        }

        if ($NeededDlls -notcontains $found) {
            $NeededDlls += $found
            Resolve-DllDeps $found ($Depth + 1)
        }
    }
}

Write-Host "  Scanning $ExePath ..."
Resolve-DllDeps $ExePath 0
Write-Host "  Found $($NeededDlls.Count) DLL(s):"
foreach ($dll in $NeededDlls) {
    Write-Host "    + $(Split-Path $dll -Leaf)"
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
Copy-Item "$ProjectDir\README.md" "$DistDir\" -ErrorAction SilentlyContinue
Write-Host "  assets/: OK"
Write-Host "  README.md"
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
