// ============================================================
// 问道 MUD - 游戏主入口
// 编译：g++ -std=c++17 -I include src/*.cpp -o game.exe
// 运行：game.exe
// ============================================================

#include <iostream>
#include <exception>
#include "game.h"

int main() {
    try {
        Game game;
        game.start();
    } catch (const std::exception& e) {
        std::cerr << "游戏发生错误：" << e.what() << std::endl;
        std::cerr << "按回车退出...";
        std::cin.get();
        return 1;
    }
    return 0;
}
