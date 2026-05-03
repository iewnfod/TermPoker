//
// Created by muyunxi on 2026/5/3.
//

#ifndef TERMPOKER_ROBOT_H
#define TERMPOKER_ROBOT_H
#include <algorithm>
#include <functional>
#include <vector>

#include "CardUtils.h"
#include "Player.h"
#include "PokerCard.h"

struct CardGroup {
    std::vector<PokerCard*> cards;
    PlayCardType type;
};

struct Plan {
    std::vector<CardGroup> plan;
};

enum class PlanStrategy {
    BALANCED,
    STRAIGHT,
    PAIR,
    THREE_PLUS_TWO,
};

class Robot {
    std::vector<PokerCard*> cards;
    std::vector<Plan> plans;
    std::map<POKER_CARD_VALUE, std::vector<PokerCard*>> count;
    std::map<POKER_CARD_VALUE, std::vector<PokerCard*>> countBackup;
    std::vector<POKER_CARD_VALUE> activeValues;
    const std::map<POKER_CARD_VALUE, int> valueIndexMap = getPokerCardValueIndex();
    const std::map<int, POKER_CARD_VALUE> inverseValueIndexMap = getInversePokerCardValueIndex();
    std::map<POKER_CARD_VALUE, int> remainCards;
    int leastRemainCards = 0;
    GameDifficulty difficulty = GameDifficulty::Medium;

    /**
     * Find consecutive cards in count.
     * @param startIdx start card value
     * @param len how many consecutive cards to find
     * @return true if there exist
     */
    bool hasConsecutive(const POKER_CARD_VALUE startIdx, const int len) {
        return hasConsecutive(startIdx, len, 1);
    }

    /**
     * Find consecutive cards in count.
     * @param startIdx start card value
     * @param len how many consecutive cards to find
     * @param size the number of same cards need to exist
     * @return true if there exist
     */
    bool hasConsecutive(const POKER_CARD_VALUE startIdx, const int len, const int size) {
        const auto vIndex = valueIndexMap.at(startIdx);
        for (int i = 0; i < len; i++) {
            int nextIdx = vIndex + i;
            auto it = inverseValueIndexMap.find(nextIdx);
            if (it == inverseValueIndexMap.end()) return false;
            POKER_CARD_VALUE nextVal = it->second;
            if (count.find(nextVal) == count.end() || count[nextVal].size() < size) return false;
        }
        return true;
    }

    /**
     * Take cards from count.
     * @param value card value
     * @param counts card number
     * @return the taken card
     */
    std::vector<PokerCard*> takeCards(const POKER_CARD_VALUE value, const int counts) {
        if (count.find(value) != count.end()) {
            std::vector<PokerCard*> cds = {};
            for (int i = 0; i < counts; i++) {
                auto c = count.at(value).back();
                if (c != nullptr) {
                    count.at(value).pop_back();
                    cds.push_back(c);
                }
            }
            return cds;
        } else {
            return {};
        }
    }

    /**
     * Extract Boom type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractBoom();

    /**
     * Extract Straight type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractStraight();

    /**
     * Extract Triple type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractTriple();

    /**
     * Extract Pair type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractPair();

    /**
     * Extract Single type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractSingle();

    /**
     * Extract Three Plus Two type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractThreePlusTwo();

    /**
     * Extract Double Triple type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractDoubleTriple();

    /**
     * Extract Triple Pair type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractTriplePair();

    /**
     * Extract Double Triple, Triple Pair, and Straight type from count.
     * @return a vector of card group
     */
    std::vector<CardGroup> extractConsecutiveEntities() {
        std::vector<CardGroup> p = {};
        for (const auto& i : extractDoubleTriple()) {
            p.push_back(i);
        };
        for (const auto& i : extractTriplePair()) {
            p.push_back(i);
        }
        for (const auto& i : extractStraight()) {
            p.push_back(i);
        }
        return p;
    }

    /**
     * Clear empty keys in count.
     */
    void clearEmptyKey() {
        for (auto it = count.begin(); it != count.end();) {
            if (it->second.empty()) it = count.erase(it);
            else ++it;
        }
    }

