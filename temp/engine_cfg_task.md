# DOE Engine — 任务交接文档（engine_cfg / 运行策略）

> 目的：供后续会话继续开发。记录「引擎运行策略配置」任务的已完成部分、schema、待办与关键设计约束。

## 目标功能（整体愿景）

本阶段要交付的 DOE Engine 能力，按功能分组：

1. **配置化运行策略（engine_cfg.json）**
   - 独立的引擎启动配置文件（与可执行文件同级，必填、严格校验、缺失/非法即退出）；
   - 覆盖：持久化（db_path、batch_size、artifact_strategy/tar）、文件系统（working_directory/scratch）、日志（level/to_stdout/file）、调度（mode/chunk_size）、限制（max_design_points）；
   - 只描述"怎么做"，不参与 `config_hash`（科学定义由 `experiment.json` 决定）。

2. **数据库持久化**
   - SQLite 三张表：`runs`（含 config_hash）、`design_points`（逐点状态+响应）、`application_runs`；
   - 方案 B 懒插入：点完成时单次写入，不预插；`INSERT OR REPLACE` 支持重跑/续算；
   - MPI 下 rank 0 单写者：非 root 通过 `SendBatch` 流式上抛记录，rank 0 `DrainAvailable/DrainAll` 按 `batch_size` 事务批量落库。

3. **断点续算（resume）**
   - `--resume <run_id>` 复用 run_id，查询 `TerminalPoints` 跳过已完成（SUCCESS/FAILED）点，只补跑剩余；
   - config_hash 校验：配置漂移则拒绝续算（保证科学一致性）。

4. **MPI 并行（动态负载均衡）**
   - `DOE_ENABLE_MPI` 开关，跨平台（Linux/macOS/Windows，OpenMPI/MPICH/MS-MPI 等实现）；
   - 设计点按 index 流式枚举（`Sampler::Size/At`），不物化全矩阵；
   - **目标**：从静态连续分块升级为**动态领块**（主从，rank 0 专职派发+收口落库，workers 算完一块领下一块，`chunk_size` 粒度）——见第 6 节。

5. **大规模设计点的文件系统压力治理**
   - 通过 `artifact_strategy: tar`：每完成 `tar_batch_size` 个设计点，把其目录打包成压缩文件并删除原目录，把目录数从 N 降到 N/tar_batch_size；
   - 解决共享并行文件系统（Lustre/GPFS）上百万小目录的元数据压力 / inode 上限 / 锁竞争。

6. **可观测性（日志）**
   - 每 run 生成 `{run_dir}/engine.log`：时间戳 + rank + 级别（INFO/WARN/ERROR）；
   - 逐设计点进度（`design point i/N: ok|failed`）、失败原因、总耗时；`tail -f` 可实时观察。

完成标准：`doe_engine experiment.json` 与 `mpiexec -np N doe_engine experiment.json` 均可用，策略（batch_size、tar、scratch、日志级别、上限）生效，resume 可续算，日志可观测。

## 1. 当前状态

- 引擎运行策略配置文件：`resources/engine_cfg.json`（源码目录），构建时由 `copy_test_fixtures` 拷贝到 `build/engine_cfg.json`（与可执行文件同级）。
- `doe_engine` 启动时从**可执行文件所在目录**加载 `engine_cfg.json`（`main.cc` 用 `ExecutableDirectory(argv[0])` 推导）；文件缺失或校验不合法 → 报错退出非零。
- 目前**只做加载与严格校验**，各字段尚未接入 runtime（main 校验后丢弃返回值）。

## 2. 涉及的文件

- `src/config/config_types.h` — 引擎策略枚举：
  `ArtifactStrategy`(none/keep_required/keep_all/tar)、`WorkingDirectoryMode`(shared/scratch)、
  `LogLevel`(info/warn/error)、`SchedulerMode`(static/dynamic)
  （`DesignPointLayout` 已删除）
- `src/config/engine_config.h` — `EngineConfig` + `PersistenceConfig/FileSystemConfig/LoggingConfig/SchedulerConfig/LimitsConfig`
- `src/config/config.h` — 伞头已包含 `engine_config.h`
- `src/utils/json/parser.h/.cc` — `Parser::ParseEngineConfig(file)`（严格校验）；parser.h 的 guard 已修正为 `DOE_ENGINE_UTILS_JSON_PARSER_H_`
- `src/main.cc` — 启动加载 + `ExecutableDirectory(argv[0])`
- `CMakeLists.txt` — `copy_test_fixtures` 拷贝 `resources/engine_cfg.json` → `build/engine_cfg.json`
- 配置文档：`doc/configuration/`（engine_cfg.json.md 及各科学定义配置文档）；README 3.3 只留入口

## 3. engine_cfg.json 当前 schema

