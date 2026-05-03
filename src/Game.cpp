//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Game.h"
#include "../include/Utils.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#ifdef _WIN32
#include <direct.h>
#endif

namespace {
    bool dirExists(const std::string& path) {
        struct stat st;
        return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
    }

    bool makeDir(const std::string& path) {
        #ifdef _WIN32
        return _mkdir(path.c_str()) == 0;
        #else
        return mkdir(path.c_str(), 0755) == 0;
        #endif
    }
}

std::string Game::getCurrentTimestamp() {
    auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Game::generateHistoryFileName() {
    auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str() + ".json";
}

void Game::ensureHistoryDir() {
    std::string historyDir = Utils::joinPath({Store().getBasePath(), "history"});
    if (!dirExists(historyDir)) makeDir(historyDir);
}

std::vector<std::string> cardsToStrings(const std::vector<PokerCard*>& cards) {
    std::vector<std::string> result;
    result.reserve(cards.size());
    for (const auto* c : cards) {
        result.push_back(c->toString());
    }
    return result;
}

void Game::collectHistoryFiles(std::vector<std::string>& files) {
    std::string historyDir = Utils::joinPath({Store().getBasePath(), "history"});
    DIR* dir = opendir(historyDir.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name.length() > 5 && name.substr(name.length() - 5) == ".json") {
                files.push_back(Utils::joinPath({historyDir, name}));
            }
        }
        closedir(dir);
    }
    std::sort(files.begin(), files.end());
}

void Game::setCurrentDifficulty(const GameDifficulty diff) {
    currentRecord.difficulty = diff;
}

void Game::recordInitialHand(const std::vector<PokerCard*>& hand) {
    currentRecord.playerInitialHand = cardsToStrings(hand);
}

void Game::recordResult(const std::string& result) {
    currentRecord.result = result;
}

void Game::saveCurrentGame() {
    if (currentRecord.timestamp.empty())
        currentRecord.timestamp = getCurrentTimestamp();

    ensureHistoryDir();
    std::string fullPath = Utils::joinPath({Store().getBasePath(), "history", generateHistoryFileName()});

    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Failed to save history: " << fullPath << std::endl;
        return;
    }
    json j = currentRecord;
    file << j.dump(4);
    file.close();

    currentRecord = GameRecord{};
}

