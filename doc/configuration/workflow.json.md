# workflow.json —— Application 流程

定义一次实验中 Application 实例的组合与执行顺序。当前首版为**串行顺序执行**（按 `applications` 数组声明顺序），连接（connections）仅记录、暂不生效。

## 顶层结构

```json
{
  "applications": [
    { "id": "run_model", "configuration": { "file": "applications/add_dif.json" } }
  ],
  "connections": []
}
```

## 字段

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `applications` | array | 是 | Application 实例列表（可为空）。 |
| `connections` | array | 是 | 连接列表（首版须为空数组 `[]`）。 |

## 单个 Application 实例

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `id` | string | 是 | 实例唯一标识；`extractors.json` 的输入绑定用它引用该实例的输出。 |
| `configuration` | object | 是 | 引用 Application 定义文件，格式 `{"file": "..."}`。 |

## connections（连接）

每个连接：

```json
{
  "from": { "application": "app_a", "port": "out_p" },
  "to":   { "application": "app_b", "port": "in_p" }
}
```

`from`/`to` 各含 `application`（实例 id）与 `port`（逻辑端口名）。当前模型 Application 输入尚无端口字段，连接**记录但不应用**，请使用空数组。

## 相关文档

- 被 [experiment.json](experiment.json.md) 引用
- 引用 [application.md](application.md)（Application 定义文件）
- 被 [extractors.json](extractors.json.md) 引用（`application` 绑定）
