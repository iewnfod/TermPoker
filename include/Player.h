//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_PLAYER_H
#define TERMPOKER_PLAYER_H
#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "PokerCard.h"
#include "Utils.h"

enum class GameDifficulty {
    Easy, Medium, Hard
};

enum class PlayCardType {
    Single,  // 桥片
    Pair,  // 对子
    Triple,  // 三张一样的
    Boom,  // 炸弹
    ThreePlusOne,  // 三带一
    ThreePlusTwo,  // 三带二
    Straight,  // 顺子
    Flush,  // 同花顺
    DoubleTriple,  // 钢板（连续两个三张一样的）
    TriplePair,  // 姐妹对（连续三个对子）
    Invalid,
};

class Player {
    std::vector<PokerCard*> cards = {};
    bool isRobot = true;
    std::vector<PokerCard*> selectedCards = {};
    PokerCard* selectedCard = nullptr;
    std::vector<std::vector<PokerCard*>> playedCards = {};
    std::string hint;
    GameDifficulty difficulty = GameDifficulty::Easy;
    bool isTeamWithPlayer = false;
    std::function<void(POKER_CARD_VALUE)> handlePlayCard;

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
public:
    Player() = default;

    explicit Player(const bool isRobot) {
        this->isRobot = isRobot;
    }

    void getCard(PokerCard* card) {
        cards.push_back(card);
    }

    static int getThreePlusXMainValueIndex(const std::vector<PokerCard*>& c) {
        if (c[0]->getValueIndex() == c[1]->getValueIndex() && c[1]->getValueIndex() == c[2]->getValueIndex()) {
            return c.front()->getValueIndex();
        } else {
            return c.back()->getValueIndex();
        }
    }

    // return true if c1 is larger then c2
    static bool compareCards(const std::vector<PokerCard*>& c1, const std::vector<PokerCard*>& c2) {
        const auto c1Type = getPlayCardType(c1), c2Type = getPlayCardType(c2);
        if (c1Type != c2Type) {
            return false;
        }
        const bool firstCmp = c1[0]->getValueIndex() > c2[0]->getValueIndex();
        switch (c1Type) {
            case PlayCardType::Single:
                return firstCmp;
            case PlayCardType::Pair:
                return firstCmp;
            case PlayCardType::Triple:
                return firstCmp;
            case PlayCardType::Straight:
                return firstCmp;
            case PlayCardType::Flush:
                return firstCmp;
            case PlayCardType::DoubleTriple:
                return firstCmp;
            case PlayCardType::TriplePair:
                return firstCmp;
            case PlayCardType::Boom:
                if (c1.size() > c2.size()) {
                    return true;
                } else if (c1.size() == c2.size()) {
                    return firstCmp;
                } else {
                    return false;
                }
            case PlayCardType::ThreePlusOne:
                return getThreePlusXMainValueIndex(c1) > getThreePlusXMainValueIndex(c2);
            case PlayCardType::ThreePlusTwo:
                return getThreePlusXMainValueIndex(c1) > getThreePlusXMainValueIndex(c2);
            case PlayCardType::Invalid:
                return false;
        }
        return false;
    }

