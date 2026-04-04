//
// Created by SYSTEM on 2026/4/4.
//

#ifndef TERMPOKER_UTILS_H
#define TERMPOKER_UTILS_H
#include <codecvt>
#include <locale>

class Utils {
public:
    static std::wstring to_wstring(const std::string& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(str);
    }
};

#endif //TERMPOKER_UTILS_H
