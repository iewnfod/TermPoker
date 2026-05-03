//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Game.h"
#include "../include/Utils.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

void Game::printMenu() const {
    const auto allMenuMap = getAllMenu();
    const auto subMenuRelations = getSubMenuRelations();
    for (auto it = allMenuMap.begin(); it != allMenuMap.end(); ++it) {
        if (this->menu == it->first) {
            Utils::setFgColor(TerminalColor::Yellow);
        }
        std::cout << "> " << it->second << std::endl;
        Utils::resetColor();

        auto subMenu = subMenuRelations.find(it->first);
        if (subMenu == subMenuRelations.end()) {
            for (auto r = subMenuRelations.begin(); r != subMenuRelations.end(); ++r) {
                for (auto & rr : r->second) {
                    if (rr == this->menu && it->first == r->first) {
                        subMenu = r;
                        break;
                    }
                }
            }
        }

        if (subMenu != subMenuRelations.end()) {
            for (auto & m : subMenu->second) {
                if (this->menu == m) {
                    Utils::setFgColor(TerminalColor::Yellow);
                }
                std::cout << "  > " << getMenuString(m) << std::endl;
                Utils::resetColor();
                ++it;
            }
        }
    }
}

void Game::clearMenu() {
    const auto allMenuMap = getAllMenu();
    for (int i = 0; i < allMenuMap.size(); i++) {
        Utils::cursorMoveAndClearLastLine();
    }
}

std::string Game::cardsToString(const std::vector<PokerCard*>& cards) {
    std::string result;
    for (auto* c : cards) {
        if (!result.empty()) result += " ";
        result += c->toString();
    }
    return result;
}

void Game::save() {
    if (currentRecord.timestamp.empty()) {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        currentRecord.timestamp = oss.str();
    }

    if (currentRecord.difficulty.empty()) currentRecord.difficulty = "Unknown";

    std::ofstream file("history.txt", std::ios::app);
    if (!file.is_open()) return;

    file << "===== BEGIN RECORD =====\n";
    file << "Timestamp: " << currentRecord.timestamp << "\n";
    file << "Difficulty: " << currentRecord.difficulty << "\n";
    file << "InitialHand: " << currentRecord.playerInitialHand << "\n";
    file << "PlaysCount: " << currentRecord.plays.size() << "\n";
    for (const auto& play : currentRecord.plays)
        file << "  " << play << "\n";
    file << "Result: " << currentRecord.result << "\n";
    file << "===== END RECORD =====\n\n";
    file.close();

    currentRecord = GameRecord{};
}

void Game::load() {
    std::ifstream file("history.txt");
    if (!file.is_open()) {
        std::cout << "No history file found.\n";
        return;
    }

    allRecords.clear();
    GameRecord rec;
    bool inside = false;
    std::string line;

    while (std::getline(file, line)) {
        if (line == "===== BEGIN RECORD =====") {
            inside = true;
            rec = GameRecord{};
            continue;
        }
        if (line == "===== END RECORD =====") {
            if (inside) allRecords.push_back(rec);
            inside = false;
            continue;
        }
        if (!inside) continue;

        if (line.find("Timestamp: ") == 0) rec.timestamp = line.substr(11);
        else if (line.find("Difficulty: ") == 0) rec.difficulty = line.substr(12);
        else if (line.find("InitialHand: ") == 0) rec.playerInitialHand = line.substr(13);
        else if (line.find("Result: ") == 0) rec.result = line.substr(8);
        else if (line.find("  ") == 0) rec.plays.push_back(line.substr(2));
    }
    file.close();

    if (allRecords.empty()) {
        std::cout << "No records.\n";
        return;
    }

    std::cout << "\n===== History (" << allRecords.size() << " games) =====\n";
    for (size_t i = 0; i < allRecords.size(); ++i) {
        const auto& r = allRecords[i];
        std::cout << "Game " << i+1 << "\n";
        std::cout << "  Time:       " << r.timestamp << "\n";
        std::cout << "  Difficulty: " << r.difficulty << "\n";
        std::cout << "  Result:     " << r.result << "\n";
        std::cout << "  Initial:    " << r.playerInitialHand << "\n";
        std::cout << "  Plays:      " << r.plays.size() << "\n";
        for (size_t j = 0; j < r.plays.size(); ++j)
            std::cout << "    " << j+1 << ": " << r.plays[j] << "\n";
        std::cout << "\n";
    }
}

