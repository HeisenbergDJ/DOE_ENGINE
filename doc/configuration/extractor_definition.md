# Extractor 定义（extractors/*.json）

`extractors.json` 中每个实例通过 `configuration.file` 引用一个 Extractor 定义文件（文件名任意，例如 `extractors/q.json`、`extractors/y1.json`）。它描述**如何从文件提取响应值**。当前支持两种类型：`delimited_text` 与 `python_script`。

## 类型总览

- `delimited_text`：内置文本提取——按分隔符拆分文本，按行列定位单元并映射到输出端口（tagger 的逆过程，detag）。
- `python_script`：运行 Python 脚本，解析脚本产出的 JSON 结果文件。

## delimited_text

```json
{
  "type": "delimited_text",
  "inputs": [
    { "port": "source_file", "type": "file" }
  ],
  "index_base": 0,
  "delimiters": [" ", "\t", "="],
  "tags": [
    { "row": 0, "column": 1, "output": "value" }
  ],
  "outputs": [
    { "port": "value", "value_type": "real" }
  ]
}
```

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `type` | string | 是 | `delimited_text`。 |
| `inputs` | array | 是 | 输入端口声明；`port` 为端口名，`type` 仅 `file`。 |
| `index_base` | integer | 是 | 行列索引基数（`0` 或 `1`），同时作用于行与列。 |
| `delimiters` | array | 是 | 分隔符列表，连续分隔符折叠。 |
| `tags` | array | 是 | 每个 tag：`row`/`column` 定位单元，`output` 指向 `outputs` 中声明的端口。 |
| `outputs` | array | 是 | 输出端口声明：`port` 端口名、`value_type`（`real`/`integer`/`string`）。 |

行为：读取输入文件 → 按行拆分 → 对每个 tag 取第 `row` 行、第 `column` 列（按分隔符拆分）的文本单元 → 按 `value_type` 解析 → 绑定到该端口对应的响应。行/列越界、端口未绑定都会报错。

## python_script

```json
{
  "type": "python_script",
  "interpreter": "python3",
  "script": "{experiment_dir}/resources/extract_q.py",
  "working_directory": "{design_point_dir}",
  "inputs": [
    { "port": "wout", "type": "file" }
  ],
  "arguments": [
    "--wout", "{input:wout}",
    "--output", "{design_point_dir}/q_response.json"
  ],
  "timeout_seconds": 0,
  "success_exit_codes": [0],
  "result": {
    "file": "{design_point_dir}/q_response.json",
    "format": "json",
    "values": [
      { "key": "q", "output": "q", "value_type": "real" }
    ]
  }
}
```

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `type` | string | 是 | `python_script`。 |
| `interpreter` | string | 是 | 解释器命令名（如 `python3`）。 |
| `script` | string | 是 | 脚本路径（路径模板）。 |
| `working_directory` | string | 是 | 工作目录（路径模板）。 |
| `inputs` | array | 是 | 输入端口声明。 |
| `arguments` | array | 是 | 传给脚本的参数；支持 `{input:<port>}` 占位符替换为绑定文件路径，也支持 `{experiment_dir}` 等路径模板。 |
| `timeout_seconds` | integer | 是 | 超时秒数；`0` = 不限时。 |
| `success_exit_codes` | array | 是 | 成功退出码列表。 |
| `result` | object | 是 | 结果解析，见下。 |

### result（结果解析）

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `file` | string | 是 | 脚本产出的结果文件路径（路径模板）。 |
| `format` | string | 是 | 当前仅 `json`。 |
| `values` | array | 是 | 每个元素：`key`（结果 JSON 中的键名）、`output`（绑定到 `outputs` 声明的端口）、`value_type`（`real`/`integer`/`string`）。 |

行为：运行 `interpreter script args...` → 校验退出码 → 读取结果文件（JSON）→ 对每个 `values` 元素取 `result_json[key]` → 解析 → 绑定到端口对应的响应。

## 路径模板

与 [application.md](application.md) 相同的占位符：`{experiment_dir}`、`{run_dir}`、`{design_point_dir}`，外加 `{input:<port>}`（绑定输入文件路径）。

## 相关文档

- 被 [extractors.json](extractors.json.md) 引用
- 引用 [responses.json](responses.json.md)（输出绑定的 `response`）
