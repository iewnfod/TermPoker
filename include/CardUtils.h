//
// Created by SYSTEM on 2026/5/2.
//

#ifndef TERMPOKER_CARDUTILS_H
#define TERMPOKER_CARDUTILS_H
#include <vector>

#include "PokerCard.h"

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

class CardUtils {
public:
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
                } else {
                    return PlayCardType::Invalid;
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
};

#endif //TERMPOKER_CARDUTILS_H
