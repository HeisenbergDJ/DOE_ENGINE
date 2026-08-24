# extractors.json —— Extractor 实例与端口绑定

定义哪些 Extractor 实例消费哪些 Application 输出，并把提取结果绑定到哪些响应。每个实例通过 `configuration.file` 引用一个 Extractor 定义文件（见 [extractor_definition.md](extractor_definition.md)）。

## 顶层结构

```json
{
  "extractors": [
    {
      "id": "extract_q",
      "configuration": { "file": "extractors/q.json" },
      "inputs": {
        "wout": { "application": "run_vmecpp", "output": "wout_file" }
      },
      "outputs": {
        "q": { "response": "q" }
      }
    }
  ]
}
```

## 字段

| 字段 | 类型 | 必填 | 说明 |
|---|---|---|---|
| `id` | string | 是 | 实例唯一标识。 |
| `configuration` | object | 是 | 引用 Extractor 定义文件，格式 `{"file": "..."}`。 |
| `inputs` | object | 是 | 输入端口绑定：键 = 定义中声明的输入端口名，值 = 绑定到某个 Application 实例的输出端口。 |
| `outputs` | object | 是 | 输出端口绑定：键 = 定义中声明的输出端口名，值 = 绑定到某个响应。 |

## inputs 绑定

```json
"inputs": {
  "source_file": { "application": "run_model", "output": "result_file" }
}
```

- 键：Extractor 定义 `inputs` 中声明的端口名。
- `application`：`workflow.json` 中 Application 实例的 `id`。
- `output`：该实例 `outputs` 中声明的 `port`。
- 实际文件路径在运行时由 workflow 的输出解析得到，传给 Extractor。

## outputs 绑定

```json
"outputs": {
  "value": { "response": "y1" }
}
```

- 键：Extractor 定义 `outputs` 中声明的端口名。
- `response`：`responses.json` 中声明的响应 `id`。

## 校验规则

- 绑定键必须与 Extractor 定义声明的端口名精确匹配；
- 绑定必须能解析到已声明的 Application 实例/输出端口/响应；
- 提取出的响应 `id` 必须在 `responses.json` 中已声明。

## 相关文档

- 被 [experiment.json](experiment.json.md) 引用
- 引用 [extractor_definition.md](extractor_definition.md)
- 引用 [workflow.json](workflow.json.md)（`application`）与 [responses.json](responses.json.md)（`response`）
