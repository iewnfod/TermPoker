//
// Created by muyunxi on 2026/5/3.
//

#ifndef TERMPOKER_ROBOT_H
#define TERMPOKER_ROBOT_H
#include <algorithm>
#include <functional>
#include <vector>

#include "CardUtils.h"
#include "PokerCard.h"

struct Plan {
    std::vector<std::vector<PokerCard*>> plan;
};

enum class PlanStrategy {
    BALANCED,
    STRAIGHT,
    PAIR,
    FLEXIBLE,
};

class Robot {
    std::vector<PokerCard*> cards;
    std::vector<Plan> plans;
    std::map<POKER_CARD_VALUE, std::vector<PokerCard*>> count;
    std::map<POKER_CARD_VALUE, std::vector<PokerCard*>> countBackup;
    std::vector<POKER_CARD_VALUE> activeValues;
    const std::map<POKER_CARD_VALUE, int> valueIndexMap = getPokerCardValueIndex();
    const std::map<int, POKER_CARD_VALUE> inverseValueIndexMap = getInversePokerCardValueIndex();

    bool hasConsecutive(const POKER_CARD_VALUE startIdx, const int len) {
        return hasConsecutive(startIdx, len, 1);
    }

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

    std::vector<std::vector<PokerCard*>> extractBoom();
    std::vector<std::vector<PokerCard*>> extractStraight();
    std::vector<std::vector<PokerCard*>> extractTriple();
    std::vector<std::vector<PokerCard*>> extractPair();
    std::vector<std::vector<PokerCard*>> extractSingle();
    std::vector<std::vector<PokerCard*>> extractThreePlusTwo();
    std::vector<std::vector<PokerCard*>> extractDoubleTriple();
    std::vector<std::vector<PokerCard*>> extractTriplePair();
    std::vector<std::vector<PokerCard*>> extractConsecutiveEntities() {
        std::vector<std::vector<PokerCard*>> p = {};
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

    void clearEmptyKey() {
        for (auto it = count.begin(); it != count.end();) {
            if (it->second.empty()) it = count.erase(it);
            else ++it;
        }
    }

    std::vector<std::function<std::vector<std::vector<PokerCard*>>()>> getStrategyActions(const PlanStrategy s) {
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
                [this]() {return this->extractBoom();},
                [this]() {return this->extractStraight();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractPair();},
                [this]() {return this->extractSingle();},
            };
        } else if (s == PlanStrategy::PAIR) {
            return {
                [this]() {return this->extractBoom();},
                [this]() {return this->extractPair();},
                [this]() {return this->extractConsecutiveEntities();},
                [this]() {return this->extractThreePlusTwo();},
                [this]() {return this->extractTriple();},
                [this]() {return this->extractSingle();},
            };
        }
        return {};
    }

    void runStrategy(const PlanStrategy s) {
        Plan plan;
        for (const auto& f : getStrategyActions(s)) {
            auto extracted = f();
            plan.plan.insert(plan.plan.end(), extracted.begin(), extracted.end());
            clearEmptyKey();
        }
        this->plans.push_back(plan);
    }

public:
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

    void generatePlans() {
        count = countBackup;
        runStrategy(PlanStrategy::BALANCED);
        count = countBackup;
        runStrategy(PlanStrategy::STRAIGHT);
        count = countBackup;
        runStrategy(PlanStrategy::PAIR);
    }

    Plan getPlanWithSmallestRounds() const {
        Plan plan;
        for (const auto& p : plans) {
            if (p.plan.size() < plan.plan.size() || plan.plan.empty()) {
                plan = p;
            }
        }
        return plan;
    }
};

#endif //TERMPOKER_ROBOT_H
