//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Game.h"
#include "../include/Utils.h"

#include <iostream>

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
    if (this->menu == GameMenu::SubDifficultyEasy) {
        deck->setDifficulty(GameDifficulty::Easy);
        action = GameMenu::PlayNow;
    }
    if (this->menu == GameMenu::SubDifficultyMedium) {
        deck->setDifficulty(GameDifficulty::Medium);
        action = GameMenu::PlayNow;
    }
    if (this->menu == GameMenu::SubDifficultyHard) {
        deck->setDifficulty(GameDifficulty::Hard);
        action = GameMenu::PlayNow;
    }
    if (action == GameMenu::Quit || this->quitFlag) {
        return;
    } else if (action == GameMenu::CheckHistory) {
        std::cout << "Coming soon..." << std::endl;
    } else if (action == GameMenu::PlayNow) {
        this->player = this->deck->autoGeneratePlayers();
        this->deck->givePlayerCards();
        while (true) {
            this->player->waitForUserInput();
            if (this->quitFlag) {
                break;
            }
            std::cout << "You played: " << this->player->getLastPlayedCardsString() << std::endl;
            this->deck->robotPlayCards();
        }
    } else if (action == GameMenu::About) {
        std::cout << "About TermPoker" << std::endl;
        std::cout << "Repository: " << Utils::getClickableLink("https://github.com/iewnfod/TermPoker") << std::endl;
        std::cout << "Author: " << Utils::getClickableLink("Iewnfod", "https://github.com/iewnfod") << ", " << Utils::getClickableLink("Andy", "https://github.com/Andymaster007") << std::endl;
        std::cout << "Data Storage Path: " << this->store.getBasePath() << std::endl;
        std::cout << "────────────────────────────────────────────" << std::endl;
        std::cout << "Press any key to continue..." << std::endl;
        Utils::getch();
    }

    this->welcome();
}
