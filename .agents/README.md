# Repository Codex Skills

这个目录存放 `local-hardware-development` 的仓库级 Codex Skills。

这些 skills 服务于当前协作工作流：

1. 用户先和 ChatGPT / GPT Project 讨论需求、拆任务、设计方案。
2. ChatGPT 生成 Codex 指令、GitHub Issue / Codex Task 或 review 意见。
3. Codex 在仓库中执行代码修改、运行测试、提交修改、创建 PR。
4. GitHub Issue / Pull Request 作为任务和结果的中转站。
5. Codex 执行结果再由用户和 ChatGPT review。

## Skills

- `ai-hardware-project-onboarding`：用于新开 Codex thread、接手仓库、恢复项目上下文，或只要求理解项目不修改代码时。
- `ai-hardware-task-intake`：用于接收 ChatGPT / GPT Project 生成的任务单、GitHub Issue 或自然语言需求后，先做任务理解、边界检查、风险检查和执行计划。
- `ai-hardware-task-implementation`：用于任务已经明确并且用户要求执行时，按仓库规则完成小而可验证的代码或文档修改。
- `ai-hardware-result-review`：用于 review Codex 或其他 agent 产生的 PR、diff、修改摘要、测试输出、benchmark 输出。
- `ai-hardware-benchmark-reporting`：用于整理、比较和解释 benchmark 结果，尤其是 CPU matmul、tiled matmul、SIMD、多线程、GPU/NPU 等实验数据。

## AGENTS.md 与 Skills 的分工

- `AGENTS.md` 是仓库长期规则，定义安全边界、项目结构、构建测试命令、PR 要求和 review 标准。
- Skills 是特定任务的操作流程，帮助 Codex 在不同场景下选择正确工作模式。

GPT Project 应该知道这些 skill 名称。生成 Codex 指令时，可以显式写出要使用的 skill，例如“使用 `ai-hardware-task-intake` 先分析这个 Issue”或“使用 `ai-hardware-task-implementation` 执行这个任务”。
