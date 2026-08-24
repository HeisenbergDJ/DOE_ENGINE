# experiment.json —— 实验入口

DOE Engine 的运行入口文件，描述**一次实验的科学定义**。它不直接包含数据，而是通过 `file` 引用其它配置文件；被引用文件相对于 `experiment.json` 所在目录解析。

实验身份由本文件及全部引用文件共同决定（`config_hash`）。

## 顶层结构

```json
{
  "format": "doe_experiment",
  "format_version": 1,
  "id": "cfqs_extcur_q_full_factorial",
  "name": "CFQS coil-current scan for Q",
  "description": "Evaluate the relationship between nine VMEC external-coil currents and the Q response.",
  "variables":  { "file": "variables.json" },
  "responses":  { "file": "responses.json" },
  "sampling":   { "file": "sampling.json" },
  "workflow":   { "file": "workflow.json" },
  "extractors": { "file": "extractors.json" }
}
```

## 字段

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `format` | string | 是 | 固定为 `"doe_experiment"`。 |
| `format_version` | integer | 是 | 格式版本号，当前为 `1`。 |
| `id` | string | 是 | 实验唯一标识。 |
| `name` | string | 是 | 实验名称。 |
| `description` | string | 是 | 实验描述。 |
| `variables` | object | 是 | 引用 `variables.json`，格式 `{"file": "..."}`。 |
| `responses` | object | 是 | 引用 `responses.json`。 |
| `sampling` | object | 是 | 引用 `sampling.json`。 |
| `workflow` | object | 是 | 引用 `workflow.json`。 |
| `extractors` | object | 是 | 引用 `extractors.json`。 |

## file 引用

`file` 引用统一为对象：

```json
{ "file": "variables.json" }
```

- 相对路径：相对于**包含该引用的 JSON 文件**所在目录解析（不是进程当前目录）。
- 也可写绝对路径。
- 最终会解析为绝对路径并入 `config_hash`。

## 相关文档

- [variables.json](variables.json.md) · [responses.json](responses.json.md) · [sampling.json](sampling.json.md) · [workflow.json](workflow.json.md) · [extractors.json](extractors.json.md)
