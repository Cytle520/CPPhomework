@echo off
chcp 65001 >nul
echo ============================================
echo   问道 MUD - 编译脚本
echo ============================================
echo.

where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 未找到 g++ 编译器！
    echo 请先安装 MinGW 或 TDM-GCC，并添加到系统 PATH。
    echo 下载地址：https://jmeubank.github.io/tdm-gcc/
    pause
    exit /b 1
)

echo 正在编译...
g++ -std=c++17 -I include -O2 src\game_data.cpp src\player.cpp src\npc.cpp src\room.cpp src\ui.cpp src\combat.cpp src\story.cpp src\game.cpp main.cpp -o game.exe

if %errorlevel% equ 0 (
    echo.
    echo [成功] 编译完成！生成 game.exe
    echo.
    echo 运行游戏请输入：game.exe
    echo.
) else (
    echo.
    echo [失败] 编译出错，请查看上方错误信息。
    echo.
)

pause
