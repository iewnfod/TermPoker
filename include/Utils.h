//
// Created by SYSTEM on 2026/4/4.
//

#ifndef TERMPOKER_UTILS_H
#define TERMPOKER_UTILS_H
#include <codecvt>
#include <comcat.h>
#include <locale>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

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
    /**
     * Convert a string into wstring.
     * @param str string waiting to convert
     * @return str in wstring
     */
    static std::wstring string2wstring(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    /**
     * Get terminal type from reading env "TERM_PROGRAM" or "TERM".
     * @return terminal type in string
     */
    static std::string getTerminalType() {
        const char* term = std::getenv("TERM_PROGRAM");
        if (term) return std::string(term);
        term = std::getenv("TERM");
        if (term) return std::string(term);
        return "unknown";
    }

    /**
     * @return reset ascii color code
     */
    static std::string getResetColor() {
        return "\033[0m";
    }

    /**
     * Output the reset color code
     */
    static void resetColor() {
        std::cout << "\033[0m";
    }

    /**
     * @param color foreground color
     * @return ascii color code for that color
     */
    static std::string getFgColor(const TerminalColor color) {
        switch (color) {
            case TerminalColor::Dark:
                return "\033[30m";
            case TerminalColor::Red:
                return "\033[31m";
            case TerminalColor::Green:
                return "\033[32m";
            case TerminalColor::Yellow:
                return "\033[33m";
            case TerminalColor::Blue:
                return "\033[34m";
            case TerminalColor::Purple:
                return "\033[35m";
            case TerminalColor::LightBlue:
                return "\033[36m";
            case TerminalColor::Light:
                return "\033[37m";
        }
        return "";
    }

    /**
     * @param color background color
     * @return ascii color code for that color
     */
    static std::string getBgColor(const TerminalColor color) {
        switch (color) {
            case TerminalColor::Dark:
                return "\033[40m";
            case TerminalColor::Red:
                return "\033[41m";
            case TerminalColor::Green:
                return "\033[42m";
            case TerminalColor::Yellow:
                return "\033[43m";
            case TerminalColor::Blue:
                return "\033[44m";
            case TerminalColor::Purple:
                return "\033[45m";
            case TerminalColor::LightBlue:
                return "\033[46m";
            case TerminalColor::Light:
                return "\033[47m";
        }
        return "";
    }

    /**
     * Output the color code.
     * @param color foreground color
     */
    static void setFgColor(const TerminalColor color) {
        std::cout << getFgColor(color);
    }

    /**
     * Output the color code.
     * @param color background color
     */
    static void setBgColor(const TerminalColor color) {
        std::cout << getBgColor(color);
    }

    /**
     * Move cursor to front and clear last line.
     */
    static void cursorMoveAndClearLastLine() {
        std::cout << "\033[A\r\033[2K";
    }

    /**
     * Get char from user keyboard without waiting for an enter.
     * @return char int
     */
    static int getch();

    /**
     * @return column number of the terminal
     */
    static int getTermColumn();

    /**
     * @return home path in string
     */
    static std::string getHomePath();

    /**
     * @return path separator, '/' on Unix-Like or '\\' on Windows
     */
    static char getPathSeparator();

    /**
     * Combine multiple parts of paths into a string, instead add separators manually.
     * @param parts vector of parts of paths
     * @return a combined path in string
     */
    static std::string joinPath(const std::vector<std::string>& parts) {
        std::string result;
        const auto sep = getPathSeparator();
        for (int i = 0; i < parts.size(); i++) {
            const auto& p = parts[i];
            if (p.back() == sep || i == parts.size() - 1) {
                result += p;
            } else {
                result += p + sep;
            }
        }
        return result;
    }

    /**
     * Convert a wstring into string.
     * @param wstr wstring waiting to convert
     * @return wstr in string
     */
    static std::string wstring2string(const std::wstring& wstr) {
        std::string res;
        const int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
        if (len <= 0){
            return res;
        }
        const auto buffer = new char[len + 1];
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), buffer, len, nullptr, nullptr);
        buffer[len] = '\0';
        res.append(buffer);
        delete[] buffer;
        return res;
    }

    /**
     * The title would be same as the link.
     * @param link target link
     * @return clickable string
     */
    static std::string getClickableLink(const std::string& link) {
        return getClickableLink(link, link);
    }

    /**
     * Generate a clickable link in terminal with custom title.
     * @param title display content
     * @param link target link
     * @return clickable string
     */
    static std::string getClickableLink(const std::string& title, const std::string& link) {
        return "\033]8;;" + link + "\033\\" + title + "\033]8;;\033\\";
    }

    /**
     * Generate an uuid v4 string.
     * @return uuid
     */
    static std::string generateUuidV4() {
        static std::random_device              rd;
        static std::mt19937                    gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        static std::uniform_int_distribution<> dis2(8, 11);

        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
};

#endif //TERMPOKER_UTILS_H