    /**
     * @param s plan strategy
     * @return a sequence of functions to extract cards
     */
    std::vector<std::function<std::vector<CardGroup>()>> getStrategyActions(const PlanStrategy s) {
        if (s == PlanStrategy::BALANCED) {
            return {
                [this]() {return this->extractBoom();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractPair();},
                [this]() {return this->extractSingle();},
            };
        } else if (s == PlanStrategy::STRAIGHT) {
            return {
                [this]() {return this->extractStraight();},
                [this]() {return this->extractBoom();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractPair();},
                [this]() {return this->extractSingle();},
            };
        } else if (s == PlanStrategy::PAIR) {
            return {
                [this]() {return this->extractPair();},
                [this]() {return this->extractBoom();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractSingle();},
            };
        } else if (s == PlanStrategy::THREE_PLUS_TWO) {
            return {
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractBoom();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractPair();},
                [this]() {return this->extractSingle();},
            };
        }
        return {};
    }

    /**
     * @param group card group
     * @return the power of the group
     */
    static int cardGroupPower(const CardGroup& group) {
        int base = 0;
        switch (group.type) {
            case PlayCardType::Single:
                base = 0; break;
            case PlayCardType::Pair:
                base = 100; break;
            case PlayCardType::Triple:
                base = 200; break;
            case PlayCardType::Straight:
                base = 300; break;
            case PlayCardType::TriplePair:
                base = 400; break;
            case PlayCardType::DoubleTriple:
                base = 500; break;
            case PlayCardType::ThreePlusTwo:
                base = 600; break;
            case PlayCardType::Boom:
                base = 1000; break;
            default: break;
        }
        int maxIndex = 0;
        for (const auto c : group.cards) {
            maxIndex = std::max(maxIndex, c->getValueIndex());
        }
        return base + maxIndex;
    }

    /**
     * Run a strategy of extracting cards and push the new plan to plans.
     * @param s strategy of the extracting cards
     */
    void runStrategy(const PlanStrategy s) {
        count = countBackup;
        Plan plan;
        for (const auto& f : getStrategyActions(s)) {
            auto extracted = f();
            plan.plan.insert(plan.plan.end(), extracted.begin(), extracted.end());
            clearEmptyKey();
        }
        bool hasSingle = false;
        for (const auto& group : plan.plan) {
            if (group.type == PlayCardType::Single) hasSingle = true;
        }
        if (cards.size() <= 4 && hasSingle) {
            std::sort(plan.plan.begin(), plan.plan.end(), [](const CardGroup& a, const CardGroup& b) {
                return cardGroupPower(a) < cardGroupPower(b);
            });
        }
        this->plans.push_back(plan);
    }

    /**
     * Estimate the probability for a pattern of cards to win
     * @param startIdx start value
     * @param len number consecutive cards
     * @param size number of same card value
     * @return a probability from 0 to 1
     */
    double estimateProb(const POKER_CARD_VALUE startIdx, const int len, const int size) const {
        int biggerLeft = 0;

        const auto vIndex = valueIndexMap.at(startIdx);
        for (auto& pair : valueIndexMap) {
            const auto idx = pair.second;
            if (idx > vIndex) {
                bool validPattern = true;
                for (int i = 0; i < len; i++) {
                    int nextIdx = idx + i;
                    auto it = inverseValueIndexMap.find(nextIdx);
                    if (it == inverseValueIndexMap.end()) {
                        validPattern = false;
                        break;
                    };
                    POKER_CARD_VALUE nextVal = it->second;
                    if (remainCards.find(nextVal) == remainCards.end() || remainCards.at(nextVal) < size) {
                        validPattern = false;
                        break;
                    };
                }
                if (validPattern) {
                    biggerLeft++;
                }
            }
        }

        if (biggerLeft == 0) return 0.95;
        if (biggerLeft <= 2) return 0.6;
        return 0.3;
    }

