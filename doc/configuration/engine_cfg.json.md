# engine_cfg.json —— 运行策略配置

`doe_engine` 可执行文件的**启动配置**。`doe_engine` 启动时读取**与可执行文件同一目录**下的 `engine_cfg.json`（构建时由 CMake 从 `resources/engine_cfg.json` 拷贝到可执行文件旁）。该文件**必须存在且合法**，否则进程报错并退出。

它只描述**运行策略**（怎么做），**不参与 `config_hash`**——科学定义由 `experiment.json` 决定。

> 当前实现：已完成加载与严格校验（缺失文件、缺字段、类型/枚举/范围错误均报错退出）。各字段的实际运行策略接入为后续实现。

## 顶层结构

```json
{
  "persistence": { ... },
  "filesystem": { ... },
  "logging": { ... },
  "scheduler": { ... },
  "limits": { ... }
}
```

五个组**全部必填**，任何一组缺失都会报错。

## persistence

| 字段 | 类型 | 必填 | 允许值 | 说明 |
|---|---|---|---|---|
| `db_path` | string | 是 | 任意字符串 | SQLite 数据库路径。**空串 `""`** = 默认 `{experiment_dir}/doe_engine.db`（`experiment.json` 所在目录）；可填绝对路径（如 `"/data/runs.db"`）或相对路径。 |
| `batch_size` | integer | 是 | > 0 | 每次事务落库的批次大小：攒满该数量的设计点记录，提交一次事务。 |
| `artifact_strategy` | string | 是 | `none` / `keep_required` / `keep_all` / `tar` | 设计点**全部文件**（输入模板、中间文件、产物输出）的处置策略，见下。 |
| `tar_batch_size` | integer | 是 | > 0 | `artifact_strategy` 为 `tar` 时，每个压缩包包含的设计点数。 |

### artifact_strategy 语义

`artifact_strategy` 统一决定设计点目录及其内文件的去留（不再单独区分"过程文件"与"产物"）：

- `none`：运行完该点即清理其全部文件，**只保留数据库**；
- `keep_required`：只保留 `required: true` 的输出文件，删除其余；
- `keep_all`：保留全部文件；
- `tar`：每当**完成 `tar_batch_size` 个设计点**，就把这若干个设计点目录**打包成一个压缩包并删除原目录**。文件系统上的目录数因此从 N 降到 N/tar_batch_size，从根本上缓解共享文件系统上大量小目录的元数据压力。

## filesystem

| 字段 | 类型 | 必填 | 允许值 | 说明 |
|---|---|---|---|---|
| `working_directory` | string | 是 | `shared` / `scratch` | 设计点工作目录位置：`shared` = 共享文件系统（`{run_dir}/DESIGN_POINT_*`）；`scratch` = 节点本地 scratch。 |
| `scratch_dir` | string | 是 | 任意字符串 | `working_directory` 为 `scratch` 时的本地目录，**此时必填非空**（校验强制）；否则可为空串。 |

## logging

| 字段 | 类型 | 必填 | 允许值 | 说明 |
|---|---|---|---|---|
| `level` | string | 是 | `info` / `warn` / `error` | 日志级别：`info` 全量；`warn` 只输出 warn/error；`error` 只输出 error。 |
| `to_stdout` | boolean | 是 | `true` / `false` | 是否同时把日志输出到标准输出。 |
| `file` | string | 是 | 任意字符串 | 日志文件路径模板，支持 `{run_dir}` 占位符，例如默认 `"{run_dir}/engine.log"`。 |

## scheduler

| 字段 | 类型 | 必填 | 允许值 | 说明 |
|---|---|---|---|---|
| `mode` | string | 是 | `static` / `dynamic` | 调度方式：`static` = 静态连续分块（每个 rank 预先领一段）；`dynamic` = 动态领块（主从、算完一块领下一块，负载均衡）。 |
| `chunk_size` | integer | 是 | >= 0 | `dynamic` 模式下一个块包含的设计点数；`0` = 自动决定。 |

## limits

| 字段 | 类型 | 必填 | 允许值 | 说明 |
|---|---|---|---|---|
| `max_design_points` | integer | 是 | >= 0 | 设计点总数上限（防止全因子指数级爆炸）；`0` = 不限。 |

## 校验规则

- 五个顶层组全部必填，且每个字段都必填（不接受缺省默认值）。
- 正整数字段（`batch_size`/`tar_batch_size`）必须 > 0。
- 枚举字段拼写必须精确匹配：`artifact_strategy`、`working_directory`、`level`、`mode`。
- 交叉约束：`working_directory == "scratch"` 时 `scratch_dir` 非空。
- 违反任一规则 → `ConfigError`，进程报错退出。
