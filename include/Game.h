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

    /**
     * @return map of all menus, from enum item to string
     */
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

    /**
     * @return a map that defined the relation of submenu
     */
    static std::map<GameMenu, std::vector<GameMenu>> getSubMenuRelations() {
        std::map<GameMenu, std::vector<GameMenu>> subMenuMap = {
            {GameMenu::PlayNow, std::vector<GameMenu>{
                GameMenu::SubDifficultyEasy, GameMenu::SubDifficultyMedium,
                GameMenu::SubDifficultyHard
            }},
        };
        return subMenuMap;
    }

    /**
     * Get game menu item name in string.
     * @param m game menu item
     * @return string of that enum item
     */
    static std::string getMenuString(GameMenu m) {
        const auto allMenu = getAllMenu();
        return allMenu.at(m);
    }

    /**
     * Move cursor to last menu item.
     */
    void menuMoveUp() {
        const auto allMenu = getAllMenu();
        auto crt = allMenu.find(this->menu);
        if (crt != allMenu.begin()) {
            --crt;
            this->menu = crt->first;
        }
    }

    /**
     * Move cursor to next menu item.
     */
    void menuMoveDown() {
        const auto allMenu = getAllMenu();
        auto crt = allMenu.find(this->menu);
        if (crt != --allMenu.end()) {
            ++crt;
            this->menu = crt->first;
        }
    }

    /**
     * Print menu to terminal.
     */
    void printMenu() const;

    /**
     * Erase printed menu for next print.
     */
    static void clearMenu();

    struct GameRecord {
        std::string timestamp;
        std::string difficulty;
        std::string playerInitialHand;
        std::vector<std::string> plays;
        std::string result;
    };
    GameRecord currentRecord;

    /**
     * Convert cards to a single line of string to store
     * @param cards
     * @return e.g. "clubs|3 diamonds|K joker|LJ"
     */
    static std::string cardsToString(const std::vector<PokerCard*>& cards);
    std::vector<GameRecord> allRecords;

    /**
     * convert GameDifficulty to string to store
     * @param diff
     * @return string
     */
    void difficultyToString(const GameDifficulty diff) {
        switch (diff) {
            case GameDifficulty::Easy:   currentRecord.difficulty = "Easy"; break;
            case GameDifficulty::Medium: currentRecord.difficulty = "Medium"; break;
            case GameDifficulty::Hard:   currentRecord.difficulty = "Hard"; break;
        }
    }

public:
    Game() {
        this->deck = new Deck(2);
        this->deck->onQuit([this]() {
            this->quit();
        });
    };

    /**
     * Save data to store.
     */
    void save();

    /**
     * Load data from store.
     */
    void load();

    /**
     * Exit game.
     */
    void quit() {
        std::cout << "Quit!" << std::endl;
        this->quitFlag = true;
    }

    /**
     * Mainloop of the game.
     */
    void mainloop();

    /**
     * Print welcome menu of the game and automatically run mainloop.
     */
    void welcome();
};

#endif //TERMPOKER_GAME_H
