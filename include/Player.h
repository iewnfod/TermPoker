//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_PLAYER_H
#define TERMPOKER_PLAYER_H
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "CardUtils.h"
#include "PokerCard.h"
#include "Utils.h"

enum class GameDifficulty {
    Easy, Medium, Hard
};

class Player {
    std::string id = Utils::generateUuidV4();
    std::vector<PokerCard*> cards = {};
    bool isRobot = true;
    std::vector<PokerCard*> selectedCards = {};
    PokerCard* selectedCard = nullptr;
    std::string hint;
    GameDifficulty difficulty = GameDifficulty::Easy;
    bool isTeamWithPlayer = false;
    std::function<std::map<POKER_CARD_VALUE, int>()> handlePrintLeftCards;
    std::function<void()> handleQuit;
    std::function<bool(std::string, std::vector<PokerCard*>)> handlePlayCards;
    std::function<std::vector<PokerCard*>()> getLastPlayedCards;
    int roundNumber = 1;

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
        for (int i = 0; i < 7; i ++) {
            Utils::cursorMoveAndClearLastLine();
        }
    }

    void printLeftCards() const;
public:
    Player() = default;

    explicit Player(const bool isRobot) {
        this->isRobot = isRobot;
    }

    std::string getId() {
        return this->id;
    }

    void receiveCard(PokerCard* card) {
        cards.push_back(card);
    }

    bool checkIsSelectedCardTypeValid() const {
        return CardUtils::getPlayCardType(this->selectedCards) != PlayCardType::Invalid;
    }

    bool playCards(const std::vector<PokerCard*>& cds) {
        if (this->handlePlayCards(this->id, cds)) {
            for (auto c : cds) {
                auto it = std::find(cards.begin(), cards.end(), c);
                if (it != cards.end()) {
                    cards.erase(it);
                }
            }
            return true;
        } else {
            return false;
        }
    }

    bool playSelectedCards() {
        if (checkIsSelectedCardTypeValid()) {
            if (playCards(selectedCards)) {
                selectedCards = {};
                selectedCard = nullptr;
                return true;
            }
        }
        return false;
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

    void setGameDifficulty(GameDifficulty diff) {
        difficulty = diff;
    }

    void setIsTeamWithPlayer(bool b) {
        isTeamWithPlayer = b;
    }

    std::vector<PokerCard*> waitForUserInput();

    void autoPlay(bool isNewRound);

    bool getIsTeamWithPlayer() const {
        return this->isTeamWithPlayer;
    }

    void onPlayCards(std::function<bool(std::string, std::vector<PokerCard*>)> f) {
        this->handlePlayCards = std::move(f);
    }

    void onPrintLeftCard(std::function<std::map<POKER_CARD_VALUE, int>()> provider) {
        this->handlePrintLeftCards = std::move(provider);
    }

    void onQuit(std::function<void()> f) {
        this->handleQuit = std::move(f);
    }

    void onGetLastPlayedCards(std::function<std::vector<PokerCard*>()> f) {
        this->getLastPlayedCards = std::move(f);
    }

    void quit() const {
        if (this->handleQuit) {
            this->handleQuit();
        }
    }

    void setRoundNumber(const int n) {
        this->roundNumber = n;
    }

    bool getIsRobot() const {
        return this->isRobot;
    }
};

#endif //TERMPOKER_PLAYER_H
