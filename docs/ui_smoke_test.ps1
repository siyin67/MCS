# 设置界面全量冒烟测试：一次构建、一次运行，把能点的路径都点一遍
#   菜单 -> 选项 -> 音乐和声音（点/拖滑条）-> 选项 -> 语言（en/zh 来回切）
#   -> 保存/读回 -> 开始游戏 -> ESC 暂停 -> 设置 -> 声音/语言 -> 返回 -> 继续 -> 退出到菜单
# 用法：先 .\build.ps1，再 & .\docs\ui_smoke_test.ps1
# 截图落在 docs\screenshots（工程目录内，跑完可以直接当交付物看）
$ErrorActionPreference = 'Stop'
$proj = 'E:\cppwork\sfmlgame'
$shotDir = Join-Path $proj 'docs\screenshots'
New-Item -ItemType Directory -Force -Path $shotDir | Out-Null
$log = Join-Path $env:TEMP 'sflm_ui.txt'
Remove-Item $log -ErrorAction SilentlyContinue

Add-Type -AssemblyName System.Drawing
Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W32D {
  [DllImport("user32.dll")] public static extern bool SetProcessDpiAwarenessContext(IntPtr ctx);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT r);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT r);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT p);
  [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hWnd, IntPtr hdc, uint flags);
  [DllImport("user32.dll")] public static extern void SetCursorPos(int x, int y);
  [DllImport("user32.dll")] public static extern void mouse_event(uint f, uint dx, uint dy, uint d, IntPtr e);
  [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint f, IntPtr extra);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
  [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
}
"@
[W32D]::SetProcessDpiAwarenessContext([IntPtr](-4)) | Out-Null

$p = Start-Process -FilePath (Join-Path $proj 'bin\game.exe') -WorkingDirectory $proj -RedirectStandardOutput $log -PassThru
$h = [IntPtr]::Zero
for ($i = 0; $i -lt 80; $i++) { Start-Sleep -Milliseconds 200; $p.Refresh(); if ($p.HasExited) { break }; $h = $p.MainWindowHandle; if ($h -ne [IntPtr]::Zero) { break } }
if ($h -eq [IntPtr]::Zero) { Write-Host "NO WINDOW"; Get-Content $log | Select-Object -Last 20; exit 1 }
Start-Sleep -Milliseconds 2200

$wr = New-Object 'W32D+RECT'; [W32D]::GetWindowRect($h, [ref]$wr) | Out-Null
$cr = New-Object 'W32D+RECT'; [W32D]::GetClientRect($h, [ref]$cr) | Out-Null
$org = New-Object 'W32D+POINT'; $org.X = 0; $org.Y = 0
[W32D]::ClientToScreen($h, [ref]$org) | Out-Null
$scale = ($cr.Right - $cr.Left) / 1280.0

function Shot($name) {
    $w = $wr.Right - $wr.Left; $ht = $wr.Bottom - $wr.Top
    $bmp = New-Object System.Drawing.Bitmap $w, $ht
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    [W32D]::PrintWindow($h, $hdc, 2) | Out-Null
    $g.ReleaseHdc($hdc); $g.Dispose()
    if (-not $name.EndsWith('.png')) { $name = $name + '.png' }
    $path = Join-Path $shotDir $name
    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png); $bmp.Dispose()
    Write-Host "shot -> $name"
}

