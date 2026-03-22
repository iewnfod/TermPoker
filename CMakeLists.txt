cmake_minimum_required(VERSION 3.13)
project(termpoker)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/main.cpp
        include/PokerCard.h
        include/Deck.h
        include/Player.h
)

add_executable(termpoker ${SOURCE_FILES})
