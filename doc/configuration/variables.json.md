# variables.json —— 设计变量定义

定义 DOE 设计变量（输入）及其取值范围。

## 顶层结构

```json
{
  "variables": [ { ... }, { ... } ]
}
```

`variables` 为数组，**必须存在**，可含 0 个或多个变量。

## 单个变量

```json
{
  "id": "extcur_1",
  "name": "EXTCUR(1)",
  "description": "VMEC external-coil current 1.",
  "unit": "A",
  "value_type": "real",
  "domain": {
    "type": "continuous",
    "lower_bound": -312500.0,
    "upper_bound": -156250.0
  }
}
```

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `id` | string | 是 | 变量唯一标识，被 `sampling.json` 的 factor、tagger 的 tag、workflow/extractor 引用。 |
| `name` | string | 是 | 变量名。 |
| `description` | string | 是 | 描述。 |
| `unit` | string | 是 | 单位（可为空串）。 |
| `value_type` | string | 是 | 取值类型：`real` / `integer` / `string`。 |
| `domain` | object | 是 | 取值范围，见下。 |

## domain（取值范围）

`domain` 的 `type` 决定具体结构：

**continuous（连续）** —— 采样在该区间内取点：

```json
{ "type": "continuous", "lower_bound": -312500.0, "upper_bound": -156250.0 }
```

**discrete（离散）** —— 显式列出允许的值集合：

```json
{ "type": "discrete", "values": [2, 4, 6] }
```

`values` 元素可为 JSON 整数、浮点数或字符串（分别对应 `integer`/`real`/`string` 取值），例如 `["北京", "上海", "成都"]`。

## 校验规则

- `variables` 数组必填；每个变量 `id/name/description/unit/value_type/domain` 全必填。
- `value_type` 仅 `real` / `integer` / `string`。
- `domain.type` 仅 `continuous` / `discrete`；continuous 需要 `lower_bound`/`upper_bound`（数字），discrete 需要非空 `values`。

## 相关文档

- 被 [experiment.json](experiment.json.md) 引用
- 被 [sampling.json](sampling.json.md) 的 factor 引用（按 `id`）
- 被 [application.md](application.md) 的 tagger 引用（按 `id`）
