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

    auto deck = new Deck(2);

    for (int i = 0; i < 4; i ++) {
        Player player;
        deck->addPlayer(player);
    }
    deck->givePlayerCards();
    deck->printCardsByPlayer();

    return 0;
}
