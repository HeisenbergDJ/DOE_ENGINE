# DOE Engine 配置说明

本目录描述 DOE Engine 的所有 JSON 配置文件：字段含义、允许取值、必填项与校验规则。

配置分两类：

1. **运行策略**：`engine_cfg.json` —— `doe_engine` 可执行文件的启动配置，描述"怎么做"，不参与 `config_hash`。
2. **科学定义**：`experiment.json` 及其引用的各配置文件，描述"做什么"，决定 `config_hash`。

## 文件引用关系

```text
engine_cfg.json                 doe_engine 启动配置（与可执行文件同级）
        │
experiment.json                 ← 运行入口
        ├── variables.json      设计变量定义
        ├── responses.json      响应定义
        ├── sampling.json       采样策略
        ├── workflow.json       Application 实例与连接
        │       └── applications/*.json    Application 定义（文件名任意）
        └── extractors.json     Extractor 实例与端口绑定
                └── extractors/*.json      Extractor 定义（文件名任意）
```

## 文档列表

| 配置文件 | 文档 | 说明 |
|---|---|---|
| `engine_cfg.json` | [engine_cfg.json.md](engine_cfg.json.md) | 运行策略（启动必填） |
| `experiment.json` | [experiment.json.md](experiment.json.md) | 实验入口与引用 |
| `variables.json` | [variables.json.md](variables.json.md) | 设计变量与取值范围 |
| `responses.json` | [responses.json.md](responses.json.md) | 响应定义 |
| `sampling.json` | [sampling.json.md](sampling.json.md) | 采样策略（全因子） |
| `workflow.json` | [workflow.json.md](workflow.json.md) | Application 实例与连接 |
| `applications/*.json` | [application.md](application.md) | Application 定义（含 tagger） |
| `extractors.json` | [extractors.json.md](extractors.json.md) | Extractor 实例与端口绑定 |
| `extractors/*.json` | [extractor_definition.md](extractor_definition.md) | Extractor 定义（delimited_text / python_script） |
