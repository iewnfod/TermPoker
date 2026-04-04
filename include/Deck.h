//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_DECK_H
#define TERMPOKER_DECK_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "Player.h"
#include "PokerCard.h"

class Deck {
    std::vector<std::unique_ptr<PokerCard>> cards = {};
    int deckNumber;
    std::vector<Player> players;
public:
    explicit Deck(const int deckNumber): deckNumber(deckNumber) {
        this->init();
        this->shuffle();
    };
    ~Deck() {
        this->cards.clear();
    };
    int getDeckSize() const {return this->cards.size();};
    void addPlayer(const Player& player) {
        this->players.push_back(player);
    };
    void init() {
        for (int i = 0; i < this->deckNumber; i ++) {
            for (auto poker_card_value_pair : getPokerCardValueIndex()) {
                auto pokerValue = poker_card_value_pair.first;
                if (pokerValue == POKER_CARD_VALUE::LargeJoker || pokerValue == POKER_CARD_VALUE::SmallJoker) {
                    std::unique_ptr<PokerCard> card(new PokerCard(pokerValue, POKER_CARD_TYPE::Joker));
                    this->cards.push_back(std::move(card));
                } else {
                    for (const auto cardType: {POKER_CARD_TYPE::Clubs, POKER_CARD_TYPE::Diamonds, POKER_CARD_TYPE::Hearts, POKER_CARD_TYPE::Spades}) {
                        std::unique_ptr<PokerCard> card(new PokerCard(pokerValue, cardType));
                        this->cards.push_back(std::move(card));
                    }
                }
            }
        }
    };
    void shuffle() {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    };
    void givePlayerCards() {
        for (int i = 0; i < cards.size(); i += this->players.size()) {
            for (int j = 0; j < this->players.size(); j++) {
                this->players[j].getCard(cards[i+j].get());
            }
        }
    };
    void printAllCards() const {
        for (auto &card: this->cards) {
            std::cout << card->getTypeString() << " " << card->getValueString() << std::endl;
        }
    };
    void printCardsByPlayer() const {
        for (int i = 0; i < this->players.size(); i++) {
            std::cout << "Player " << i+1 << std::endl;
            this->players[i].printCards();
        }
    };
};

#endif //TERMPOKER_DECK_H
