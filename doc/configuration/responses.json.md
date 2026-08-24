# responses.json —— 响应定义

定义 DOE 的响应变量（输出）——由 Extractor 提取后填入设计点的结构化标量。

## 顶层结构

```json
{
  "responses": [ { ... }, { ... } ]
}
```

`responses` 为数组，**必须存在**，可含 0 个或多个响应。

## 单个响应

```json
{
  "id": "q",
  "name": "Q scalar",
  "description": "Scalar Q metric computed from the VMEC wout NetCDF file.",
  "unit": "1",
  "value_type": "real"
}
```

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `id` | string | 是 | 响应唯一标识，被 `extractors.json` 的输出绑定引用。 |
| `name` | string | 是 | 响应名。 |
| `description` | string | 是 | 描述。 |
| `unit` | string | 是 | 单位（可为空串）。 |
| `value_type` | string | 是 | 取值类型：`real` / `integer` / `string`。 |

## 校验规则

- `responses` 数组必填；每条 `id/name/description/unit/value_type` 全必填。
- `value_type` 仅 `real` / `integer` / `string`。
- Extractor 产出的响应 `id` 必须在本文件中已声明，否则运行时报错。

## 相关文档

- 被 [experiment.json](experiment.json.md) 引用
- 被 [extractors.json](extractors.json.md) 的输出绑定引用（按 `id`）
