//
// Created by SYSTEM on 2026/4/14.
//

#include <cmath>
#include <iostream>
#include <array>
#include <set>

#include "../include/Player.h"
#include "../include/Utils.h"

void Player::printCards() const {
    const auto resetColor = Utils::getResetColor();
    const auto selectColor = Utils::getFgColor(TerminalColor::Yellow);

    std::string uh;  // upper head to display selected cards
    std::string head, tail;
    std::string body1, body2, body3;
    auto term = Utils::getTerminalType();
    bool isUpArr[this->cards.size()];
    int selectedIndex = -1;
    for (int i = 0; i < this->cards.size(); i++) {
        isUpArr[i] = std::find(this->selectedCards.begin(), this->selectedCards.end(), this->cards[i])
            != this->selectedCards.end();
        if (this->cards[i] == this->selectedCard) {
            selectedIndex = i;
        }
    }
    for (int ci = 0; ci < this->cards.size(); ci++) {
        auto card = this->cards[ci];

        auto isUp = isUpArr[ci];
        bool isSelected = ci == selectedIndex;
        bool isLastUp = false;
        bool isLastSelected = false;
        if (ci != 0) {
            isLastUp = isUpArr[ci-1];
            isLastSelected = ci-1 == selectedIndex;
        }
        bool isNextUp = false;
        bool isNextSelected = false;
        if (ci != this->cards.size()-1) {
            isNextUp = isUpArr[ci+1];
            isNextSelected = ci+1 == selectedIndex;
        }

        auto t = card->getTypeString();
        auto v = card->getValueString();
        const int vWidth = static_cast<int>(v.length());
        const int tsWidth = static_cast<int>(Utils::string2wstring(t).length());
        int widthOffset = 0;
        int numberOffset = 0;
        if (term == "xterm-256color") {
            widthOffset += 1;
            numberOffset += 1;
            if (card->getType() == POKER_CARD_TYPE::Joker) {
                widthOffset -= 2;
            }
        }

        const int width = vWidth + 4 + widthOffset;

        if (isSelected) {
            uh += selectColor; head += selectColor; body1 += selectColor; body2 += selectColor; body3 += selectColor; tail += selectColor;
        }

        if (ci == 0) {
            if (isUp) {
                uh += "┌";
                head += "│";
            } else {
                uh += " ";
                head += "┌";
            }
            tail += "└";
        }

        for (int i = 0; i < width; i++) {
            tail += "─";
            if (!isUp) {
                head += "─";
                uh += " ";
            } else {
                if (i == 0 && ci != 0 && !isLastUp) {
                    uh += "┌";
                } else {
                    uh += "─";
                }
                head += " ";
            }
        }
        if (isUp) {
            if (!isLastUp && ci != 0) {
                uh += "─";
            }
            if (isNextUp && isNextSelected) {
                uh += selectColor + "┐";
                uh += resetColor;
            } else {
                uh += "┐";
            }
            if (isNextSelected) {
                head += selectColor + "│";
                head += resetColor;
            } else {
                head += "│";
            }
        } else {
            if (isNextUp) {
                if (isNextSelected) {
                    head += selectColor + "│";
                    head += resetColor;
                } else {
                    head += "│";
                }
            } else {
                if (isNextSelected) {
                    head += selectColor + "┐";
                    head += resetColor;
                } else {
                    head += "┐";
                }
                uh += " ";
            }
        }
        if (isNextSelected) {
            tail += selectColor + "┘";
            tail += resetColor;
        } else {
            tail += "┘";
        }
        if (isLastSelected) {
            body1 += selectColor + "│ ";
            body1 += resetColor;
        } else {
            body1 += "│ ";
        }
        body1 += t;
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body1 += " ";
        }
        const int body2Padding = static_cast<int>(std::ceil((width-vWidth-1)/2.0)) + numberOffset;
        if (isLastSelected) {
            body2 += selectColor + "│";
            body2 += resetColor;
            body3 += selectColor + "│";
            body3 += resetColor;
        } else {
            body2 += "│";
            body3 += "│";
        }
        for (int i = 0; i < body2Padding; i++) {
            body2 += " ";
        }
        body2 += v;
        for (int i = 0; i < width-vWidth-body2Padding; i++) {
            body2 += " ";
        }
        for (int i = 0; i < width-tsWidth-1+widthOffset; i++) {
            body3 += " ";
        }
        body3 += t + " ";

        if (isSelected) {
            uh += resetColor; head += resetColor; body1 += resetColor; body2 += resetColor; body3 += resetColor; tail += resetColor;
        }
    }
    if (cards.size()-1 == selectedIndex) {
        body1 += selectColor; body2 += selectColor; body3 += selectColor;
    }
    body1 += "│"; body2 += "│"; body3 += "│";
    body1 += resetColor; body2 += resetColor; body3 += resetColor;
    if (this->cards.empty()) {
        uh.clear(); head.clear(); body1.clear(); body2.clear(); body3.clear(); tail.clear();
    }
    std::cout
        << uh << std::endl
        << head << std::endl
        << body1 << std::endl
        << body2 << std::endl
        << body3 << std::endl
        << tail << std::endl
        << "Card type: " << CardUtils::getPlayCardTypeString(CardUtils::getPlayCardType(this->selectedCards)) << std::endl
        << Utils::getFgColor(TerminalColor::Red) << hint << Utils::getResetColor() << std::endl;
}

