//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_PLAYER_H
#define TERMPOKER_PLAYER_H
#include <vector>

#include "PokerCard.h"

class Player {
    std::vector<PokerCard*> cards = {};
public:
    void getCard(PokerCard* card) {
        cards.push_back(card);
    };

    PokerCard* playCard() const {
        return cards.at(0);
    };

    std::vector<PokerCard*> getCards() const {
        return cards;
    };

    void printCards() const;
};

#endif //TERMPOKER_PLAYER_H
