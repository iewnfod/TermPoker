//
// Created by SYSTEM on 2026/4/4.
//

#include <cmath>
#include <iostream>

#include "../include/Player.h"
#include "../include/Utils.h"

void Player::printCards() const {
    std::string head = "┌";
    std::string body1, body2, body3;
    std::string tail = "└";
    auto term = Utils::getTerminalType();
    for (const auto& card : cards) {
        auto t = card->getTypeString();
        auto v = card->getValueString();
        const int vWidth = static_cast<int>(v.length());
        const int tsWidth = static_cast<int>(Utils::to_wstring(t).length());
        int widthOffset = 0;
        if (term == "xterm-256color") {
            widthOffset += 1;
            if (card->getType() == POKER_CARD_TYPE::Joker) {
                widthOffset -= 2;
            }
        }
        const int width = vWidth + 4 + widthOffset;
        for (int i = 0; i < width; i++) {
            head += "─";
            tail += "─";
        }
        head += "┐";
        tail += "┘";
        body1 += "│ " + t;
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body1 += " ";
        }
        const int body2Padding = std::ceil((width-vWidth-1)/2.0);
        body2 += "|";
        for (int i = 0; i < body2Padding; i++) {
            body2 += " ";
        }
        body2 += v;
        for (int i = 0; i < width-vWidth-body2Padding; i++) {
            body2 += " ";
        }
        body3 += "│";
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body3 += " ";
        }
        body3 += t + " ";
    }
    body1 += "│";
    body2 += "│";
    body3 += "│";
    std::cout << head << std::endl << body1 << std::endl << body2 << std::endl << body3 << std::endl << tail << std::endl;
}
