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

    /**
     * Mark or unmark the card with cursor as selected and automatically move cursor to next card in the right.
     */
    void selectCard() {
        if (selectedCard == nullptr) {
            return;
        }
        const auto it = std::find(selectedCards.begin(), selectedCards.end(), selectedCard);
        if (it == selectedCards.end()) {
            selectedCards.push_back(selectedCard);
        } else {
            selectedCards.erase(it);
        }
        this->cardMoveRight();
    }

    /**
     * Move card cursor to the one in the left.
     */
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

    /**
     * Move card cursor to the one in the right.
     */
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

    /**
     * Clear printed cards data for next print.
     */
    static void clearCards() {
        for (int i = 0; i < 8; i ++) {
            Utils::cursorMoveAndClearLastLine();
        }
    }

    /**
     * Print remained cards for easy mode.
     */
    void printLeftCards() const;
public:
    Player() = default;

    /**
     * @param isRobot if this is a robot player
     */
    explicit Player(const bool isRobot) {
        this->isRobot = isRobot;
    }

    /**
     * @return id of this player
     */
    std::string getId() {
        return this->id;
    }

    /**
     * Receive card from deck and save it.
     * @param card new card the player received
     */
    void receiveCard(PokerCard* card) {
        cards.push_back(card);
    }

    /**
     * @return true if the current selected cards have a valid type
     */
    bool checkIsSelectedCardTypeValid() const {
        return CardUtils::getPlayCardType(this->selectedCards) != PlayCardType::Invalid;
    }

    /**
     * Inform deck to play card and erase played cards from local cards.
     * @param cds a vector of cards waiting for play
     * @return true if played successfully
     */
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

    /**
     * Check if selected cards can play and clear selected cards after play succeed.
     * @return true if selected cards played successfully
     */
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

    /**
     * @return player's available cards
     */
    std::vector<PokerCard*> getCards() const {
        return cards;
    }

    /**
     * Print all available cards in a pretty way.
     */
    void printCards() const;

    /**
     * @param b is a robot player
     */
    void setIsRobot(bool b) {
        isRobot = b;
    }

    /**
     * @return true if this player is a robot
     */
    bool getIsRobot() const {
        return this->isRobot;
    }

    /**
     * @param diff game difficulty
     */
    void setGameDifficulty(const GameDifficulty diff) {
        difficulty = diff;
    }

    /**
     * @param b if the robot is team with the user
     * @deprecated we are not making team now, everyone play for themselves
     */
    void setIsTeamWithPlayer(bool b) {
        isTeamWithPlayer = b;
    }

    /**
     * @return true if this robot is a teammate with user
     * @deprecated we are not making team now, everyone play for themselves
     */
    bool getIsTeamWithPlayer() const {
        return this->isTeamWithPlayer;
    }

    /**
     * Wait for user to choose play cards and play it.
     * @return cards the player played
     */
    std::vector<PokerCard*> waitForUserInput();

    /**
     * For robots, automatically play cards.
     * @param isNewRound if this is a new round
     */
    void autoPlay(bool isNewRound);

    /**
     * Register the event which will be called when the player played cards.
     * @param f play card function, with an player id and played cards as param, it should return true if cards are played successfully
     */
    void onPlayCards(std::function<bool(std::string, std::vector<PokerCard*>)> f) {
        this->handlePlayCards = std::move(f);
    }

    /**
     * Register the event which will be called when the player want to know remain cards.
     * @param provider get remain cards function, return a map from POKER_CARD_VALUE to a number representing how many of this value are remained
     */
    void onPrintLeftCard(std::function<std::map<POKER_CARD_VALUE, int>()> provider) {
        this->handlePrintLeftCards = std::move(provider);
    }

    /**
     * Register the event which will be called when the player want to exit the game.
     * @param f quit function
     */
    void onQuit(std::function<void()> f) {
        this->handleQuit = std::move(f);
    }

    /**
     * Register the event which will be called when the player want to now what last player has played.
     * @param f last played function, return a vector of poker cards, it should not be empty unless this is a new round
     */
    void onGetLastPlayedCards(std::function<std::vector<PokerCard*>()> f) {
        this->getLastPlayedCards = std::move(f);
    }

    /**
     * Called when player want to exit the game.
     */
    void quit() const {
        if (this->handleQuit) {
            this->handleQuit();
        }
    }

    /**
     * @param n new round number
     */
    void setRoundNumber(const int n) {
        this->roundNumber = n;
    }

    /**
     * Sort cards for this player.
     */
    void sortCards() {
        CardUtils::sortCards(this->cards);
    }
};

#endif //TERMPOKER_PLAYER_H
