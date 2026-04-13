//
// Created by muyunxi on 2026/4/13.
//


#include "../include/Game.h"
#include "../include/Utils.h"

#include <iostream>

void Game::printMenu() const {
    const auto allMenuMap = getAllMenu();
    for (auto & it : allMenuMap) {
        if (this->menu == it.first) {
            Utils::setFgColor(TerminalColor::Yellow);
        }
        std::cout << "> " << it.second << std::endl;
        Utils::resetColor();
    }
}

void Game::clearMenu() {
    const auto allMenuMap = getAllMenu();
    for (int i = 0; i < allMenuMap.size(); i++) {
        Utils::cursorMoveAndClearLastLine();
    }
}

void Game::welcome() {
    std::cout << "Welcome to TermPoker!" << std::endl;
    std::cout << "Use <up⬆️> or <down⬇️> to select and <enter↩️> to confirm." << std::endl;

    int ch = 0;
    while (true) {
        printMenu();
        ch = Utils::getch();
        if (ch == 0x1B) {
            if (Utils::getch() == 0x5B) {
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
        else if (ch == 0xE0 || ch == 0x00) {
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
            break;
        }
        clearMenu();
    }

    std::cout << getMenuString(this->menu) << std::endl;
}
