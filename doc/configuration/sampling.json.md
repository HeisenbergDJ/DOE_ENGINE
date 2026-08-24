# sampling.json —— 采样策略

定义如何从设计变量生成设计点。当前支持等距全因子采样。

## 顶层结构

```json
{
  "type": "full_factorial",
  "factors": [
    { "variable": "extcur_1", "levels": 2 },
    { "variable": "extcur_2", "levels": 2 }
  ]
}
```

## 字段

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `type` | string | 是 | 采样类型，当前仅 `full_factorial`。 |
| `factors` | array | 是 | 因子列表（可为空，空 = 单个空设计点）。 |

## 单个 factor

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `variable` | string | 是 | 引用 [variables.json](variables.json.md) 中某个变量的 `id`。 |
| `levels` | integer | 是 | 水平数，**必须 >= 2**。 |

## 行为

- **连续变量**：在 `[lower_bound, upper_bound]` 之间等距取 `levels` 个水平，**必含两端**。例如 levels=3 时取 `lower, (lower+upper)/2, upper`。
- **离散变量**：直接使用 `domain.values` 作为该因子的水平，忽略 `levels` 数值。
- 设计点总数为各因子水平数的乘积（全因子笛卡尔积）。

## 校验规则

- `type` 仅 `full_factorial`。
- `factors` 数组必填；每个 factor 的 `variable` 必须已声明，`levels >= 2`。
- 设计点总数可能极大（全因子指数级），可用 `engine_cfg.json` 的 `limits.max_design_points` 设上限。

## 相关文档

- 被 [experiment.json](experiment.json.md) 引用
- 引用 [variables.json](variables.json.md)
