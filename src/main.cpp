#include "../include/Deck.h"
#include "../include/Game.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    std::locale::global(std::locale(""));
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    auto game = new Game();
    game->welcome();
    game->mainloop();

    return 0;
}
