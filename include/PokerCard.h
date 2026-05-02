//
// Created by muyunxi on 2026/3/22.
//

#ifndef TERMPOKER_POKERCARD_H
#define TERMPOKER_POKERCARD_H
#include <map>
#include <string>

enum class POKER_CARD_VALUE {
    N3, N4, N5, N6, N7, N8, N9, N10, J, Q, K, A, N2, SmallJoker, LargeJoker, Unknown
};

enum class POKER_CARD_TYPE {
    Clubs, Diamonds, Hearts, Spades, Joker, Unknown
};

inline std::map<POKER_CARD_VALUE, int> getPokerCardValueIndex() {
    static std::map<POKER_CARD_VALUE, int> POKER_CARD_VALUE_INDEX = {
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
        {POKER_CARD_VALUE::A, 12},
        {POKER_CARD_VALUE::N2, 13},
        {POKER_CARD_VALUE::SmallJoker, 15},
        {POKER_CARD_VALUE::LargeJoker, 16},
    };
    return POKER_CARD_VALUE_INDEX;
}

class PokerCard {
    POKER_CARD_VALUE value;
    POKER_CARD_TYPE type;
public:
    PokerCard(const POKER_CARD_VALUE v, const POKER_CARD_TYPE t): value(v), type(t) {}

    explicit PokerCard(const std::string& s) {
        const auto sep = s.find('|');
        if (sep != std::string::npos) {
            const auto t = s.substr(0, sep);
            const auto v = s.substr(sep + 1);
            this->setTypeByString(t);
            this->setValueByString(v);
        } else {
            this->type = POKER_CARD_TYPE::Unknown;
            this->value = POKER_CARD_VALUE::Unknown;
        }
    }

    POKER_CARD_VALUE getValue() const {return value;}
    POKER_CARD_TYPE getType() const {return type;}

    void setTypeByString(const std::string &t) {
        if (t == "clubs") {
            this->type = POKER_CARD_TYPE::Clubs;
        } else if (t == "diamonds") {
            this->type = POKER_CARD_TYPE::Diamonds;
        } else if (t == "hearts") {
            this->type = POKER_CARD_TYPE::Hearts;
        } else if (t == "spades") {
            this->type = POKER_CARD_TYPE::Spades;
        } else if (t == "joker") {
            this->type = POKER_CARD_TYPE::Joker;
        } else {
            this->type = POKER_CARD_TYPE::Unknown;
        }
    }

    void setValueByString(const std::string &v) {
        if (v == "3") {
            this->value = POKER_CARD_VALUE::N3;
        } else if (v == "4") {
            this->value = POKER_CARD_VALUE::N4;
        } else if (v == "5") {
            this->value = POKER_CARD_VALUE::N5;
        } else if (v == "6") {
            this->value = POKER_CARD_VALUE::N6;
        } else if (v == "7") {
            this->value = POKER_CARD_VALUE::N7;
        } else if (v == "8") {
            this->value = POKER_CARD_VALUE::N8;
        } else if (v == "9") {
            this->value = POKER_CARD_VALUE::N9;
        } else if (v == "10") {
            this->value = POKER_CARD_VALUE::N10;
        } else if (v == "J") {
            this->value = POKER_CARD_VALUE::J;
        } else if (v == "Q") {
            this->value = POKER_CARD_VALUE::Q;
        } else if (v == "K") {
            this->value = POKER_CARD_VALUE::K;
        } else if (v == "A") {
            this->value = POKER_CARD_VALUE::A;
        } else if (v == "2") {
            this->value = POKER_CARD_VALUE::N2;
        } else if (v == "SJ") {
            this->value = POKER_CARD_VALUE::SmallJoker;
        } else if (v == "LJ") {
            this->value = POKER_CARD_VALUE::LargeJoker;
        } else {
            this->value = POKER_CARD_VALUE::Unknown;
        }
    }

    std::string toString() const {
        std::string s;
        switch (this->type) {
            case POKER_CARD_TYPE::Clubs:
                s = "clubs";
                break;
            case POKER_CARD_TYPE::Diamonds:
                s = "diamonds";
                break;
            case POKER_CARD_TYPE::Hearts:
                s = "hearts";
                break;
            case POKER_CARD_TYPE::Spades:
                s = "spades";
                break;
            case POKER_CARD_TYPE::Joker:
                s = "joker";
                break;
            case POKER_CARD_TYPE::Unknown:
                break;
        }
        return s + "|" + getValueString();
    }

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
            case POKER_CARD_TYPE::Unknown:
                break;
        }
        return "";
    }

    int getValueIndex() const {
        const auto indexMap = getPokerCardValueIndex();
        return indexMap.at(value);
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
                return "SJ";
            case POKER_CARD_VALUE::LargeJoker:
                return "LJ";
            case POKER_CARD_VALUE::Unknown:
                break;
        }
        return "";
    }
};

#endif //TERMPOKER_POKERCARD_H
