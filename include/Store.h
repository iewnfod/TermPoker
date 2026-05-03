//
// Created by SYSTEM on 2026/5/2.
//

#ifndef TERMPOKER_STORE_H
#define TERMPOKER_STORE_H
#include <string>
#include "Utils.h"

class Store {
    std::string basePath = Utils::joinPath({Utils::getHomePath(), ".termpoker"});
public:
    /**
     * Expect to be $HOME/.termpoker on Unix-Like System or %USERPROFILE%\.termpoker on Windows.
     * @return base path of store.
     */
    std::string getBasePath() {return this->basePath;}
};

#endif //TERMPOKER_STORE_H
