//
// Created by SYSTEM on 2026/4/4.
//

#ifndef TERMPOKER_UTILS_H
#define TERMPOKER_UTILS_H
#include <codecvt>
#include <locale>
#include <iostream>

enum class TerminalColor {
    Dark,
    Red,
    Green,
    Yellow,
    Blue,
    Purple,
    LightBlue,
    Light,
};

class Utils {
public:
    static std::wstring to_wstring(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    static std::string getTerminalType() {
        const char* term = std::getenv("TERM_PROGRAM");
        if (term) return std::string(term);
        term = std::getenv("TERM");
        if (term) return std::string(term);
        return "unknown";
    }

    static void resetColor() {
        std::cout << "\033[0m";
    }

    static void setFgColor(TerminalColor color) {
        switch (color) {
            case TerminalColor::Dark:
                std::cout << "\033[30m";
                break;
            case TerminalColor::Red:
                std::cout << "\033[31m";
                break;
            case TerminalColor::Green:
                std::cout << "\033[32m";
                break;
            case TerminalColor::Yellow:
                std::cout << "\033[33m";
                break;
            case TerminalColor::Blue:
                std::cout << "\033[34m";
                break;
            case TerminalColor::Purple:
                std::cout << "\033[35m";
                break;
            case TerminalColor::LightBlue:
                std::cout << "\033[36m";
                break;
            case TerminalColor::Light:
                std::cout << "\033[37m";
                break;
        }
    }

    static void cursorMoveAndClearLastLine() {
        std::cout << "\033[A\r\033[2K";
    }

    static int getch();
};

#endif //TERMPOKER_UTILS_H
