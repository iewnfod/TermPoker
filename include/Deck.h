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
    /**
     * Init deck, shuffle cards, count left cards.
     * @param deckNumber number of deck of cards
     */
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

    /**
     * Destroy cards and deck.
     */
    ~Deck() {
        this->cards.clear();
    }

    /**
     * Get the number of all cards in this deck.
     * @return all card number
     */
    int getDeckSize() const {
        return static_cast<int>(this->cards.size());
    }

    /**
     * Add new player to this game.
     * @param player new player for the game
     */
    void addPlayer(const Player& player) {
        this->players.push_back(player);
    }

    /**
     * Automatically generate 4 players for this game, including 3 robots and 1 player.
     * @return user controlled player
     */
    Player* autoGeneratePlayers() {
        for (int i = 0; i < 4; i++) {
            const auto player = new Player(i != 0);
            bindToPlayer(*player);
            player->setIsTeamWithPlayer(i % 2 == 0);
            this->players.push_back(*player);
        }
        return &this->players[0];
    }

    /**
     * Init all cards needed.
     */
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

    /**
     * Shuffle cards into random order.
     */
    void shuffle() {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    /**
     * Assign cards to all players uniformly.
     */
    void givePlayerCards() {
        for (int i = 0; i < cards.size(); i += static_cast<int>(this->players.size())) {
            for (int j = 0; j < this->players.size(); j++) {
                this->players[j].receiveCard(cards[i+j].get());
            }
        }
        this->isGivenCards = true;
    }

    /**
     * @deprecated no need to print all cards now
     */
    void printAllCards() const {
        for (auto &card: this->cards) {
            std::cout << card->getTypeString() << " " << card->getValueString() << std::endl;
        }
    }

    /**
     * @deprecated no need to print cards by players individually, player can print it by themselves
     */
    void printCardsByPlayer() {
        for (int i = 0; i < this->players.size(); i++) {
            std::cout << "Player " << i+1 << std::endl;
            this->players[i].sortCards();
            this->players[i].printCards();
        }
    }

    /**
     * Set difficulty of the game.
     * @param d difficulty
     */
    void setDifficulty(const GameDifficulty d) {
        this->difficulty = d;
        for (auto & player : this->players) {
            player.setGameDifficulty(d);
        }
    }

    /**
     * @return has given player cards
     */
    bool hasGivenCards() const {
        return this->isGivenCards;
    }

    /**
     * Play robot and print what they played.
     */
    void robotPlayCards() {
        for (int i = 1; i < this->players.size(); i++) {
            this->players[i].autoPlay(this->isNewRound());
            std::cout << "Robot " << i+1 << " played: " << this->getLastPlayedCardsString() << std::endl;
        }
    }

    /**
     * @return true if this is a new round
     */
    bool isNewRound() const {
        return this->rounds.size()+1 == this->currentRound;
    }

    /**
     * Return the last played cards in this round. It will not be empty unless this is a new round.
     * @return vector of played cards
     */
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

    /**
     * Return what last player played. It might be empty if last player played nothing.
     * @return vector of cards
     */
    std::vector<PokerCard*> getLastPlayedCards() const {
        auto round = this->rounds.back();
        if (!round.played.empty()) {
            return round.played.back().cards;
        } else {
            return {};
        }
    }

    /**
     * @return string of what last player played
     */
    std::string getLastPlayedCardsString() const {
        const auto cds = getLastPlayedCards();
        std::string s;
        for (const auto c : cds) {
            s += c->getValueString() + c->getTypeString() + " ";
        }
        return s;
    }

    /**
     * When player play cards. Check if it is valid and record it.
     * @param id the player id
     * @param cds a vector of played cards
     * @return true if the cards are valid and played
     */
    bool onPlayerPlayCards(const std::string& id, const std::vector<PokerCard*>& cds) {
        if (cds.empty()) {
            if (!isNewRound()) {
                this->rounds.back().played.push_back({id, {}});
                this->tryNewRound();
                return true;
            } else {
                return false;
            }
        }
        if (!isNewRound()) {
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

    /**
     * Try to new a round if all players except the last played player played nothing.
     */
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

    /**
     * New a round and tell all players.
     */
    void newRound() {
        this->currentRound += 1;
        for (auto & player : this->players) {
            player.setRoundNumber(currentRound);
        }
    }

    /**
     * Check whether a player win depends on if they have played all of their cards and record the winner id.
     * @return true if a player win
     */
    bool checkWin() {
        for (auto player : this->players) {
            if (player.getCards().empty()) {
                this->winner = player.getId();
                return true;
            }
        }
        return false;
    }

    /**
     * Congratulation if someone win.
     */
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

    /**
     * Output the information for one whole round.
     */
    void outputCollectInfo() const {
        std::cout << "Player number: " << this->players.size() << std::endl;
        std::cout << "The game finished in " << this->currentRound << " rounds" << std::endl;
    }

    /**
     * Bind some events to a player so that the player can communicate data with the game.
     * @param player an instance of a player
     */
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

    /**
     * Register quit event.
     * @param q function when quit is called
     */
    void onQuit(std::function<void()> q) {
        this->handleQuit = std::move(q);
    }

    /**
     * Quit.
     */
    void quit() const {
        if (this->handleQuit) {
            this->handleQuit();
        }
    }
};

#endif //TERMPOKER_DECK_H
