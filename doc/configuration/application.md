# Application 定义（applications/*.json）

`workflow.json` 中每个 Application 实例通过 `configuration.file` 引用一个 Application 定义文件（文件名任意，例如 `applications/vmecpp.json`）。它描述一个可执行计算步骤：如何准备输入、运行哪个程序、检查哪些输出。

## 顶层结构

```json
{
  "type": "command",
  "executable": "{experiment_dir}/resources/add_solver",
  "working_directory": "{design_point_dir}",
  "arguments": ["{design_point_dir}/my.in"],
  "timeout_seconds": 0,
  "success_exit_codes": [0],
  "inputs": [ { ... }, { ... } ],
  "outputs": [ { ... } ]
}
```

## 字段

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `type` | string | 是 | 仅 `command`。 |
| `executable` | string | 是 | 可执行文件路径或命令名，支持路径模板（见下）。Windows 上可省略 `.exe`（运行时会自动补）。 |
| `working_directory` | string | 是 | 进程工作目录，支持路径模板。 |
| `arguments` | array | 是 | 命令行参数数组，**逐条传参、不拼 shell**；每条支持路径模板。 |
| `timeout_seconds` | integer | 是 | 超时秒数；`0` = 不限时。 |
| `success_exit_codes` | array | 是 | 视为成功的退出码列表，通常 `[0]`。 |
| `inputs` | array | 是 | 输入准备列表（可为空）。 |
| `outputs` | array | 是 | 输出声明列表（可为空）。 |

## 路径模板

`executable` / `working_directory` / `arguments` / 输入输出的路径都支持占位符：

| 占位符 | 含义 |
|---|---|
| `{experiment_dir}` | `experiment.json` 所在目录 |
| `{run_dir}` | 本次运行目录 `{experiment_dir}/runs/<run_id>` |
| `{design_point_dir}` | 当前设计点目录 `{run_dir}/DESIGN_POINT_<index>` |

未知或未闭合的占位符会报错。路径可含空格。

## inputs（输入准备）

每个输入项：

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `mode` | string | 是 | `copy`（普通复制）或 `tagged_copy`（模板替换）。 |
| `source` | string | 是 | 源文件路径（路径模板）。 |
| `destination` | string | 是 | 目标文件路径（路径模板）。 |
| `tagger` | object | `tagged_copy` 必填 | 模板替换器，`copy` 禁止出现，见下。 |

### tagger（模板替换器）

**placeholder_text（占位符替换）**

```json
{
  "type": "placeholder_text",
  "tags": [
    { "placeholder": "{{X1}}", "variable": "x1" },
    { "placeholder": "{{X2}}", "variable": "x2" }
  ]
}
```

把模板中所有 `placeholder` 替换为该 `variable`（引用 `variables.json` 的 id）的取值。

**delimited_text（按分隔符行列定位替换）**

```json
{
  "type": "delimited_text",
  "index_base": 0,
  "delimiters": [" ", "\t", "="],
  "value_format": { "notation": "scientific", "precision": 12 },
  "tags": [
    { "row": 0, "column": 1, "variable": "x1" }
  ]
}
```

- `index_base`：行列索引的基数（`0` 或 `1`），同时作用于行与列。
- `delimiters`：分隔符列表，连续分隔符折叠。
- `value_format.notation`：`scientific` / `fixed` / `general`；`precision`：小数位数。
- `tags`：每个 tag 把第 `row` 行、第 `column` 列（按分隔符拆分后的 token）替换为对应 `variable` 的格式化值。

## outputs（输出声明）

每个输出项：

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `port` | string | 是 | 逻辑输出端口名，被 `extractors.json` 的输入绑定引用。 |
| `type` | string | 是 | 产物类型，仅 `file`。 |
| `path` | string | 是 | 产物文件路径（路径模板）。 |
| `required` | boolean | 是 | 是否必须存在；`true` 时运行结束后检查，缺失则报错。 |

## 相关文档

- 被 [workflow.json](workflow.json.md) 引用
- 引用 [variables.json](variables.json.md)（tagger 的 `variable`）
- 运行期使用 [engine_cfg.json](engine_cfg.json.md) 的策略