function ClickClient($x, $y, $label) {
    $px = $org.X + [int]($x * $scale); $py = $org.Y + [int]($y * $scale)
    [W32D]::SetCursorPos($px, $py) | Out-Null
    Start-Sleep -Milliseconds 240
    [W32D]::mouse_event(0x0002, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 130
    [W32D]::mouse_event(0x0004, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 650
    Write-Host "click $label ($x,$y)"
}

# 按住不放、移动、松开 —— 验证"直接在滑条上拖动"
function DragClient($x1, $y1, $x2, $y2, $label) {
    $px1 = $org.X + [int]($x1 * $scale); $py1 = $org.Y + [int]($y1 * $scale)
    $px2 = $org.X + [int]($x2 * $scale); $py2 = $org.Y + [int]($y2 * $scale)
    [W32D]::SetCursorPos($px1, $py1) | Out-Null
    Start-Sleep -Milliseconds 200
    [W32D]::mouse_event(0x0002, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 150
    [W32D]::SetCursorPos([int](($px1 + $px2) / 2), [int](($py1 + $py2) / 2)) | Out-Null
    Start-Sleep -Milliseconds 150
    [W32D]::SetCursorPos($px2, $py2) | Out-Null
    Start-Sleep -Milliseconds 200
    [W32D]::mouse_event(0x0004, 0, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 600
    Write-Host "drag $label ($x1,$y1) -> ($x2,$y2)"
}

function PressEsc() {
    [W32D]::keybd_event(0x1B, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 120
    [W32D]::keybd_event(0x1B, 0, 2, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 800
    Write-Host "pressed ESC"
}

# 按一下任意键（vk 用 16 进制字节，如 0x39 是数字键 9）
function PressKey([byte]$vk, [string]$label) {
    [W32D]::keybd_event($vk, 0, 0, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 150
    [W32D]::keybd_event($vk, 0, 2, [IntPtr]::Zero)
    Start-Sleep -Milliseconds 700
    Write-Host "pressed key $label"
}

# ---- 坐标（1280x720 客户区）----
# 菜单：start (640,425) / settings (640,490) / exit (640,555)
# 选项页：声音 (420,190) / 语言 (860,190) / 完成 (640,635)
# 声音页：主音量条 y=190 整行 220..1060；武器 y=262 左列 220..620；怪物右列 660..1060；音乐/音效 y=334
# 语言页：两行居中，en_us (640,330) / zh_cn (640,360)，完成 (640,635)
# 暂停页：继续 (640,425) / 设置 (640,495) / 主菜单 (640,565)

Shot 't01_menu_zh'
ClickClient 640 490 'menu-settings'
Shot 't02_options_zh'

# --- 音乐和声音：点一下 + 拖一次 + 主音量整行点一下 ---
ClickClient 420 190 'options-sound'
Shot 't03_sound_page'
ClickClient 420 262 'weapon-click-50'
DragClient 420 262 560 262 'weapon->85'
ClickClient 850 190 'master-75'
Shot 't04_sound_after'
ClickClient 640 635 'sound-done'
Shot 't05_options_back'

# --- 语言：切英文 -> 切回中文 ---
ClickClient 860 190 'options-language'
Shot 't06_language_zh'
ClickClient 640 330 'pick-en'
Shot 't07_language_en'
ClickClient 640 360 'pick-zh'
Shot 't08_language_zh_again'
ClickClient 640 635 'language-done'

# --- 换成英文看菜单是否跟着变，再换回中文 ---
ClickClient 860 190 'options-language2'
ClickClient 640 330 'pick-en2'
ClickClient 640 635 'language-done2'
ClickClient 640 635 'options-done'
Shot 't09_menu_en'
ClickClient 640 490 'menu-settings2'
ClickClient 860 190 'options-language3'
ClickClient 640 360 'pick-zh3'
ClickClient 640 635 'language-done3'
ClickClient 640 635 'options-done2'
Shot 't10_menu_zh_again'

# --- 进游戏，先看升级界面（游戏里按 9 会直接触发升级），再 ESC 暂停 ---
ClickClient 640 425 'start-game'
Start-Sleep -Milliseconds 2600
Shot 't11_game'
PressKey 0x39 '9-upgrade'
Shot 't11b_upgrade_zh'
ClickClient 640 390 'upgrade-pick'
Start-Sleep -Milliseconds 600
Shot 't11c_after_upgrade'
PressEsc
Shot 't12_pause_zh'
ClickClient 640 495 'pause-settings'
Shot 't13_options_from_pause'
ClickClient 420 190 'pause-sound'
Shot 't14_sound_from_pause'
ClickClient 640 635 'sound-done2'
ClickClient 640 635 'options-done3'
Shot 't15_pause_back'
ClickClient 640 425 'pause-resume'
Start-Sleep -Milliseconds 800
Shot 't16_game_resumed'
PressEsc
ClickClient 640 565 'pause-exit'
Shot 't17_menu_after_exit'

Start-Sleep -Milliseconds 300
Stop-Process -Id $p.Id -Force -ErrorAction SilentlyContinue

Write-Host "=== settings.json ==="
Get-Content (Join-Path $proj 'assets\config\settings.json')
Write-Host "=== log (settings/lang/errors) ==="
Get-Content $log | Select-String 'Language|Settings|Failed|Warning' | ForEach-Object { $_.Line }
