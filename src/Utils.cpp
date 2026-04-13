//
// Created by muyunxi on 2026/4/13.
//

#include "../include/Utils.h"

#ifdef _WIN32
#include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <cstdio>
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
