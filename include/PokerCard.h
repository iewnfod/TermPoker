//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_POKERCARD_H
#define TERMPOKER_POKERCARD_H
#include <map>
#include <string>

enum class POKER_CARD_VALUE {
    N3, N4, N5, N6, N7, N8, N9, N10, J, Q, K, A, N2, SmallJoker, LargeJoker
};

enum class POKER_CARD_TYPE {
    Clubs, Diamonds, Hearts, Spades, Joker
};

extern std::map<POKER_CARD_VALUE, int> POKER_CARD_VALUE_INDEX = {
    {POKER_CARD_VALUE::N3, 1},
    {POKER_CARD_VALUE::N4, 2},
    {POKER_CARD_VALUE::N5, 3},
    {POKER_CARD_VALUE::N6, 4},
    {POKER_CARD_VALUE::N7, 5},
    {POKER_CARD_VALUE::N8, 6},
    {POKER_CARD_VALUE::N9, 7},
    {POKER_CARD_VALUE::N10, 8},
    {POKER_CARD_VALUE::J, 9},
    {POKER_CARD_VALUE::Q, 10},
    {POKER_CARD_VALUE::K, 11},
    {POKER_CARD_VALUE::SmallJoker, 12},
    {POKER_CARD_VALUE::LargeJoker, 13},
};

class PokerCard {
    POKER_CARD_VALUE value;
    POKER_CARD_TYPE type;
public:
    PokerCard(const POKER_CARD_VALUE v, const POKER_CARD_TYPE t): value(v), type(t) {};
    POKER_CARD_VALUE getValue() const {return value;};
    POKER_CARD_TYPE getType() const {return type;};
    std::string getTypeString() const {
        switch (this->type) {
            case POKER_CARD_TYPE::Clubs:
                return "♣️";
            case POKER_CARD_TYPE::Diamonds:
                return "♦️";
            case POKER_CARD_TYPE::Hearts:
                return "♥️";
            case POKER_CARD_TYPE::Spades:
                return "♠️";
            case POKER_CARD_TYPE::Joker:
                return "⭐";
        }
        return "";
    }

    std::string getValueString() const {
        switch (this->value) {
            case POKER_CARD_VALUE::N3:
                return "3";
            case POKER_CARD_VALUE::N4:
                return "4";
            case POKER_CARD_VALUE::N5:
                return "5";
            case POKER_CARD_VALUE::N6:
                return "6";
            case POKER_CARD_VALUE::N7:
                return "7";
            case POKER_CARD_VALUE::N8:
                return "8";
            case POKER_CARD_VALUE::N9:
                return "9";
            case POKER_CARD_VALUE::N10:
                return "10";
            case POKER_CARD_VALUE::J:
                return "J";
            case POKER_CARD_VALUE::Q:
                return "Q";
            case POKER_CARD_VALUE::K:
                return "K";
            case POKER_CARD_VALUE::A:
                return "A";
            case POKER_CARD_VALUE::N2:
                return "2";
            case POKER_CARD_VALUE::SmallJoker:
                return "Small Joker";
            case POKER_CARD_VALUE::LargeJoker:
                return "Large Joker";
        }
        return "";
    }
};

#endif //TERMPOKER_POKERCARD_H
