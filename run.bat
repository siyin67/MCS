@echo off
rem 双击本文件即可启动游戏。
rem game.exe 用相对路径读取 assets\resources.json，工作目录必须是项目根目录，
rem 所以不能直接双击 bin\game.exe（那样工作目录会变成 bin\，找不到资源）。
cd /d "%~dp0"
start "" "%~dp0bin\game.exe"
