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
    GameDifficulty difficulty = GameDifficulty::Easy;
    bool isGivenCards = false;
public:
    explicit Deck(const int deckNumber): deckNumber(deckNumber) {
        this->init();
        this->shuffle();
    }
    ~Deck() {
        this->cards.clear();
    }
    int getDeckSize() const {
        return static_cast<int>(this->cards.size());
    }
    void addPlayer(const Player& player) {
        this->players.push_back(player);
    }
    Player* autoGeneratePlayers() {
        for (int i = 0; i < 4; i++) {
            const auto player = new Player(i != 0);
            player->setIsTeamWithPlayer(i % 2 == 0);
            this->players.push_back(*player);
        }
        return &this->players[0];
    }
    void init() {
        for (int i = 0; i < this->deckNumber; i ++) {
            for (const auto poker_card_value_pair : getPokerCardValueIndex()) {
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
    }
    void shuffle() {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }
    void givePlayerCards() {
        for (int i = 0; i < cards.size(); i += static_cast<int>(this->players.size())) {
            for (int j = 0; j < this->players.size(); j++) {
                this->players[j].getCard(cards[i+j].get());
            }
        }
        this->isGivenCards = true;
    }
    void printAllCards() const {
        for (auto &card: this->cards) {
            std::cout << card->getTypeString() << " " << card->getValueString() << std::endl;
        }
    }
    void printCardsByPlayer() {
        for (int i = 0; i < this->players.size(); i++) {
            std::cout << "Player " << i+1 << std::endl;
            this->players[i].sortCards();
            this->players[i].printCards();
        }
    }
    void setDifficulty(const GameDifficulty d) {
        this->difficulty = d;
        for (auto & player : this->players) {
            player.setGameDifficulty(d);
        }
    }
    bool hasGivenCards() const {
        return this->isGivenCards;
    }
    void robotPlayCards() {
        for (int i = 1; i < this->players.size(); i++) {
            this->players[i].autoPlay();
            if (this->players[i].getIsTeamWithPlayer()) {
                std::cout << "Teammate ";
            } else {
                std::cout << "Opponent ";
            }
            std::cout << "Robot " << i+1 << " played: " << this->players[i].getLastPlayedCardsString() << std::endl;
        }
    }
};

#endif //TERMPOKER_DECK_H
