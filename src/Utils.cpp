//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Utils.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <sys/ioctl.h>
#include <cstdlib>
#endif

#ifdef _WIN32
int Utils::getch() {
    return _getch();
}
#else
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
#endif

#ifdef _WIN32
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
#else
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
#endif
