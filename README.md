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

TermPoker is a Poker Card game that runs in your terminal.  是一款运行在终端（命令行）中的扑克牌游戏，无需图形界面，随时随地可以畅玩。游戏规则改编自「掼蛋」的基础玩法，去掉了组队与升级机制，专注于核心的出牌与比较逻辑。

每局游戏共有 **4 位玩家**：1 名真人玩家和 3 名 AI 机器人。游戏使用 **2 副牌**（共 108 张）平均分发，先出完手牌者获胜。

**牌型（从小到大）：**

| 牌型 | 说明 |
|------|------|
| 单张 | 任意单张牌 |
| 对子 | 两张相同点数的牌 |
| 三张 | 三张相同点数的牌 |
| 顺子 | 5 张连续点数的牌（不含 2）|
| 三带二 | 三张相同 + 一对 |
| 钢板 | 两组连续的三张（6 张）|
| 姐妹对 | 三对连续点数的对子（6 张）|
| 炸弹 | 4 / 5 / 6 张相同点数的牌，大炸弹优先 |

**牌面大小：** 3 < 4 < 5 < 6 < 7 < 8 < 9 < 10 < J < Q < K < A < 2 < 小王 < 大王

---

## 功能列表及编码要素说明

### 功能一：完整的扑克牌对象模型

使用 `PokerCard`、`Deck`、`Player` 等类对扑克牌、牌组、玩家进行抽象建模。

- **编码要素 1（类与封装）**：`PokerCard` 类将牌面点数（`POKER_CARD_VALUE`）与花色（`POKER_CARD_TYPE`）封装为私有成员，仅通过公有方法（`getValue()`、`getValueString()` 等）对外暴露，体现了数据封装原则。

### 功能二：多难度 AI 机器人对手

提供简单、中等、困难三个难度，AI 行为由独立的 `Robot` 类实现。

- **编码要素 2（组合与分层设计）**：`Robot` 与 `Player` 通过组合方式协作——`Player::autoPlay()` 内部创建 `Robot` 实例，再调用其 `generatePlans()` / `getBestPlan()` 方法。不同难度下 `evaluatePlan()` 采用不同的概率估算策略（`estimateProb()`），体现了多层对象协作与封装。
- **编码要素 3（函数重载）**：`Robot` 类中 `hasConsecutive(startIdx, len)` 与 `hasConsecutive(startIdx, len, size)` 是对同名函数的重载，分别处理"至少有 1 张"和"至少有 size 张"两种查询场景。

### 功能三：游戏历史记录的持久化存储

每局结束后自动将时间戳、难度、初始手牌、胜负结果以 JSON 格式写入磁盘，并可在主菜单中查看历史。

- **编码要素 4（文件 I/O）**：`Game::saveCurrentGame()` 使用 `std::ofstream` 将 `GameRecord` 序列化后写入 `~/.termpoker/history/<时间戳>.json`；`Game::showHistory()` 使用 `std::ifstream` 读取并解析所有历史文件，遍历输出。
- **编码要素 1（类与封装）**：`GameRecord` 结构体及其 `to_json` / `from_json` 辅助函数将序列化逻辑集中在 `Game` 类内部，外部无需感知存储细节。

### 功能四：丰富的出牌类型检测与比较

`CardUtils` 类支持检测单张、对子、三张、炸弹、三带二、顺子、钢板、姐妹对等所有合法牌型，并提供跨牌型的大小比较。

- **编码要素 5（STL 容器与算法）**：`CardUtils::getPlayCardType()` 和 `compareCards()` 大量使用 `std::vector`、`std::map` 存储牌面及映射关系；`std::sort`（通过 `CardUtils::sortCards()`）对手牌排序；`std::find` 在 `Player` 类中查找已选牌。
- **编码要素 3（函数重载）**：静态方法 `getThreePlusXMainValueIndex()` 与其他检测函数统一在 `CardUtils` 类下，以静态成员函数形式提供复用。

### 功能五：键盘交互的终端 UI

玩家可以通过方向键移动光标、空格键选牌、回车键出牌，主菜单同样支持键盘导航，并在终端宽度不足时给出警告提示。

- **编码要素 2（组合与事件回调）**：`Player` 持有多个 `std::function` 回调（`handlePlayCards`、`handleQuit`、`getLastPlayedCards` 等），由 `Deck::bindToPlayer()` 在运行时注入，实现了松耦合的组件间通信。
- **编码要素 5（STL 函数对象）**：所有事件回调均使用 `std::function` 与 lambda 表达式，属于 STL 函数对象机制的实际应用。

---

## 非标准 C/C++ 库

| 库名称 | 版本 | 集成方式 | 支持的功能 |
|--------|------|----------|------------|
| [JSON for Modern C++](https://json.nlohmann.me)（nlohmann/json） | 单头文件 `json.hpp` | 直接放入 `ext-lib/` 目录，随源码一起编译 | 游戏历史记录的序列化与反序列化（保存 / 读取 `.json` 文件）；`GameRecord`、`GameDifficulty`、`PokerCard` 对象与 JSON 的互相转换 |

---

## 编译与运行

### 前置条件

- 支持 C++11 或更高标准的 C++ 编译器（推荐 `g++ 7+` 或 `clang++ 5+`）
- GNU Make **或** CMake 3.10+

### 使用 Make（推荐）

```shell
# 1. 克隆仓库
git clone https://github.com/iewnfod/TermPoker.git
cd TermPoker

# 2. 构建
make

# 3. 运行
bin/termpoker

# 4. 清理构建产物（可选）
make clean
```

### 使用 CMake

```shell
# 1. 克隆仓库
git clone https://github.com/iewnfod/TermPoker.git
cd TermPoker

# 2. 配置与构建
cmake -S . -B cmake-build
cmake --build cmake-build

# 3. 运行
cmake-build/termpoker
```

### 游戏操作说明

| 按键        | 功能            |
|-----------|---------------|
| `↑` / `↓` | 在菜单间移动光标      |
| `⬅` / `➡` | 在游戏中间移动光标     |
| `Enter`   | 确认选择 / 出牌     |
| `Space`   | 选中 / 取消选中当前手牌 |
| `p`       | 快速跳转到「开始游戏」   |
| `c`       | 快速跳转到「查看历史」   |
| `r`       | 快速跳转到「游戏规则」   |
| `a`       | 快速跳转到「关于」     |
| `q`       | 退出游戏          |

> **提示：** 建议将终端宽度设置为 **190 列以上**（`xterm-256color` 环境）以获得最佳显示效果。

---

## 使用的技术

* C++11
* CMake
* Make
* [JSON for Modern C++](https://json.nlohmann.me)

---

## 开源许可

[MPL-2.0](./LICENSE)