void Game::showHistory() {
    std::vector<std::string> files;
    collectHistoryFiles(files);
    if (files.empty()) {
        std::cout << "No history found.\n";
        return;
    }

    allRecords.clear();
    for (const auto& path : files) {
        std::ifstream file(path);
        if (!file.is_open()) continue;
        json j;
        try {
            file >> j;
            allRecords.push_back(j.get<GameRecord>());
        } catch (const std::exception& e) {
            std::cerr << "Parse error in " << path << ": " << e.what() << std::endl;
        }
    }

    if (allRecords.empty()) {
        std::cout << "No valid records.\n";
        return;
    }

    std::cout << "\n===== Game History (" << allRecords.size() << " games) =====\n\n";
    for (size_t i = 0; i < allRecords.size(); ++i) {
        const auto& rec = allRecords[i];
        std::cout << "Game " << (i+1) << "\n";
        std::cout << "  Time:       " << rec.timestamp << "\n";
        std::cout << "  Difficulty: ";
        switch (rec.difficulty) {
            case GameDifficulty::Easy:   std::cout << "Easy"; break;
            case GameDifficulty::Medium: std::cout << "Medium"; break;
            case GameDifficulty::Hard:   std::cout << "Hard"; break;
            default:                     std::cout << "Unknown";
        }
        std::cout << "\n  Result:     " << rec.result << "\n";
        // std::cout << "  Initial Hand: ";
        // for (const auto& cardStr : rec.playerInitialHand) {
        //     // 可选：将字符串转回 PokerCard 再打印漂亮格式，或直接打印原始字符串
        //     PokerCard card(cardStr);
        //     std::cout << card.getTypeString() << card.getValueString() << " ";
        // }
        std::cout << "\n";
    }
}

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
        else if (ch == 'r') {
            this->menu = GameMenu::Rule;
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
        this->showHistory();
    } else if (action == GameMenu::PlayNow) {
        this->deck = new Deck(2);
        deck->onQuit([this]() {
            this->quit();

        });
        this->player = deck->autoGeneratePlayers();
        deck->setDifficulty(difficulty);
        this->setCurrentDifficulty(difficulty);
        deck->givePlayerCards();
        const std::vector<PokerCard*> playerHand = player->getCards();
        this->recordInitialHand(playerHand);
        while (deck->getWinner().empty()) {
            player->waitForUserInput();
            if (this->quitFlag) {
                this->recordResult("Quit");
                break;
            }
            std::cout << std::endl;
            const auto lastPlayed = deck->getLastPlayedCards();
            if (lastPlayed.empty()) {
                std::cout << "You played nothing" << std::endl;
            } else {
                std::cout << "You played: " << deck->getLastPlayedCardsString() << std::endl;
            }
            if (deck->checkWin()) {
                break;
            }
            deck->tryNewRound();
            deck->robotPlayCards();
        }
        if (!deck->getWinner().empty()) {
            deck->congratulateWin();
            this->recordResult(deck->getWinner() == player->getId() ? "Win" : "Lose");
        }
        this->saveCurrentGame();
    } else if (action == GameMenu::About) {
        std::cout << "About TermPoker" << std::endl;
        std::cout << "Repository: " << Utils::getClickableLink("https://github.com/iewnfod/TermPoker") << std::endl;
        std::cout << "Author: " << Utils::getClickableLink("Iewnfod", "https://github.com/iewnfod") << ", " << Utils::getClickableLink("Andy", "https://github.com/Andymaster007") << std::endl;
        std::cout << "Open Source License: " << Utils::getClickableLink("MPL-2.0", "https://github.com/iewnfod/TermPoker/blob/master/LICENSE") << std::endl;
        std::cout << "Local Data Storage Path: " << this->store.getBasePath() << std::endl;
        std::cout << "────────────────────────────────────────────" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        Utils::getch();
    } else if (action == GameMenu::Rule) {
        std::cout << "Rule of TermPoker" << std::endl;
        std::cout << "In TermPoker, we use the ruleset of GuanDan(掼蛋) with some modifications." << std::endl;
        std::cout << "We removed the team and upgrade concept and only use the basic rule." << std::endl;

        std::cout << std::endl << "Card values:" << std::endl;
        std::cout << "3 < 4 < 5 < 6 < 7 < 8 < 9 < 10 < J < Q < K < A < 2 < Small Joker (SJ) < Large Joker (LJ)" << std::endl;

        std::cout << std::endl << "Card colors:" << std::endl;
        std::cout << " • " << PokerCard(POKER_CARD_VALUE::N3, POKER_CARD_TYPE::Diamonds).getTypeString() << " Diamonds" << std::endl;
        std::cout << " • " << PokerCard(POKER_CARD_VALUE::N3, POKER_CARD_TYPE::Clubs).getTypeString() << " Clubs" << std::endl;
        std::cout << " • " << PokerCard(POKER_CARD_VALUE::N3, POKER_CARD_TYPE::Hearts).getTypeString() << " Hearts" << std::endl;
        std::cout << " • " << PokerCard(POKER_CARD_VALUE::N3, POKER_CARD_TYPE::Spades).getTypeString() << " Spades" << std::endl;
        std::cout << "All joker cards are not belonging to any color. They are marked as " << PokerCard(POKER_CARD_VALUE::N3, POKER_CARD_TYPE::Joker).getTypeString() << " " << std::endl;

        std::cout << std::endl << "Card types:" << std::endl;
        std::cout << " • Single \t\t Only one card" << std::endl;
        std::cout << " • Pair \t\t Two same cards" << std::endl;
        std::cout << " • Triple \t\t Three same cards" << std::endl;
        std::cout << " • Boom \t\t Four, five or six same cards" << std::endl;
        std::cout << " • Three Plus Two \t A triple with another pair" << std::endl;
        std::cout << " • Straight \t\t Five consecutive cards, not including 2" << std::endl;
        std::cout << " • Double Triple \t Two triple with consecutive values (6 cards in total)" << std::endl;
        std::cout << " • Triple Pair \t\t Three pair with consecutive values (6 cards in total)" << std::endl;
        std::cout << "Jokers can only be played as single or pair, and Joker boom is not allowed in this rule." << std::endl;

        std::cout << std::endl << "Compare card types:" << std::endl;
        std::cout << " • Single / Pair / Triple \t\t Directly compare the card value" << std::endl;
        std::cout << " • Boom \t\t\t\t First compare the boom size, then compare the card value" << std::endl;
        std::cout << " • Three Plus Two \t\t\t Compare the value of triple in it" << std::endl;
        std::cout << " • Straight \t\t\t\t Compare the largest value in this straight" << std::endl;
        std::cout << " • Triple Pair / Double Triple \t\t Compare the largest pair or triple" << std::endl;

        std::cout << std::endl << "When a game start, a new round will be created." << std::endl;
        std::cout << "For each new round, the first player can play any valid card type but not skip." << std::endl;
        std::cout << "Then others will play in sequence respectively. Player can do following options:" << std::endl;
        std::cout << " • Play the same card type (including boom) larger than the previous one." << std::endl;
        std::cout << " • Play a boom and take over the round when the previous one is not a boom." << std::endl;
        std::cout << " • Skip the turn." << std::endl;
        std::cout << "Until a player played something and all of others choose to skip, a new round will be created again, where the last played player would play first in the new round." << std::endl;
        std::cout << "Once a player played out all cards, the player is the winner." << std::endl;

        std::cout << "────────────────────────────────────────────" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        Utils::getch();
    }

    this->welcome();
}
