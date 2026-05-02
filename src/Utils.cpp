//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Utils.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#include <shlobj.h>
#include <sstream>
#include <comcat.h>

int Utils::getch() {
    return _getch();
}

int Utils::getTermColumn() {
    try {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        const int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return columns;
    } catch (std::exception& e) {
        return 80;
    }
}

std::string Utils::getHomePath() {
    char homePath[MAX_PATH];
    const HRESULT hr = SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, homePath);

    if (SUCCEEDED(hr)) {
        return std::string(homePath);
    } else {
        const char* home = getenv("USERPROFILE");
        return home;
    }
}

char Utils::getPathSeparator() {
    return '\\';
}

std::string Utils::wstring2string(const std::wstring& wstr) {
    const int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string result(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], len, nullptr, nullptr);
    return result;
}

#else
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <cstdlib>
#include <pwd.h>

int Utils::getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int Utils::getTermColumn() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    const char* col = std::getenv("COLUMNS");
    if (col != nullptr) {
        int columns = std::atoi(col);
        if (columns > 0) {
            return columns;
        }
    }
    return 80;
}

std::string Utils::getHomePath() {
    const char* home = getenv("HOME");
    if (!home) {
        struct passwd* pw = getpwuid(getuid());
        home = pw->pw_dir;
    }
    return home;
}

char Utils::getPathSeparator() {
    return '/';
}

std::string Utils::wstring2string(const std::wstring& wstr) {
    std::mbstate_t state{};
    const wchar_t* src = wstr.c_str();

    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) {
        return "";
    }

    std::string result(len + 1, '\0');
    state = std::mbstate_t{};
    src = wstr.c_str();

    std::wcsrtombs(&result[0], &src, result.size(), &state);
    result.resize(len);
    return result;
}
#endif
