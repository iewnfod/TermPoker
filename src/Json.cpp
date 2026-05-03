//
// Created by Andy on 2026/5/3.
//

#include "../include/Player.h"
#include "../include/Game.h"

void to_json(json& j, const GameDifficulty& d) {
    switch (d) {
        case GameDifficulty::Easy:   j = "Easy"; break;
        case GameDifficulty::Medium: j = "Medium"; break;
        case GameDifficulty::Hard:   j = "Hard"; break;
        default:                     j = "Unknown";
    }
}
void from_json(const json& j, GameDifficulty& d) {
    auto s = j.get<std::string>();
    if (s == "Easy") d = GameDifficulty::Easy;
    else if (s == "Medium") d = GameDifficulty::Medium;
    else if (s == "Hard") d = GameDifficulty::Hard;
    else d = GameDifficulty::Easy;
}

void to_json(json& j, const PokerCard& card) {
    j = card.toString();
}
void from_json(const json& j, PokerCard& card) {
    card = PokerCard(j.get<std::string>());
}

void to_json(json& j, const Game::GameRecord& r) {
    j = json{
            {"timestamp", r.timestamp},
            {"difficulty", r.difficulty},
            {"playerInitialHand", r.playerInitialHand},
            {"plays", r.plays},
            {"result", r.result}
    };
}

void from_json(const json& j, Game::GameRecord& r) {
    j.at("timestamp").get_to(r.timestamp);
    j.at("difficulty").get_to(r.difficulty);
    j.at("playerInitialHand").get_to(r.playerInitialHand);
    j.at("plays").get_to(r.plays);
    j.at("result").get_to(r.result);
}

