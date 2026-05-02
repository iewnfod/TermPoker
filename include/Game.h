//
// Created by muyunxi on 2026/4/13.
//

#ifndef TERMPOKER_GAME_H
#define TERMPOKER_GAME_H
#include <map>
#include <string>
#include <vector>

#include "Deck.h"
#include "Store.h"

enum class GameMenu {
    PlayNow,
    SubDifficultyEasy,
    SubDifficultyMedium,
    SubDifficultyHard,
    CheckHistory,
    About,
    Quit,
};

class Game {
    GameMenu menu = GameMenu::PlayNow;
    bool quitFlag = false;
    Deck *deck;
    Player *player{};
    Store store = Store();

    static std::map<GameMenu, std::string> getAllMenu() {
        std::map<GameMenu, std::string> menuMap = {
            {GameMenu::PlayNow, "Play Now"},
            {GameMenu::SubDifficultyEasy, "Easy Mode"},
            {GameMenu::SubDifficultyMedium, "Medium Mode"},
            {GameMenu::SubDifficultyHard, "Hard Mode"},
            {GameMenu::CheckHistory, "Check History"},
            {GameMenu::About, "About"},
            {GameMenu::Quit, "Quit"},
        };
        return menuMap;
    }

    static std::map<GameMenu, std::vector<GameMenu>> getSubMenuRelations() {
        std::map<GameMenu, std::vector<GameMenu>> subMenuMap = {
            {GameMenu::PlayNow, std::vector<GameMenu>{
                GameMenu::SubDifficultyEasy, GameMenu::SubDifficultyMedium,
                GameMenu::SubDifficultyHard
            }},
        };
        return subMenuMap;
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
    static void clearMenu();

public:
    Game() {
        this->deck = new Deck(2);
        this->deck->onQuit([this]() {
            this->quit();
        });
    };

    void save();
    void load();
    void quit() {
        std::cout << "Quit!" << std::endl;
        this->quitFlag = true;
    }
    void mainloop();
    void welcome();
};

#endif //TERMPOKER_GAME_H
