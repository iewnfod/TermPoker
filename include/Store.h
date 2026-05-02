//
// Created by SYSTEM on 2026/5/2.
//

#ifndef TERMPOKER_STORE_H
#define TERMPOKER_STORE_H
#include <string>

class Store {
    std::string basePath = Utils::joinPath({Utils::getHomePath(), ".termpoker"});
public:
    std::string getBasePath() {return this->basePath;}
};

#endif //TERMPOKER_STORE_H
