# Build script - incremental compile + link game.exe
$ErrorActionPreference = "Stop"

$Gpp = "C:\msys64\ucrt64\bin\g++.exe"
$Out = "bin\game.exe"
$ObjDir = "obj"
$Include = "C:/msys64/ucrt64/include"
$Lib = "C:/msys64/ucrt64/lib"

$Src = @(
    "src/main.cpp"
    "src/Application/Application.cpp"
    "src/Application/Statemanager.cpp"
    "src/Application/States/MenuState.cpp"
    "src/Application/States/GameState.cpp"
    "src/Application/States/PauseState.cpp"
    "src/Application/States/GameOverState.cpp"
    "src/Application/States/UpgradeState.cpp"
    "src/Application/States/SettingsState.cpp"
    "src/Application/States/SoundOptionsState.cpp"
    "src/Application/States/LanguageState.cpp"
    "src/Localization/Localization.cpp"
    "src/Localization/UiText.cpp"
    "src/Collision/Collision.cpp"
    "src/Collision/CollisionResolver.cpp"
    "src/Sound/SoundManager.cpp"
    "src/Monster/Monster.cpp"
    "src/Monster/Spawner.cpp"
    "src/Monster/Zombie.cpp"
    "src/Monster/Skeleton.cpp"
    "src/Monster/Spider.cpp"
    "src/Monster/Drowned.cpp"
    "src/Monster/Breeze.cpp"
    "src/Monster/Creeper.cpp"
    "src/Monster/Enderman.cpp"
    "src/Monster/Witch.cpp"
    "src/Monster/Blaze.cpp"
    "src/Monster/WitherSkeleton.cpp"
    "src/Monster/Ghast.cpp"
    "src/Monster/Wither.cpp"
    "src/Monster/Enderdragon.cpp"
    "src/Resourcemanager/ResourceManager.cpp"
    "src/Player/player.cpp"
    "src/UI/UIManager.cpp"
    "src/Weapon/weapon.cpp"
    "src/Weapon/WeaponFactory.cpp"
    "src/weapon/WeaponManager.cpp"
    "src/weapon/SwordWeapon.cpp"
    "src/weapon/BowWeapon.cpp"
    "src/weapon/ShieldWeapon.cpp"
    "src/weapon/TNTWeapon.cpp"
    "src/weapon/TridentWeapon.cpp"
    "src/weapon/ClockWeapon.cpp"
    "src/weapon/AxeWeapon.cpp"
    "src/weapon/DragonBreathWeapon.cpp"
    "src/Weapon/SeaHeartWeapon.cpp"
    "src/Weapon/LavaWeapon.cpp"
    "src/Weapon/MaceWeapon.cpp"
    "src/Weapon/LightningRodWeapon.cpp"
    "src/Projectile/Arrow.cpp"
    "src/Projectile/Projectile.cpp"
    "src/Projectile/MonsterProjectile.cpp"
    "src/Projectile/ProjectileManager.cpp"
    "src/Projectile/TNTProjectile.cpp"
    "src/Projectile/TridentProjectile.cpp"
    "src/Projectile/AxeProjectile.cpp"
    "src/Projectile/DragonBreathArea.cpp"
    "src/Projectile/BubbleProjectile.cpp"
    "src/Projectile/LavaProjectile.cpp"
    "src/Item/DropItem.cpp"
    "src/Item/XpOrb.cpp"
    "src/Item/DropItemManager.cpp"
    "src/Item/PotionItem.cpp"
    "src/Net/MojangAPI.cpp"
)

# 编译（只生成 .o）用的参数
$CompileFlags = @(
    "-c"
    "-std=c++20"
    "-I", "src"
    "-I", $Include
    "-Os"
)

# 链接用的参数（保持原来的一套库）
$LinkFlags = @(
    "-o", $Out
    "-s"
    "-L", $Lib
    "-lmingw32"
    "-lsfml-graphics"
    "-lsfml-window"
    "-lsfml-system"
    "-lsfml-main"
    "-lsfml-audio"
    "-lsfml-network"
    "-lfreetype"
    "-lwininet"
    "-lgdi32"
    "-lopengl32"
    "-lwinmm"
    "-luser32"
    "-lws2_32"
    "-mwindows"
)

