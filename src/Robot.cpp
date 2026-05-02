//
// Created by muyunxi on 2026/5/3.
//

#include "../include/Robot.h"

std::vector<std::vector<PokerCard*>> Robot::extractBoom() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (count.find(val) != count.end() && count.at(val).size() >= 4) {
            result.push_back(takeCards(val, 4));
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractStraight() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 5)) {
            result.emplace_back();
            for (int i = 0; i < 5; ++i) {
                const int newValIndex = it.second + i;
                const auto cds = takeCards(inverseValueIndexMap.at(newValIndex), 1);
                result.back().push_back(cds[0]);
            }
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractSingle() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 1)) {
            result.push_back(takeCards(val, 1));
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractPair() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 1, 2)) {
            result.push_back(takeCards(val, 2));
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractTriple() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 1, 3)) {
            result.push_back(takeCards(val, 3));
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractDoubleTriple() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 2, 3)) {
            result.emplace_back();
            for (int i = 0; i < 2; i ++) {
                const auto nextValIt = inverseValueIndexMap.find(it.second+i);
                if (nextValIt == inverseValueIndexMap.end()) {
                    continue;
                }
                const auto nextVal = nextValIt->second;
                for (auto c : takeCards(nextVal, 3)) {
                    result.back().push_back(c);
                }
            }
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractTriplePair() {
    std::vector<std::vector<PokerCard*>> result = {};
    for (auto &it : valueIndexMap) {
        const POKER_CARD_VALUE val = it.first;
        while (hasConsecutive(val, 3, 2)) {
            result.emplace_back();
            for (int i = 0; i < 3; i ++) {
                const auto nextValIt = inverseValueIndexMap.find(it.second+i);
                if (nextValIt == inverseValueIndexMap.end()) {
                    continue;
                }
                const auto nextVal = nextValIt->second;
                for (auto c : takeCards(nextVal, 2)) {
                    result.back().push_back(c);
                }
            }
        }
    }
    return result;
}

std::vector<std::vector<PokerCard*>> Robot::extractThreePlusTwo() {
    std::vector<std::vector<PokerCard*>> result;
    for (auto& it : valueIndexMap) {
        const POKER_CARD_VALUE tripleVal = it.first;
        while (hasConsecutive(tripleVal, 1, 3)) {  // find 3 same cards
            bool findPair = false;
            for (auto& it2 : valueIndexMap) {
                const POKER_CARD_VALUE pairVal = it2.first;
                if (tripleVal == pairVal) {
                    continue;
                }
                if (hasConsecutive(pairVal, 1, 2)) {   // find a pair
                    auto tripleCards = takeCards(tripleVal, 3);
                    auto pairCards = takeCards(pairVal, 2);
                    tripleCards.insert(tripleCards.end(), pairCards.begin(), pairCards.end());
                    result.push_back(tripleCards);
                    findPair = true;
                    break;
                }
            }
            if (!findPair) break;
        }
    }
    return result;
}