    static PlayCardType getPlayCardType(const std::vector<PokerCard*>& cds) {
        bool isSameValue = true, isStraight = true, isSameType = true;

        for (int i = 0; i < cds.size(); i ++) {
            if (cds[i]->getValueIndex() != cds.back()->getValueIndex()) {
                isSameValue = false;
            }
            if (i != cds.size()-1 && cds[i]->getValueIndex()+1 != cds[i+1]->getValueIndex()) {
                isStraight = false;
            }
            if (cds[i]->getType() != cds.back()->getType()) {
                isSameType = false;
            }
        }

        switch (cds.size()) {
            case 1:
                return PlayCardType::Single;
            case 2:
                if (isSameValue) {
                    return PlayCardType::Pair;
                } else {
                    return PlayCardType::Invalid;
                }
            case 3:
                if (isSameValue) {
                    return PlayCardType::Triple;
                }
            case 4:
                if (isSameValue) {
                    return PlayCardType::Boom;
                } else if (
                    (cds[0]->getValueIndex() == cds[1]->getValueIndex() && cds[1]->getValueIndex() == cds[2]->getValueIndex())
                    || (cds[1]->getValueIndex() == cds[2]->getValueIndex() && cds[2]->getValueIndex() == cds[3]->getValueIndex())
                ) {
                    return PlayCardType::ThreePlusOne;
                } else {
                    return PlayCardType::Invalid;
                }
            case 5:
                if (isSameValue) {
                    return PlayCardType::Boom;
                } else if (isStraight) {
                    if (isSameType) {
                        return PlayCardType::Flush;
                    } else {
                        return PlayCardType::Straight;
                    }
                } else {
                    if (
                        (cds[0]->getValueIndex() == cds[1]->getValueIndex() && cds[1]->getValueIndex() == cds[2]->getValueIndex()
                        && cds[3]->getValueIndex() == cds[4]->getValueIndex())
                        || (cds[2]->getValueIndex() == cds[3]->getValueIndex() && cds[3]->getValueIndex() == cds[4]->getValueIndex()
                        && cds[0]->getValueIndex() == cds[1]->getValueIndex())
                    ) {
                        return PlayCardType::ThreePlusTwo;
                    } else {
                        return PlayCardType::Invalid;
                    }
                }
            case 6:
                if (isSameValue) {
                    return PlayCardType::Boom;
                } else if (
                    cds[0]->getValueIndex() == cds[1]->getValueIndex() && cds[1]->getValueIndex() == cds[2]->getValueIndex()
                    && cds[3]->getValueIndex() == cds[4]->getValueIndex() && cds[4]->getValueIndex() == cds[5]->getValueIndex()
                    && cds[0]->getValueIndex()+1 == cds[3]->getValueIndex()
                ) {
                    return PlayCardType::DoubleTriple;
                } else if (
                    cds[0]->getValueIndex() == cds[1]->getValueIndex() && cds[2]->getValueIndex() == cds[3]->getValueIndex()
                    && cds[4]->getValueIndex() == cds[5]->getValueIndex()
                    && cds[0]->getValueIndex()+1 == cds[2]->getValueIndex()
                    && cds[2]->getValueIndex()+1 == cds[4]->getValueIndex()
                ) {
                    return PlayCardType::TriplePair;
                } else {
                    return PlayCardType::Invalid;
                }
            default:
                return PlayCardType::Invalid;
        }
    }

    static std::string getPlayCardTypeString(const PlayCardType t) {
        std::map<PlayCardType, std::string> m = {
            {PlayCardType::Single, "Single"},
            {PlayCardType::Pair, "Pair"},
            {PlayCardType::Triple, "Triple"},
            {PlayCardType::Boom, "Boom"},
            {PlayCardType::ThreePlusOne, "Three Plus One"},
            {PlayCardType::ThreePlusTwo, "Three Plus Two"},
            {PlayCardType::Straight, "Straight"},
            {PlayCardType::Flush, "Flush"},
            {PlayCardType::DoubleTriple, "Double Triple"},
            {PlayCardType::TriplePair, "Triple Pair"},
            {PlayCardType::Invalid, "Invalid"},
        };
        return m[t];
    }

    bool checkIsSelectedCardTypeValid() const {
        return getPlayCardType(this->selectedCards) != PlayCardType::Invalid;
    }

    void playCards(const std::vector<PokerCard*>& cds) {
        for (auto c : cds) {
            auto it = std::find(cards.begin(), cards.end(), c);
            if (it != cards.end()) {
                this->handlePlayCard(c->getValue());
                cards.erase(it);
            }
        }
        playedCards.push_back(cds);
    }

    void playSelectedCards() {
        if (checkIsSelectedCardTypeValid()) {
            playCards(selectedCards);
            selectedCards = {};
            selectedCard = nullptr;
        };
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

    void autoPlay();

    std::string getLastPlayedCardsString() const {
        std::string cardsString;
        for (auto c : this->playedCards.back()) {
            cardsString += c->getValueString() + c->getTypeString();
            if (Utils::getTerminalType() == "xterm-256color") {
                cardsString += " ";
            }
            cardsString += " ";
        }
        return cardsString;
    }

    bool getIsTeamWithPlayer() const {
        return this->isTeamWithPlayer;
    }

    void onPlayCard(std::function<void(POKER_CARD_VALUE)> f) {
        this->handlePlayCard = std::move(f);
    }
};

#endif //TERMPOKER_PLAYER_H