void Game::welcome() {
    this->quitFlag = false;

    std::cout << std::endl;
    std::cout << "Welcome to TermPoker!" << std::endl;
    std::cout << "Use <up⬆️> or <down⬇️> to select and <enter↩️> to confirm." << std::endl;

    const auto column = Utils::getTermColumn();
    const auto term = Utils::getTerminalType();
    Utils::setFgColor(TerminalColor::Light);
    std::cout << "Terminal Type: " << term << "; ";
    std::cout << "Column: " << column << "; " << std::endl;
    Utils::resetColor();
    if ((term == "xterm-256color" && column < 190) || column < 80) {
        Utils::setFgColor(TerminalColor::Red);
        std::cout << "Your terminal column is less than expected. There might be some display error." << std::endl;
        Utils::resetColor();
    }

    int ch = 0;
    while (true) {
        printMenu();

        ch = Utils::getch();
        if (ch == 0x1B) {
            if (Utils::getch() == 0x5B) {  // [
                switch (Utils::getch()) {
                    case 0x41:  // 'A' Up
                        this->menuMoveUp();
                        break;
                    case 0x42:  // 'B' Down
                        this->menuMoveDown();
                        break;
                    default: break;
                }
            }
        }
        #ifdef _WIN32
        else if (ch == 0xE0 || ch == 0x00) {  // Windows Extend
            switch (Utils::getch()) {
                case 0x48:  // Up
                    this->menuMoveUp();
                    break;
                case 0x50:  // Down
                    this->menuMoveDown();
                    break;
                // case 0x4B: std::cout << "Left" << std::endl; break; // Left
                // case 0x4D: std::cout << "Right" << std::endl; break; // Right
                default: break;
            }
        }
        #endif
        else if (ch == '\n' || ch == '\r') {
            if (this->menu == GameMenu::PlayNow) {
                this->menu = GameMenu::SubDifficultyEasy;
            } else {
                break;
            }
        }
        else if (ch == 'q') {
            this->menu = GameMenu::Quit;
        }
        else if (ch == 'p') {
            this->menu = GameMenu::PlayNow;
        }
        else if (ch == 'c') {
            this->menu = GameMenu::CheckHistory;
        }
        else if (ch == 'a') {
            this->menu = GameMenu::About;
        }

        clearMenu();
    }

    if (this->menu == GameMenu::Quit) {
        this->quit();
    } else {
        std::cout << std::endl;
        this->mainloop();
    }
}

void Game::mainloop() {
    GameMenu action = this->menu;
    auto difficulty = GameDifficulty::Easy;
    if (this->menu == GameMenu::SubDifficultyEasy) {
        difficulty = GameDifficulty::Easy;
        action = GameMenu::PlayNow;
    }
    if (this->menu == GameMenu::SubDifficultyMedium) {
        difficulty = GameDifficulty::Medium;
        action = GameMenu::PlayNow;
    }
    if (this->menu == GameMenu::SubDifficultyHard) {
        difficulty = GameDifficulty::Hard;
        action = GameMenu::PlayNow;
    }
    if (action == GameMenu::Quit || this->quitFlag) {
        return;
    } else if (action == GameMenu::CheckHistory) {
        std::cout << "Coming soon..." << std::endl;
    } else if (action == GameMenu::PlayNow) {
        this->deck = new Deck(2);
        deck->onQuit([this]() {
            this->quit();
        });
        this->player = deck->autoGeneratePlayers();
        deck->setDifficulty(difficulty);
        deck->givePlayerCards();
        while (deck->getWinner().empty()) {
            player->waitForUserInput();
            if (this->quitFlag) {
                break;
            }
            std::cout << std::endl << "You played: " << deck->getLastPlayedCardsString() << std::endl;
            if (deck->checkWin()) {
                break;
            }
            deck->tryNewRound();
            deck->robotPlayCards();
        }
        if (!deck->getWinner().empty()) {
            deck->congratulateWin();
        }
    } else if (action == GameMenu::About) {
        std::cout << "About TermPoker" << std::endl;
        std::cout << "Repository: " << Utils::getClickableLink("https://github.com/iewnfod/TermPoker") << std::endl;
        std::cout << "Author: " << Utils::getClickableLink("Iewnfod", "https://github.com/iewnfod") << ", " << Utils::getClickableLink("Andy", "https://github.com/Andymaster007") << std::endl;
        std::cout << "Open Source License: " << Utils::getClickableLink("MPL-2.0", "https://github.com/iewnfod/TermPoker/blob/master/LICENSE") << std::endl;
        std::cout << "Local Data Storage Path: " << this->store.getBasePath() << std::endl;
        std::cout << "────────────────────────────────────────────" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        Utils::getch();
    }

    this->welcome();
}