public:
    /**
     * Init Robot.
     * @param cards cards for plan
     */
    explicit Robot(const std::vector<PokerCard*> &cards) {
        this->cards = cards;
        CardUtils::sortCards(this->cards);
        for (const auto c : cards) {
            const auto v = c->getValue();
            if (countBackup.find(v) == countBackup.end()) {
                countBackup.insert({v, {c}});
            } else {
                countBackup[v].push_back(c);
            }
            if (std::find(activeValues.begin(), activeValues.end(), v) == activeValues.end()) {
                activeValues.push_back(v);
            }
        }
    }

    /**
     * @param cds all remain cards in the game, including the robot itself
     */
    void setRemainCards(const std::map<POKER_CARD_VALUE, int>& cds) {
        this->remainCards = cds;
    }

    /**
     * @param n the least remain card number in the game, except the robot itself
     */
    void setLeastRemainCards(const int n) {
        this->leastRemainCards = n;
    }

    /**
     * @param diff difficulty of the game
     */
    void setGameDifficulty(const GameDifficulty diff) {
        this->difficulty = diff;
    }

    /**
     * Generate plans for various strategies.
     */
    void generatePlans() {
        runStrategy(PlanStrategy::BALANCED);
        runStrategy(PlanStrategy::STRAIGHT);
        runStrategy(PlanStrategy::PAIR);
        runStrategy(PlanStrategy::THREE_PLUS_TWO);
    }

    /**
     * Get the plan with the lowest cost.
     * @return the best plan
     */
    Plan getBestPlan() const {
        Plan plan;
        double planCost = 1e9;
        for (const auto& p : plans) {
            const double cost = evaluatePlan(p);
            if (cost < planCost) {
                plan = p;
                planCost = cost;
            }
        }
        return plan;
    }

    /**
     * @return all plans
     */
    std::vector<Plan> getPlans() {
        return this->plans;
    }

    /**
     * Sort plans depends on their cost, from small to large.
     */
    void sortPlans() {
        std::sort(plans.begin(), plans.end(), [this](const Plan& p1, const Plan& p2) {
            return evaluatePlan(p1) < evaluatePlan(p2);
        });
    }

    /**
     * Estimate the cost depends on vairous information.
     * @param plan plan to evaluate cost
     * @return a cost value in double
     */
    double evaluatePlan(const Plan& plan) const {
        const auto totalRounds = static_cast<double>(plan.plan.size());
        double weakRounds = 0, boomRounds = 0;
        for (const auto& block : plan.plan) {
            if (block.type == PlayCardType::Boom) {
                boomRounds++;
            }
            const auto mainValue = block.cards[0]->getValue();
            if (difficulty == GameDifficulty::Hard) {
                double winProb = 0.5;
                if (block.type == PlayCardType::Single) {
                    winProb = estimateProb(mainValue, 1, 1);
                } else if (block.type == PlayCardType::Pair) {
                    winProb = estimateProb(mainValue, 1, 2);
                } else if (block.type == PlayCardType::Triple) {
                    winProb = estimateProb(mainValue, 1, 3);
                } else if (block.type == PlayCardType::DoubleTriple) {
                    winProb = estimateProb(mainValue, 2, 3);
                } else if (block.type == PlayCardType::TriplePair) {
                    winProb = estimateProb(mainValue, 3, 2);
                } else if (block.type == PlayCardType::Straight) {
                    winProb = estimateProb(mainValue, 5, 1);
                } else if (block.type == PlayCardType::ThreePlusTwo) {
                    winProb = estimateProb(mainValue, 1, 3) * estimateProb(POKER_CARD_VALUE::N3, 1, 2);
                }
                if (winProb < 0.5) weakRounds++;
            } else {
                if (valueIndexMap.at(mainValue) <= valueIndexMap.at(POKER_CARD_VALUE::N10)) {
                    weakRounds++;
                }
            }
        }

        double cost = totalRounds * 1.0;
        cost += weakRounds * 1.5;

        if (difficulty == GameDifficulty::Hard) {
            if (leastRemainCards <= 3) {
                boomRounds *= 1.7;
            } else if (leastRemainCards <= 10) {
                boomRounds *= 1.3;
            } else {
                boomRounds *= 0.5;
            }
        }

        cost -= boomRounds * 0.8;

        return cost;
    }
};

#endif //TERMPOKER_ROBOT_H
