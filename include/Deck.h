//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_DECK_H
#define TERMPOKER_DECK_H
#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include "Player.h"
#include "PokerCard.h"

struct PlayCardWithId {
    std::string id;
    std::vector<PokerCard*> cards;
};

struct PlayRound {
    int round;  // round number
    std::vector<PlayCardWithId> played;
};

class Deck {
    std::vector<std::unique_ptr<PokerCard>> cards = {};
    int deckNumber;
    std::vector<Player> players;
    GameDifficulty difficulty = GameDifficulty::Easy;
    bool isGivenCards = false;
    std::map<POKER_CARD_VALUE, int> leftCards;
    std::vector<POKER_CARD_VALUE> getRemainingCards() const;
    std::function<void()> handleQuit;
    std::vector<PlayRound> rounds;
    int currentRound = 1;
    std::string winner;

public:
    explicit Deck(const int deckNumber): deckNumber(deckNumber) {
        this->init();
        this->shuffle();
        for (const auto & card : cards) {
            if (leftCards.find(card->getValue()) != leftCards.end()) {
                leftCards[card->getValue()]++;
            } else {
                leftCards[card->getValue()] = 1;
            }
        }
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
            bindToPlayer(*player);
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
                this->players[j].receiveCard(cards[i+j].get());
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
            this->players[i].autoPlay(this->isNewRound());
            std::cout << "Robot " << i+1 << " played: " << this->getLastPlayedCardsString() << std::endl;
        }
    }

    bool isNewRound() const {
        return this->rounds.size() != this->currentRound;
    }

    std::vector<PokerCard*> getTrueLastPlayedCards() const {
        auto round = this->rounds.back();
        if (!round.played.empty()) {
            for (int i = static_cast<int>(round.played.size())-1; i >= 0; i --) {
                if (!round.played[i].cards.empty()) {
                    return round.played[i].cards;
                }
            }
            return {};
        } else {
            return {};
        }
    }

    std::vector<PokerCard*> getLastPlayedCards() const {
        auto round = this->rounds.back();
        if (!round.played.empty()) {
            return round.played.back().cards;
        } else {
            return {};
        }
    }

    std::string getLastPlayedCardsString() const {
        const auto cds = getLastPlayedCards();
        std::string s;
        for (const auto c : cds) {
            s += c->getValueString() + c->getTypeString() + " ";
        }
        return s;
    }

    bool onPlayerPlayCards(const std::string& id, const std::vector<PokerCard*>& cds) {
        if (!isNewRound()) {
            if (cds.empty()) {
                this->rounds.back().played.push_back({id, {}});
                this->tryNewRound();
                return true;
            }
            const auto lastPlayedCards = this->getLastPlayedCards();
            if (!CardUtils::compareCards(cds, lastPlayedCards)) {
                return false;
            } else {
                this->rounds.back().played.push_back({id, cds});
            }
        } else {
            PlayRound round = {currentRound, {}};
            round.played.push_back({id, cds});
            this->rounds.push_back(round);
        }
        for (const auto c : cds) {
            const auto cardValue = c->getValue();
            if (leftCards.find(cardValue) != leftCards.end()) {
                leftCards[cardValue]--;
            }
        }
        this->tryNewRound();
        if (this->checkWin()) {
            this->congratulateWin();
            this->quit();
        }
        return true;
    }

    void tryNewRound() {
        const auto played = this->rounds.back().played;
        bool canNewRoundOpen = true;
        if (played.size() >= this->players.size()) {
            for (int i = 1; i < this->players.size(); i++) {
                if (!played[played.size()-i].cards.empty()) {
                    canNewRoundOpen = false;
                    break;
                }
            }
        } else {
            canNewRoundOpen = false;
        }
        if (canNewRoundOpen) {
            this->newRound();
        }
    }

    void newRound() {
        this->currentRound += 1;
        for (auto & player : this->players) {
            player.setRoundNumber(currentRound);
        }
    }

    bool checkWin() {
        for (auto player : this->players) {
            if (player.getCards().empty()) {
                this->winner = player.getId();
                return true;
            }
        }
        return false;
    }

    void congratulateWin() const {
        for (auto player : this->players) {
            if (player.getId() == this->winner) {
                if (!player.getIsRobot()) {
                    std::cout << "Congratulation! You win this game!" << std::endl;
                } else {
                    std::cout << "You failed the game..." << std::endl;
                }
                this->outputCollectInfo();
                break;
            }
        }
    }

    void outputCollectInfo() const {
        std::cout << "The game finished in " << this->currentRound << " rounds" << std::endl;
    }

    void bindToPlayer(Player& player) {
        player.onPlayCards([this](const std::string& id, const std::vector<PokerCard*>& cds) {
            return this->onPlayerPlayCards(id, cds);
        });
        player.onPrintLeftCard([this]() {
            return this->leftCards;
        });
        player.onQuit([this]() {
            this->quit();
        });
        player.onGetLastPlayedCards([this]() {
            return this->getTrueLastPlayedCards();
        });
    }

    void onQuit(std::function<void()> q) {
        this->handleQuit = std::move(q);
    }

    void quit() const {
        if (this->handleQuit) {
            this->handleQuit();
        }
    }
};

#endif //TERMPOKER_DECK_H
