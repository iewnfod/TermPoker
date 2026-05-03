<p align="center">
<h3 align="center">TermPoker</h3>
</p>
<p align="center">
<a href="./README_zh.md">简体中文</a> | <a href="./README.md">English</a>
</p>

> This project use some AI tools for design and optimization, but it is not a vide-coding project.

---

## Group Members

| Member    | GitHub |
|-----------|--------|
| Wu Ziqian | [@iewnfod](https://github.com/iewnfod) |
| Ye Tianze | [@AndyMaster007](https://github.com/AndyMaster007) |

---

## Game Description

TermPoker is a Poker Card game that runs in your terminal. It does not require a GUI so you can enjoy it anytime, anywhere. The game rules are adapted from the basic mechanics of "Guan Dan," removing the team-based and level-up systems to focus on the core card-playing and comparison logic. 

Every game consists of **4 players**：1 human and 3 robots.The game uses **2 sets of cards**（total 108 cards）equally split among the 4 players.

**Card Type（From smallest to largest）：**

| Card Type      | Description                                |
|----------------|--------------------------------------------|
| Single         | Any single card                            |
| Double         | Two cards with same rank                   |
| Triple         | Three cards with same rank                 |
| Straight       | 5 cards with consecutive ranks excluding 2 |
| Three Plus Two | Triple + Double                            |
| Double Triple  | Two groups of consecutive triples          |
| Triple Pair    | Three groups of consecutive doubles        |
| Bomb           | 4 or more cards with same rank             |

**Card rank comparison：** 3 < 4 < 5 < 6 < 7 < 8 < 9 < 10 < J < Q < K < A < 2 < Small Joker < Large Joker

---

## Features & Coding Elements

### Feature 1: Complete Poker Card Object Model

Classes such as `PokerCard`, `Deck`, and `Player` are used to abstractly model cards, decks, and players.

- **Coding Element 1 (Class & Encapsulation):** The `PokerCard` class encapsulates the card rank (`POKER_CARD_VALUE`) and suit (`POKER_CARD_TYPE`) as private members, exposing them only through public methods (`getValue()`, `getValueString()`, etc.), embodying the principle of data encapsulation.

### Feature 2: Multi-Difficulty AI Robot Opponents

Three difficulty levels are provided: Easy, Medium, and Hard. AI behavior is implemented by an independent `Robot` class.

- **Coding Element 2 (Composition & Layered Design):** The `Robot` and `Player` classes collaborate through composition—`Player::autoPlay()` internally creates a `Robot` instance and then calls its `generatePlans()` / `getBestPlan()` methods. For different difficulty levels, `evaluatePlan()` adopts different probability estimation strategies (`estimateProb()`), demonstrating multi-layer object collaboration and encapsulation.

- **Coding Element 3 (Function Overloading):** The `Robot` class overloads the `hasConsecutive(startIdx, len)` and `hasConsecutive(startIdx, len, size)` methods to handle two query scenarios: "at least 1 card" and "at least `size` cards", respectively.

### Feature 3: Persistent Storage of Game History

After each round, the timestamp, difficulty, initial hand, and game result are automatically written to disk in JSON format, and can be viewed from the main menu.

- **Coding Element 4 (File I/O):** `Game::saveCurrentGame()` uses `std::ofstream` to serialize a `GameRecord` and write it to `~/.termpoker/history/<timestamp>.json`; `Game::showHistory()` uses `std::ifstream` to read and parse all history files, iterating and displaying them.

- **Coding Element 1 (Class & Encapsulation):** The `GameRecord` struct and its `to_json` / `from_json` helper functions centralize serialization logic within the `Game` class, hiding storage details from the outside.

### Feature 4: Rich Playable Card Type Detection and Comparison

The `CardUtils` class supports detecting all legal card types (single, pair, triple, bomb, three-with-two, straight,钢板, sister pairs, etc.) and provides cross-type comparison logic.

- **Coding Element 5 (STL Containers & Algorithms):** `CardUtils::getPlayCardType()` and `compareCards()` make extensive use of `std::vector` and `std::map` to store card ranks and mappings; `std::sort` (via `CardUtils::sortCards()`) sorts hands; `std::find` is used in the `Player` class to locate selected cards.

- **Coding Element 3 (Function Overloading):** The static method `getThreePlusXMainValueIndex()` and other detection functions are unified under the `CardUtils` class, provided as static member functions for reusability.

### Feature 5: Keyboard-Interactive Terminal UI

Players can move the cursor with arrow keys, select cards with the space bar, and play cards with the Enter key. The main menu also supports keyboard navigation, with a warning when the terminal width is insufficient.

- **Coding Element 2 (Composition & Event Callbacks):** The `Player` class holds multiple `std::function` callbacks (`handlePlayCards`, `handleQuit`, `getLastPlayedCards`, etc.), which are injected at runtime by `Deck::bindToPlayer()`, achieving loosely coupled inter-component communication.

- **Coding Element 5 (STL Function Objects):** All event callbacks use `std::function` and lambda expressions, representing a practical application of the STL function object mechanism.

---

## Non-Standard C/C++ Libraries

| Library | Version | Integration Method | Supported Features |
|---------|---------|--------------------|--------------------|
| [JSON for Modern C++](https://json.nlohmann.me) (nlohmann/json) | Single-header `json.hpp` | Placed directly into `ext-lib/` directory, compiled with source code | Serialization & deserialization of game history (save / read `.json` files); conversion between `GameRecord`, `GameDifficulty`, `PokerCard` objects and JSON |


---

## Build & Run

### Prerequisites

- A C++ compiler supporting C++11 or later (GCC 7+, Clang 5+ recommended)
- GNU Make **or** CMake 3.10+

### Using Make (Recommended)


```shell
# 1. Clone the repository
git clone https://github.com/iewnfod/TermPoker.git
cd TermPoker

# 2. Build
make

# 3. Run
bin/termpoker

# 4. Clean build artifacts (optional)
make clean
```

### Using CMake

```shell
# 1. Clone the repository
git clone https://github.com/iewnfod/TermPoker.git
cd TermPoker

# 2. Configure and build
cmake -S . -B cmake-build
cmake --build cmake-build

# 3. Run
cmake-build/termpoker
```

### Game Controls

| Key       | Function                                |
|-----------|-----------------------------------------|
| `↑` / `↓` | Move cursor between menu items          |
| `⬅` / `➡` | Move cursor between cards in the game   |
| `Enter`   | Confirm selection / Play cards          |
| `Space`   | Select / Deselect the current card      |
| `p`       | Quick jump to "Start Game"              |
| `c`       | Quick jump to "View History"            |
| `r`       | Quick jump to "Game Rules"              |
| `a`       | Quick jump to "About"                   |
| `q`       | Exit Game                               |

> **Tip：** For the best display experience, it is recommended to set your terminal width to **190 columns or more**（using`xterm-256color` environment）

---

## Technologies Used

* C++11
* CMake
* Make
* [JSON for Modern C++](https://json.nlohmann.me)

---

## Open Source License

[MPL-2.0](./LICENSE)
