//
// Created by muyunxi on 2026/5/3.
//

#include <set>

#include "../include/Player.h"
#include "../include/Robot.h"

void Player::autoPlayEasy(const bool isNewRound) {
    sortCards();
    if (isNewRound) {

        this->sortCards();
        int minRank = this->cards.front()->getValueIndex();

        int minRankCount = 0;
        for (auto* c : this->cards) {
            if (c->getValueIndex() == minRank) {
                ++minRankCount;
            } else {
                break;
            }
        }

        std::vector<PokerCard*> firstPlay;

        if (minRankCount >= 3) {

            for (auto* c : this->cards) {
                if (c->getValueIndex() == minRank && firstPlay.size() < 3) {
                    firstPlay.push_back(c);
                }
            }

            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            int pairRank = -1;
            for (auto& it : rankCnt) {
                if (it.second >= 2 && it.first != minRank) {
                    pairRank = it.first;
                    break;
                }
            }
            if (pairRank != -1) {

                for (auto* c : this->cards) {
                    if (c->getValueIndex() == pairRank && firstPlay.size() < 5) {
                        firstPlay.push_back(c);
                    }
                }
            } else {

                firstPlay.clear();
            }
        }

        if (firstPlay.empty() && minRankCount >= 2) {
            for (auto* c : this->cards) {
                if (c->getValueIndex() == minRank && firstPlay.size() < 2) {
                    firstPlay.push_back(c);
                }
            }
        }

        if (firstPlay.empty()) {

            std::set<int> availableRanks;
            for (auto* c : this->cards) {
                int r = c->getValueIndex();
                if (r <= 13) availableRanks.insert(r);
            }

            int straightLen = 1;
            int cur = minRank;
            while (availableRanks.count(cur + 1)) {
                ++cur;
                ++straightLen;
            }
            if (straightLen >= 5) {

                for (int r = minRank; r <= minRank + straightLen - 1; ++r) {
                    for (auto* c : this->cards) {
                        if (c->getValueIndex() == r) {
                            firstPlay.push_back(c);
                            break;
                        }
                    }
                }
            }
        }

        if (firstPlay.empty()) {
            firstPlay = {this->cards.front()};
        }

        this->playCards(firstPlay);
        return;
    }

    const auto lastPlayed = this->getLastPlayedCards();
    const auto lastType = CardUtils::getPlayCardType(lastPlayed);

    // find the smallest card larger than last played card
    auto findSameTypeBeat = [&]() -> std::vector<PokerCard*> {
        std::vector<PokerCard*> result;

        if (lastType == PlayCardType::Single) {
            PokerCard* minBigger = nullptr;
            for (auto* c : this->cards) {
                if (CardUtils::compareCards({c}, lastPlayed)) {
                    minBigger = c;
                    break;
                }
            }
            if (minBigger) result = {minBigger};
        }

        else if (lastType == PlayCardType::Pair) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            int lastRank = lastPlayed[0]->getValueIndex();
            int targetRank = -1;
            for (auto & it : rankCnt) {
                if (it.second >= 2 && it.first > lastRank) {
                    targetRank = it.first;
                    break;
                }
            }
            if (targetRank != -1) {
                for (auto* c : this->cards) {
                    if (c->getValueIndex() == targetRank && result.size() < 2)
                        result.push_back(c);
                }

            }
        }

        else if (lastType == PlayCardType::Triple) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            int lastRank = lastPlayed[0]->getValueIndex();
            int targetRank = -1;
            for (auto & it : rankCnt) {
                if (it.second >= 3 && it.first > lastRank) {
                    targetRank = it.first;
                    break;
                }
            }
            if (targetRank != -1) {
                for (auto* c : this->cards) {
                    if (c->getValueIndex() == targetRank && result.size() < 3)
                        result.push_back(c);
                }
            }
        }

        else if (lastType == PlayCardType::ThreePlusTwo) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            int lastMainRank = CardUtils::getThreePlusXMainValueIndex(lastPlayed);
            int targetMainRank = -1;
            for (auto & it : rankCnt) {
                if (it.second >= 3 && it.first > lastMainRank) {
                    targetMainRank = it.first;
                    break;
                }
            }
            if (targetMainRank != -1) {
                for (auto* c : this->cards) {
                    if (c->getValueIndex() == targetMainRank && result.size() < 3)
                        result.push_back(c);
                }

                std::map<int, int> remainingCnt;
                for (auto* c : this->cards) {
                    if (c->getValueIndex() != targetMainRank)
                        remainingCnt[c->getValueIndex()]++;
                }
                int pairRank = -1;
                for (auto & it : remainingCnt) {
                    if (it.second >= 2) {
                        pairRank = it.first;
                        break;
                    }
                }
                if (pairRank != -1) {
                    std::vector<PokerCard*> pairCards;
                    for (auto* c : this->cards) {
                        if (c->getValueIndex() == pairRank && pairCards.size() < 2)
                            pairCards.push_back(c);
                    }
                    result.insert(result.end(), pairCards.begin(), pairCards.end());
                }
            }
        }

        else if (lastType == PlayCardType::Straight) {
            int straightLen = lastPlayed.size();
            int lastStartRank = lastPlayed[0]->getValueIndex();
            std::set<int> uniqueRanks;
            for (auto* c : this->cards) {
                int idx = c->getValueIndex();
                if (idx <= 13)
                    uniqueRanks.insert(idx);
            }
            std::vector<int> sortedRanks(uniqueRanks.begin(), uniqueRanks.end());
            for (size_t i = 0; i + straightLen <= sortedRanks.size(); ++i) {
                bool continuous = true;
                for (int j = 1; j < straightLen; ++j) {
                    if (sortedRanks[i + j] != sortedRanks[i + j - 1] + 1) {
                        continuous = false;
                        break;
                    }
                }
                if (continuous && sortedRanks[i] > lastStartRank) {
                    for (int k = 0; k < straightLen; ++k) {
                        int needRank = sortedRanks[i + k];
                        for (auto* c : this->cards) {
                            if (c->getValueIndex() == needRank) {
                                result.push_back(c);
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        }

        else if (lastType == PlayCardType::Flush) {
            int flushLen = lastPlayed.size();
            int lastStartRank = lastPlayed[0]->getValueIndex();
            POKER_CARD_TYPE lastSuit = lastPlayed[0]->getType();
            std::vector<PokerCard*> sameSuit;
            for (auto* c : this->cards) {
                if (c->getType() == lastSuit && c->getValueIndex() <= 13)
                    sameSuit.push_back(c);
            }
            if (sameSuit.size() >= flushLen) {
                std::set<int> ranks;
                for (auto* c : sameSuit) ranks.insert(c->getValueIndex());
                std::vector<int> sortedRanks(ranks.begin(), ranks.end());
                for (size_t i = 0; i + flushLen <= sortedRanks.size(); ++i) {
                    bool continuous = true;
                    for (int j = 1; j < flushLen; ++j) {
                        if (sortedRanks[i + j] != sortedRanks[i + j - 1] + 1) {
                            continuous = false;
                            break;
                        }
                    }
                    if (continuous && sortedRanks[i] > lastStartRank) {
                        for (int k = 0; k < flushLen; ++k) {
                            int needRank = sortedRanks[i + k];
                            for (auto* c : sameSuit) {
                                if (c->getValueIndex() == needRank) {
                                    result.push_back(c);
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        else if (lastType == PlayCardType::DoubleTriple) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            std::vector<int> tripleRanks;
            for (auto & it : rankCnt) {
                if (it.second >= 3) tripleRanks.push_back(it.first);
            }
            std::sort(tripleRanks.begin(), tripleRanks.end());
            int lastStartRank = lastPlayed[0]->getValueIndex();
            for (size_t i = 0; i + 1 < tripleRanks.size(); ++i) {
                if (tripleRanks[i+1] == tripleRanks[i] + 1 && tripleRanks[i] > lastStartRank) {
                    for (int k = 0; k < 2; ++k) {
                        int r = tripleRanks[i + k];
                        int taken = 0;
                        for (auto* c : this->cards) {
                            if (c->getValueIndex() == r && taken < 3) {
                                result.push_back(c);
                                ++taken;
                            }
                        }
                    }
                    break;
                }
            }
        }

        else if (lastType == PlayCardType::TriplePair) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            std::vector<int> pairRanks;
            for (auto & it : rankCnt) {
                if (it.second >= 2) pairRanks.push_back(it.first);
            }
            std::sort(pairRanks.begin(), pairRanks.end());
            int lastStartRank = lastPlayed[0]->getValueIndex();
            for (size_t i = 0; i + 2 < pairRanks.size(); ++i) {
                if (pairRanks[i+1] == pairRanks[i] + 1 &&
                    pairRanks[i+2] == pairRanks[i] + 2 &&
                    pairRanks[i] > lastStartRank) {
                    for (int k = 0; k < 3; ++k) {
                        int r = pairRanks[i + k];
                        int taken = 0;
                        std::vector<PokerCard*> pair;
                        for (auto* c : this->cards) {
                            if (c->getValueIndex() == r && taken < 2) {
                                pair.push_back(c);
                                ++taken;
                            }
                        }
                        result.insert(result.end(), pair.begin(), pair.end());
                    }
                    break;
                }
            }
        }

        else if (lastType == PlayCardType::Boom) {
            std::map<int, int> rankCnt;
            for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
            int lastRank = lastPlayed[0]->getValueIndex();
            int targetRank = -1;
            for (auto & it : rankCnt) {
                if (it.second >= 4 && ((it.second == lastPlayed.size() && it.first > lastRank) || it.second >= lastPlayed.size())) {
                    targetRank = it.first;
                    break;
                }
            }
            if (targetRank != -1) {
                for (auto* c : this->cards) {
                    if (c->getValueIndex() == targetRank)
                        result.push_back(c);
                }
            }
        }
        return result;
    };

    // find any bomb if no cards match last played card type
    auto findAnyBomb = [this]() -> std::vector<PokerCard*> {
        std::vector<PokerCard*> bomb;
        std::map<int, int> rankCnt;
        for (auto* c : this->cards) rankCnt[c->getValueIndex()]++;
        int targetRank = -1;
        for (auto & it : rankCnt) {
            if (it.second >= 4) {
                targetRank = it.first;
                break;
            }
        }
        if (targetRank != -1) {
            for (auto* c : this->cards) {
                if (c->getValueIndex() == targetRank)
                    bomb.push_back(c);
            }
        }
        return bomb;
    };

    // main logic
    std::vector<PokerCard*> toPlay = findSameTypeBeat();
    if (toPlay.empty() && lastType != PlayCardType::Boom) {
        toPlay = findAnyBomb();
    }

    if (!toPlay.empty()) {
        if (!this->playCards(toPlay)) {
            this->playCards({});
        };
    } else {
        this->playCards({});
    }
}

void Player::autoPlayHard(const bool isNewRound) {
    const auto remainCards = handlePrintLeftCards();
    const auto leastRemainCards = handleGetLeastRemainCards();
    auto robot = Robot(this->cards);
    robot.setGameDifficulty(difficulty);
    robot.setRemainCards(remainCards);
    robot.setLeastRemainCards(leastRemainCards);
    robot.generatePlans();
    if (isNewRound) {
        const auto bestPlan = robot.getBestPlan();
        auto it = bestPlan.plan.begin();
        while (it != bestPlan.plan.end()) {
            if (it->type != PlayCardType::Boom) {
                playCards(it->cards);
                return;
            } else {
                ++it;
            }
        }
        playCards(bestPlan.plan.front().cards);
    } else {
        robot.sortPlans();
        const auto lastPlayedCards = getLastPlayedCards();
        for (auto& plan : robot.getPlans()) {
            for (auto& g : plan.plan) {
                if (CardUtils::compareCards(g.cards, lastPlayedCards) && g.type != PlayCardType::Boom) {
                    if (!playCards(g.cards)) {
                        playCards({});
                    }
                    return;
                }
            }

            const bool shouldBoom = (this->cards.size() <= 6)
                || (leastRemainCards <= 10)
                || lastPlayedCards.at(0)->getValueIndex() > getPokerCardValueIndex().at(POKER_CARD_VALUE::N2);

            if (shouldBoom) {
                for (auto& p : robot.getPlans()) {
                    for (auto& g : p.plan) {
                        if (CardUtils::compareCards(g.cards, lastPlayedCards) && g.type == PlayCardType::Boom) {
                            if (!playCards(g.cards)) {
                                playCards({});
                            }
                            return;
                        }
                    }
                }
            }
        }
        playCards({});
    }
}
