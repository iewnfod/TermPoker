cmake_minimum_required(VERSION 3.13)
project(termpoker)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/main.cpp
        include/PokerCard.h
        include/Deck.h
        include/Player.h
        include/Utils.h
        include/Game.h
        src/Game.cpp
        src/Utils.cpp
        src/Player.cpp
        include/store.h
        include/CardUtils.h
        src/Robot.cpp
        include/Robot.h
        src/AutoPlay.cpp
)

add_executable(termpoker ${SOURCE_FILES})
