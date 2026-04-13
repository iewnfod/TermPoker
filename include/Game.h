//
// Created by muyunxi on 2026/4/13.
//

#ifndef TERMPOKER_GAME_H
#define TERMPOKER_GAME_H
#include <map>
#include <string>

enum class GameMenu {
    SelectDifficulty,
    CheckHistory,
    Quit,
};

class Game {
    GameMenu menu = GameMenu::SelectDifficulty;

    static std::map<GameMenu, std::string> getAllMenu() {
        std::map<GameMenu, std::string> menuMap = {
            {GameMenu::SelectDifficulty, "Select Difficulty"},
            {GameMenu::CheckHistory, "Check History"},
            {GameMenu::Quit, "Quit"},
        };
        return menuMap;
    }

    static std::string getMenuString(GameMenu m) {
        const auto allMenu = getAllMenu();
        return allMenu.at(m);
    }

    void menuMoveUp() {
        const auto allMenu = getAllMenu();
        auto crt = allMenu.find(this->menu);
        if (crt != allMenu.begin()) {
            --crt;
            this->menu = crt->first;
        }
    }

    void menuMoveDown() {
        const auto allMenu = getAllMenu();
        auto crt = allMenu.find(this->menu);
        if (crt != --allMenu.end()) {
            ++crt;
            this->menu = crt->first;
        }
    }

    void printMenu() const;
    static void clearMenu() ;

public:
    Game() = default;

    void save();
    void load();
    void welcome();
};

#endif //TERMPOKER_GAME_H
