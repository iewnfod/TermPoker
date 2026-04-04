//
// Created by SYSTEM on 2026/4/4.
//

#include <iostream>
#include <cwchar>

#include "../include/Player.h"
#include "../include/Utils.h"

void Player::printCards() const {
    std::string head = "┌";
    std::string body1, body2, body3;
    std::string tail = "└";
    for (const auto& card : cards) {
        auto t = card->getTypeString();
        auto v = card->getValueString();
        const int tsWidth = Utils::to_wstring(t).length();
        const int width = v.length() + 4;
        for (int i = 0; i < width; i++) {
            head += "─";
            tail += "─";
        }
        head += "┐";
        tail += "┘";
        body1 += "│ " + t;
        for (int i = 0; i < width-tsWidth-1; i++) {
            body1 += " ";
        }
        body2 += "│  " + v + "  ";
        body3 += "│";
        for (int i = 0; i < width-tsWidth-1; i++) {
            body3 += " ";
        }
        body3 += t + " ";
    }
    body1 += "│";
    body2 += "│";
    body3 += "│";
    std::cout << head << std::endl << body1 << std::endl << body2 << std::endl << body3 << std::endl << tail << std::endl;
}
