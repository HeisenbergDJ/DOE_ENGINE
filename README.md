# DOE Engine

## 1. 项目简介

（待补充）

## 2. 环境配置与编译构建

### 通用要求

- 支持 C++17 的编译器（GCC/Clang/MSVC 均可）
- CMake >= 3.20
- 构建工具（Ninja 或 make）
- 仓库已内置 `third_party/nlohmann/json.hpp`，JSON 依赖无需单独安装
- SQLite 走 vendor 单体版（`third_party/sqlite/`，见其中的 README）：从 <https://www.sqlite.org/download.html> 下载 amalgamation，把 `sqlite3.c`/`sqlite3.h` 放入该目录

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

### 2.4 MPI 并行（可选）

默认关闭（串行模式，无需 MPI）。开启后支持本机多核或集群多进程并行计算。通用编译开关为 `-DDOE_ENABLE_MPI=ON`，运行一律通过 `mpiexec`/`mpirun` 启动。

#### 2.4.1 Linux

**环境检测**（确认装了哪种 MPI 实现）：

```sh
which mpicxx
mpirun --version      # 输出含 "Open MPI" / "MPICH" 等实现名
```

- OpenMPI：Ubuntu/Debian `sudo apt install openmpi-bin libopenmpi-dev`；RHEL/CentOS `sudo dnf install openmpi`（部分发行版需 `module load mpi/openmpi-x86_64` 后才能用）。
- MPICH：`sudo apt install mpich`。

**编译**（用该实现的 `mpicxx` 作为编译器，FindMPI 与链接都跟着它走）：

```sh
export PATH=<你的mpi>/bin:$PATH          # 例如 /usr/lib64/openmpi/bin 或集群的 /public/software/mpi/openmpi/<版本>/bin
cmake -S . -B build -DDOE_ENABLE_MPI=ON -DCMAKE_CXX_COMPILER=$(which mpicxx)
cmake --build build
```

配置输出里确认 `MPI CXX wrapper` 指向你想要的实现。

**运行**：

```sh
mpiexec -np 4 ./build/doe_engine build/tests/fixtures/delimited_text_experiment/experiment.json
```

#### 2.4.2 macOS

**环境检测**：

```sh
which mpicxx; mpirun --version
```

未装则安装（Homebrew）：`brew install openmpi`（或 `brew install mpich`）。

**编译**（同 Linux）：

```sh
cmake -S . -B build -DDOE_ENABLE_MPI=ON -DCMAKE_CXX_COMPILER=$(which mpicxx)
cmake --build build
```

**运行**：

```sh
mpiexec -np 4 ./build/doe_engine build/tests/fixtures/delimited_text_experiment/experiment.json
```

#### 2.4.3 Windows

**方案 A：MS-MPI（微软官方，推荐）**

1. 安装 MS-MPI（下载页含 SDK 与运行时），安装后 `mpiexec -version` 验证。
2. 编译（FindMPI 会自动探测 MS-MPI；若未找到，用 `-DMPI_CXX_INCLUDE_PATH`、`-DMPI_CXX_LIBRARIES` 手动指向 SDK）：

   ```sh
   cmake -S . -B build -DDOE_ENABLE_MPI=ON
   cmake --build build
   ```

3. 运行：

   ```sh
   mpiexec -np 4 build\doe_engine.exe build\tests\fixtures\delimited_text_experiment\experiment.json
   ```

**方案 B：MSYS2 + MinGW64 + OpenMPI**

1. MinGW64 终端安装：`pacman -S mingw-w64-x86_64-openmpi`。
2. 编译（把 mingw64 的 `mpicxx` 放 PATH，并以它作编译器）：

   ```sh
   export PATH=/c/msys64/mingw64/bin:$PATH
   cmake -S . -B build -DDOE_ENABLE_MPI=ON -DCMAKE_CXX_COMPILER=$(which mpicxx)
   cmake --build build
   ```

3. 运行：

   ```sh
   mpiexec -np 4 build/doe_engine.exe build/tests/fixtures/delimited_text_experiment/experiment.json
   ```

#### 注意事项

- 未用 `mpiexec`/`mpirun` 直接运行 MPI 构建产物，行为未定义，请始终通过启动器运行；
- 设计点按连续块分片到各 rank，每个 rank 只枚举并运行自己的片段（全因子按 index 流式生成，不在内存中物化整张设计矩阵）；
- 每个 rank 各自创建并写入自己的 `DESIGN_POINT_<index>` 目录；
- `run_id` 由 rank 0 生成并广播，运行摘要（成功数）在全部 rank 汇总；
- 进程内日志与进度在 `{run_dir}/engine.log`，带时间戳与 rank 号（见第 3 章）；
- 串行模式行为与关闭开关时一致，可作调试基线。

### 2.5 构建产物

- macOS/Linux：`build/doe_engine`
- Windows：`build/doe_engine.exe`

## 3. 快速开始

构建完成后（见第 2 章），运行引擎并传入一个实验配置文件的路径即可。

### 3.1 macOS 与 Linux

```sh
./build/doe_engine build/tests/fixtures/delimited_text_experiment/experiment.json
```

### 3.2 Windows

```sh
build\doe_engine.exe build\tests\fixtures\delimited_text_experiment\experiment.json
```

仓库自带三个可运行的实验 fixture：`delimited_text_experiment`、`multiple_extractors_experiment`、`placeholder_text_experiment`（位于 `build/tests/fixtures/`，其中的测试 CLI 程序由构建按当前平台自动编译生成），把上面的路径换成对应 `experiment.json` 即可。

引擎会把运行记录（`runs/`）写入 `experiment.json` 所在目录，并把每个设计点持久化到该目录下的 SQLite 数据库 `doe_engine.db`（`runs`/`design_points`/`application_runs` 三张表，含每次运行的 `config_hash` 指纹），同时把每个设计点响应值打印到标准输出。

每次运行还会在 `{run_dir}`（即 `runs/<run_id>/`）下生成 `engine.log`，逐行带时间戳、rank 号与级别（INFO/WARN/ERROR），包含每个设计点的进度（`design point i/N: ok|failed`）、失败原因、以及结束时的总耗时。MPI 多进程会带各自的 rank 前缀。超算上可用 `tail -f runs/<run_id>/engine.log` 实时查看进度，`grep -i error` 查看错误。

断点续算：`doe_engine <experiment.json> --resume <run_id>` 会复用该 `run_id`，跳过数据库中已完成（SUCCESS/FAILED）的设计点，只补跑剩余的点；若当前配置与数据库中该 run 记录的 `config_hash` 不一致则拒绝续算。