# 编译中间文件（.s 等）默认落在系统 TEMP，C 盘快满时会报 "No space left on device"。
# 把子进程的 TEMP/TMP 指到工程目录下的 .tmp（E 盘），构建就不受系统盘剩余空间影响。
$BuildTemp = Join-Path $PSScriptRoot ".tmp"
New-Item -ItemType Directory -Force -Path $BuildTemp | Out-Null
$env:TEMP = $BuildTemp
$env:TMP = $BuildTemp

New-Item -ItemType Directory -Force -Path $ObjDir | Out-Null
New-Item -ItemType Directory -Force -Path (Split-Path $Out) | Out-Null

Write-Host "=== Building $Out (incremental) ==="
Write-Host "  Sources: $($Src.Count) files"
Write-Host "  Objects: $ObjDir\"
Write-Host ""

# 任何头文件都比 .o 新，就重编该 .o（头文件改动没有依赖表，用"全工程最新 .h"做保守判断）
$NewestHeader = Get-ChildItem -Path "src" -Recurse -File -Include *.h, *.hpp -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTimeUtc -Descending |
    Select-Object -First 1
$HeaderTime = if ($NewestHeader) { $NewestHeader.LastWriteTimeUtc } else { [datetime]::MinValue }
if ($NewestHeader) { Write-Host "  Newest header: $($NewestHeader.FullName) ($($HeaderTime.ToLocalTime()))" }

# 用 Start-Process（继承 stdout）而不是 `& $Gpp @args 2>&1 | ...`：
# 受限沙箱/无管道环境下 `&` 的重定向会失败（拒绝访问 / StandardOutputEncoding），
# Start-Process -NoNewWindow 让子进程直接写当前控制台，任何环境都能跑。
# 某些宿主拿不到 ExitCode：退化为"产物是否被本次重写"。
function Invoke-Gpp([string[]] $Arguments, [string] $Output) {
    $Before = if (Test-Path $Output) { (Get-Item $Output).LastWriteTimeUtc } else { [datetime]::MinValue }
    $Proc = Start-Process -FilePath $Gpp -ArgumentList $Arguments -NoNewWindow -Wait -PassThru
    $Proc.WaitForExit()
    $Code = $Proc.ExitCode
    if ($null -eq $Code) {
        $Written = (Test-Path $Output) -and ((Get-Item $Output).LastWriteTimeUtc -gt $Before)
        $Code = if ($Written) { 0 } else { 1 }
    }
    return $Code
}

$Objects = New-Object System.Collections.Generic.List[string]
$Compiled = 0
$Skipped = 0
$Failed = 0

foreach ($S in $Src) {
    $Rel = $S.Substring(4)                       # 去掉 "src/"
    $Obj = Join-Path $ObjDir ($Rel -replace '\.cpp$', '.o')
    New-Item -ItemType Directory -Force -Path (Split-Path $Obj) | Out-Null
    $Objects.Add($Obj)

    $NeedBuild = $true
    if (Test-Path $Obj) {
        $ObjTime = (Get-Item $Obj).LastWriteTimeUtc
        $SrcTime = (Get-Item $S).LastWriteTimeUtc
        if ($ObjTime -ge $SrcTime -and $ObjTime -ge $HeaderTime) { $NeedBuild = $false }
    }

    if (-not $NeedBuild) { $Skipped++; continue }

    Write-Host "  CC  $S"
    $Code = Invoke-Gpp ($CompileFlags + @($S) + @("-o", $Obj)) $Obj
    if ($Code -ne 0) {
        Write-Host ""
        Write-Host "=== Build FAILED at $S (exit $Code) ==="
        exit $Code
    }
    $Compiled++
}

Write-Host ""
if ($Compiled -eq 0) { Write-Host "  No source changed ($Skipped objects up to date)" }
else { Write-Host "  Compiled $Compiled, up to date $Skipped" }
Write-Host "  LD  $Out"

$Before = if (Test-Path $Out) { (Get-Item $Out).LastWriteTimeUtc } else { [datetime]::MinValue }
$Code = Invoke-Gpp ($Objects + $LinkFlags) $Out
if ($Code -ne 0) {
    Write-Host ""
    Write-Host "=== Build FAILED at link (exit $Code) ==="
    exit $Code
}

$Size = [math]::Round((Get-Item $Out).Length / 1KB, 1)
Write-Host ""
Write-Host "=== Build OK ($Size KB) ==="
