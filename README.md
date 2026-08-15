# DOE Engine

## 1. 项目简介

（待补充）

## 2. 环境配置与编译构建

### 通用要求

- 支持 C++17 的编译器（GCC/Clang/MSVC 均可）
- CMake >= 3.20
- 构建工具（Ninja 或 make）
- 仓库已内置 `third_party/nlohmann/json.hpp`，JSON 依赖无需单独安装

### 2.1 macOS

1. 安装 Xcode Command Line Tools：

   ```sh
   xcode-select --install
   ```

2. 安装 CMake 与 Ninja（需先安装 Homebrew）：

   ```sh
   brew install cmake ninja
   ```

3. 配置并构建：

   ```sh
   cmake -S . -B build -G Ninja
   cmake --build build
   ```

如需 Debug 构建：`cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`

### 2.2 Linux

以 Debian/Ubuntu 系为例：

1. 安装编译工具链：

   ```sh
   sudo apt install build-essential cmake ninja-build
   ```

2. 配置并构建：

   ```sh
   cmake -S . -B build -G Ninja
   cmake --build build
   ```

如需 Debug 构建：`cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug`

### 2.3 Windows

方案 A：MSYS2 + MinGW64（推荐，仓库当前使用该工具链）

1. 安装 MSYS2（<https://www.msys2.org>）。
2. 在 MinGW64 终端安装工具链：

   ```sh
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-gdb
   ```

3. 将 `C:\msys64\mingw64\bin` 加入系统 PATH。
4. 配置并构建：

   ```sh
   cmake -S . -B build -G Ninja
   cmake --build build
   ```

方案 B：Visual Studio + MSVC

1. 安装 Visual Studio 2019/2022，勾选「使用 C++ 的桌面开发」。
2. 配置并构建（MSVC 为多配置生成器）：

   ```sh
   cmake -S . -B build
   cmake --build build --config Debug
   ```

### 2.4 构建产物

- macOS/Linux：`build/doe_engine`
- Windows：`build/doe_engine.exe`

## 3. 快速开始

（待补充）