std::vector<PokerCard*> Player::waitForUserInput() {
    std::cout << std::endl;
    std::cout << "Use <left⬅️> or <right➡️> to select, <space␣> to choose, <enter↩️> to confirm, <s> to skip, and <q> to exit this game." << std::endl;
    std::cout << "Round " << this->roundNumber << std::endl;
    this->hint.clear();
    this->printLeftCards();

    sortCards();
    if (this->selectedCard == nullptr) {
        cardMoveRight();
    }

    int ch = 0;
    while (true) {
        printCards();

        ch = Utils::getch();
        if (ch == 0x1B) {
            if (Utils::getch() == 0x5B) {
                switch (Utils::getch()) {
                    case 0x44:  // Left
                        this->cardMoveLeft();
                        break;
                    case 0x43:  // Right
                        this->cardMoveRight();
                        break;
                    default: break;
                }
            }
        }
        #ifdef _WIN32
        else if (ch == 0xE0 || ch == 0x00) {  // Windows Extend
            switch (Utils::getch()) {
                case 0x4B:  // Left
                    this->cardMoveLeft();
                    break;
                case 0x4D:  // Right
                    this->cardMoveRight();
                    break;
                default: break;
            }
        }
        #endif
        else if (ch == ' ') {
            this->selectCard();
        }
        else if (ch == 'q') {
            this->quit();
            break;
        }
        else if (ch == 's') {
            if (this->playCards({})) {
                break;
            } else {
                this->hint = "At least play one cards in a new round";
            }
        }
        else if (ch == '\n' || ch == '\r') {
            if (checkIsSelectedCardTypeValid()) {
                if (this->playSelectedCards()) {
                    break;
                } else {
                    this->hint = "Selected cards are not large enough or have different card type";
                }
            } else {
                if (this->selectedCards.empty()) {
                    this->hint = "Select at least one card";
                } else {
                    this->hint = "Invalid card type";
                }
            }
        }

        clearCards();
    }

    return this->selectedCards;
}

void Player::autoPlay(const bool isNewRound) {
    sortCards();
    if (isNewRound) {
        this->playCards({this->cards.front()});
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
                if (idx <= 13) // 3~2，排除大小王（15,16）
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

void Player::printLeftCards() const {
    if (this->handlePrintLeftCards && this->difficulty == GameDifficulty::Easy) {
        std::map<POKER_CARD_VALUE, int> remainingCardsInGame = this->handlePrintLeftCards();
        std::vector<std::array<std::string, 3>> remainCards = {};
        for (const auto& kv : remainingCardsInGame) {
            const int count = kv.second;
            if (count == 0) continue;
            const auto tmpC = new PokerCard(kv.first, POKER_CARD_TYPE::Clubs);
            const std::string cardValue = tmpC->getValueString();
            delete tmpC;
            std::string head = "┌─", body = "│ ", tail = "└─";
            for (const char c : cardValue) {
                head += "─";
                tail += "─";
                body += c;
            }
            head += "─┐"; body += " │"; tail += "─┘";
            body += "x" + std::to_string(count);
            for (int i = 0; i < std::to_string(count).size()+1; i++) {
                head += " ";
                tail += " ";
            }
            remainCards.push_back({head, body, tail});
        }
        int column = Utils::getTermColumn(), printedColumn = 0;
        std::cout << "Remaining cards: " << std::endl;
        std::string head, body, tail;
        for (const auto& c : remainCards) {
            const int width = static_cast<int>(Utils::string2wstring(c[0]).size());
            if (printedColumn + width >= column) {
                std::cout << head << std::endl << body << std::endl << tail << std::endl;
                printedColumn = 0;
                head.clear(); body.clear(); tail.clear();
            }
            printedColumn += width + 1;
            head += c[0] + " "; body += c[1] + " "; tail += c[2] + " ";
        }
        if (!head.empty()) {
            std::cout << head << std::endl << body << std::endl << tail << std::endl;
        }
    }
}
