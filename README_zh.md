<p align="center">
    <h1 align="center">TermPoker</h1>
    <p align="center">在终端里随时扑克！</p>
    <p align="center">
        <a href="./README_zh.md">简体中文</a> | <a href="./README.md">English</a>
    </p>
</p>

> 本项目使用了一些 AI 工具来帮助设计与优化，但这并不是一个 Vibe-Coding 的项目。

## 构建与运行
### Make
1. 克隆当前仓库
```shell
git clone https://github.com/iewnfod/TermPoker.git
```
2. 使用 `make` 进行构建
```shell
make
```
3. 运行可执行文件
```shell
bin/termpoker
```
4. 清理构建
```shell
make clean
```

### CMake
```shell
cmake -S . -B cmake-build
```

## 使用的技术
* C++11
* CMake
* Make
* [JSON for Modern C++](https://json.nlohmann.me)

## 开源许可
[MPL-2.0](./LICENSE)

## 贡献者
* [Iewnfod](https://github.com/iewnfod)
* [Ye Tianze](https://github.com/AndyMaster007)
