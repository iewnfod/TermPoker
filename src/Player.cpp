//
// Created by SYSTEM on 2026/4/14.
//

#include <cmath>
#include <iostream>

#include "../include/Player.h"
#include "../include/Utils.h"

void Player::printCards() const {
    const auto resetColor = Utils::getResetColor();
    const auto selectColor = Utils::getFgColor(TerminalColor::Yellow);

    std::string uh;  // upper head to display selected cards
    std::string head, tail;
    std::string body1, body2, body3;
    auto term = Utils::getTerminalType();
    bool isUpArr[this->cards.size()];
    int selectedIndex = -1;
    for (int i = 0; i < this->cards.size(); i++) {
        isUpArr[i] = std::find(this->selectedCards.begin(), this->selectedCards.end(), this->cards[i])
            != this->selectedCards.end();
        if (this->cards[i] == this->selectedCard) {
            selectedIndex = i;
        }
    }
    for (int ci = 0; ci < this->cards.size(); ci++) {
        auto card = this->cards[ci];

        auto isUp = isUpArr[ci];
        bool isSelected = ci == selectedIndex;
        bool isLastUp = false;
        bool isLastSelected = false;
        if (ci != 0) {
            isLastUp = isUpArr[ci-1];
            isLastSelected = ci-1 == selectedIndex;
        }
        bool isNextUp = false;
        bool isNextSelected = false;
        if (ci != this->cards.size()-1) {
            isNextUp = isUpArr[ci+1];
            isNextSelected = ci+1 == selectedIndex;
        }

        auto t = card->getTypeString();
        auto v = card->getValueString();
        const int vWidth = static_cast<int>(v.length());
        const int tsWidth = static_cast<int>(Utils::to_wstring(t).length());
        int widthOffset = 0;
        int numberOffset = 0;
        if (term == "xterm-256color") {
            widthOffset += 1;
            numberOffset += 1;
            if (card->getType() == POKER_CARD_TYPE::Joker) {
                widthOffset -= 2;
            }
        }

        const int width = vWidth + 4 + widthOffset;

        if (isSelected) {
            uh += selectColor; head += selectColor; body1 += selectColor; body2 += selectColor; body3 += selectColor; tail += selectColor;
        }

        if (ci == 0) {
            if (isUp) {
                uh += "┌";
                head += "│";
            } else {
                uh += " ";
                head += "┌";
            }
            tail += "└";
        }

        for (int i = 0; i < width; i++) {
            tail += "─";
            if (!isUp) {
                head += "─";
                uh += " ";
            } else {
                if (i == 0 && ci != 0 && !isLastUp) {
                    uh += "┌";
                } else {
                    uh += "─";
                }
                head += " ";
            }
        }
        if (isUp) {
            if (!isLastUp && ci != 0) {
                uh += "─";
            }
            if (isNextUp && isNextSelected) {
                uh += selectColor + "┐";
                uh += resetColor;
            } else {
                uh += "┐";
            }
            if (isNextSelected) {
                head += selectColor + "│";
                head += resetColor;
            } else {
                head += "│";
            }
        } else {
            if (isNextUp) {
                if (isNextSelected) {
                    head += selectColor + "│";
                    head += resetColor;
                } else {
                    head += "│";
                }
            } else {
                if (isNextSelected) {
                    head += selectColor + "┐";
                    head += resetColor;
                } else {
                    head += "┐";
                }
                uh += " ";
            }
        }
        if (isNextSelected) {
            tail += selectColor + "┘";
            tail += resetColor;
        } else {
            tail += "┘";
        }
        if (isLastSelected) {
            body1 += selectColor + "│ ";
            body1 += resetColor;
        } else {
            body1 += "│ ";
        }
        body1 += t;
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body1 += " ";
        }
        const int body2Padding = static_cast<int>(std::ceil((width-vWidth-1)/2.0)) + numberOffset;
        if (isLastSelected) {
            body2 += selectColor + "│";
            body2 += resetColor;
            body3 += selectColor + "│";
            body3 += resetColor;
        } else {
            body2 += "│";
            body3 += "│";
        }
        for (int i = 0; i < body2Padding; i++) {
            body2 += " ";
        }
        body2 += v;
        for (int i = 0; i < width-vWidth-body2Padding; i++) {
            body2 += " ";
        }
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body3 += " ";
        }
        body3 += t + " ";

        if (isSelected) {
            uh += resetColor; head += resetColor; body1 += resetColor; body2 += resetColor; body3 += resetColor; tail += resetColor;
        }
    }
    if (cards.size()-1 == selectedIndex) {
        body1 += selectColor; body2 += selectColor; body3 += selectColor;
    }
    body1 += "│"; body2 += "│"; body3 += "│";
    body1 += resetColor; body2 += resetColor; body3 += resetColor;
    std::cout
        << uh << std::endl
        << head << std::endl
        << body1 << std::endl
        << body2 << std::endl
        << body3 << std::endl
        << tail << std::endl;
}

std::vector<PokerCard*> Player::waitForUserInput() {
    std::cout << "Yellow means current card." << std::endl;
    std::cout << "Use <left⬅️> or <right➡️> to select, <space␣> to choose, and <enter↩️> to confirm." << std::endl;

    sortCards();

    int ch = 0;
    while (true) {
        printCards();

        ch = Utils::getch();
        if (ch == 0x1B) {
            if (Utils::getch() == 0x5B) {
                switch (Utils::getch()) {
                    case 0x44:  // Left
                        this->cardMoveLeft();
                        break;
                    case 0x43:  // Right
                        this->cardMoveRight();
                        break;
                    default: break;
                }
            }
        }
        #ifdef _WIN32
        else if (ch == 0xE0 || ch == 0x00) {  // Windows Extend
            switch (Utils::getch()) {
                case 0x4B:  // Left
                    this->cardMoveLeft();
                    break;
                case 0x4D:  // Right
                    this->cardMoveRight();
                    break;
                default: break;
            }
        }
        #endif
        else if (ch == ' ') {
            this->selectCard();
        }
        else if (ch == '\n' || ch == '\r') {
            break;
        }

        clearCards();
    }

    return this->selectedCards;
}
