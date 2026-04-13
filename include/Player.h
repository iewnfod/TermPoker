//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_PLAYER_H
#define TERMPOKER_PLAYER_H
#include <algorithm>
#include <vector>

#include "PokerCard.h"
#include "Utils.h"

class Player {
    std::vector<PokerCard*> cards = {};
    bool isRobot = true;
    std::vector<PokerCard*> selectedCards = {};
    PokerCard* selectedCard = nullptr;

    void selectCard() {
        if (selectedCard == nullptr) {
            return;
        }
        const auto it = std::find(selectedCards.begin(), selectedCards.end(), selectedCard);
        if (it == selectedCards.end()) {
            selectedCards.push_back(selectedCard);
            this->cardMoveRight();
        } else {
            selectedCards.erase(it);
        }
    }

    void cardMoveLeft() {
        if (this->selectedCard == nullptr) {
            this->selectedCard = cards.back();
        } else {
            auto it = std::find(cards.begin(), cards.end(), selectedCard);
            if (it != cards.begin()) {
                --it;
                selectedCard = *it;
            }
        }
    }

    void cardMoveRight() {
        if (this->selectedCard == nullptr) {
            this->selectedCard = cards.front();
        } else {
            auto it = std::find(cards.begin(), cards.end(), selectedCard);
            if (it != --cards.end()) {
                ++it;
                selectedCard = *it;
            }
        }
    }

    static void clearCards() {
        for (int i = 0; i < 6; i ++) {
            Utils::cursorMoveAndClearLastLine();
        }
    }
public:
    Player() = default;

    explicit Player(const bool isRobot) {
        this->isRobot = isRobot;
    }

    void getCard(PokerCard* card) {
        cards.push_back(card);
    }

    PokerCard* playCard() const {
        return cards.at(0);
    }

    std::vector<PokerCard*> getCards() const {
        return cards;
    }

    void sortCards() {
        std::sort(cards.begin(), cards.end(), [](const PokerCard* c1, const PokerCard* c2) {
            if (c1->getValueIndex() < c2->getValueIndex()) {
                return true;
            }
            return false;
        });
    }

    void printCards() const;

    void setIsRobot(bool b) {
        isRobot = b;
    }

    std::vector<PokerCard*> waitForUserInput();
};

#endif //TERMPOKER_PLAYER_H