```json
{
  "persistence": {
    "db_path": "",                       // 空 = {experiment_dir}/doe_engine.db
    "batch_size": 256,                   // >0
    "artifact_strategy": "keep_all",     // none|keep_required|keep_all|tar
    "tar_batch_size": 10000              // >0
  },
  "filesystem": {
    "working_directory": "shared",       // shared|scratch
    "scratch_dir": ""                    // working_directory==scratch 时必填
  },
  "logging": {
    "level": "info",                     // info|warn|error
    "to_stdout": false,
    "file": "{run_dir}/engine.log"
  },
  "scheduler": {
    "mode": "static",                    // static|dynamic
    "chunk_size": 0                      // 0=自动
  },
  "limits": {
    "max_design_points": 0               // 0=不限
  }
}
```

已移除：`keep_transient_files`、`design_point_layout`、`shard_width`。
`artifact_strategy` 现统一决定设计点**全部文件**（输入模板/中间文件/产物输出）的处置：
`none` 只留库；`keep_required` 只留 required 输出；`keep_all` 全留；`tar` 每完成 `tar_batch_size` 个点打包并删原目录（目录数 N → N/tar_batch_size，根治共享 FS 元数据压力）。

校验规则（已生效）：五组全必填；`batch_size`/`tar_batch_size` >0；枚举拼写精确；`scratch` 模式 `scratch_dir` 非空。

## 4. 待办（按建议顺序）

1. **runtime 接线**：把 `EngineConfig` 从 main 传入 `Experiment`/`Scheduler`/`Persistence`（遵循"config 只用于构造"），使以下字段生效：
   - `logging.level/to_stdout/file` → `doe::log::Logger` 支持级别过滤、stdout 开关、文件模板展开；
   - `persistence.batch_size` → 替代写死的 `kRecordBatchSize = 256`（experiment.cc:39，注意：该提交来自另一台电脑的 `adac5dc`，当前 HEAD 已包含它）；
   - `limits.max_design_points` → `Experiment::Run` 里对 `sampler_->Size()` 设上限；
   - `persistence.db_path` → main 构造 `SqlitePersistence` 用配置路径；
   - `artifact_strategy` → 设计点目录/产物按策略清理或 `tar` 打包（tar 打包由 rank 0 协调：攒够 N 点 → 打包 → 删原目录）。
2. **filesystem**：`working_directory=scratch` 时工作目录用 `scratch_dir`（节点本地）。
3. **scheduler dynamic**：主从动态领块（rank 0 专职派发+落库，workers 算完一块领下一块，`chunk_size` 粒度）——需要接口调整 `Run(point_count, run_point, persist)`，见第 6 节。

## 5. 关键设计约束（勿破坏）

- **config_hash 只代表科学定义**：engine_cfg 不进 `experiment.json`、不参与 `config_hash`；否则改策略会被误判为"配置漂移"拒绝 resume。
- **resume 已实现**：`--resume <run_id>` 复用 run_id，查 `TerminalPoints` 跳过 SUCCESS/FAILED 点；config_hash 不匹配拒绝续算；`RecordPointOutcome`/`RecordExperimentStart` 用 `INSERT OR REPLACE`。
- **MPI 单写者**：仅 rank 0 持有 `Persistence`，非 root 用 `SendBatch` 把点记录发给 rank 0，rank 0 `DrainAvailable/DrainAll` 批量落库（`BeginBatch/EndBatch/AbortBatch` 事务）。
- **方案 B 懒插入**：点记录只在完成时单次写入，不预插。
- **静态 vs 动态调度**：当前 `MpiScheduler::Run` 是静态连续分块（每 rank 一大块），rank 0 也算点；动态待实现。

## 6. 动态调度（待实现）要点

- 接口：`PointScheduler::Run(point_count, run_point, persist)`，把编排收进调度器（现 `SendBatch/DrainAvailable/DrainAll` 由 experiment.cc 外置调用，将移为内部细节）。
- 主从协议消息：`WORK_REQUEST`（worker→rank0）、`WORK_CHUNK`（rank0→worker，含下一块起始或 END）、`RECORDS_BATCH`（worker→rank0）、`WORKER_DONE`。
- rank 0 专职：`MPI_Probe(ANY_SOURCE)` 分派 + 收到批次即 `persist`；不跑点。
- worker：请求块 → 跑 chunk 内点（`run_point` 攒记录）→ 满批发批次 → 请求下一块；结束后 `WORKER_DONE`。
- 保留串行与 static 模式；`chunk_size` 由配置决定（负载均衡）。

## 7. 其它背景

- MPI 开关 `DOE_ENABLE_MPI`（CMake option）；不开则串行。
- 持久化层：SQLite vendor 在 `third_party/sqlite`（需用户放置 sqlite3.c/h，amalgamation 3.53.4）。
- 运行日志：`{run_dir}/engine.log`（`doe::log::Logger`，时间戳+rank+级别，`std::endl` 即时 flush）。
- 目录结构：`src/config`（纯数据）、`src/utils/json`（Parser）、`src/utils/log`（error/logger）、`src/persistence`（接口+SqlitePersistence）、`src/runtime/{data,application,extractor,sampling,workflow,experiment,scheduler}`。
