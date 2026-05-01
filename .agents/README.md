# Repository Codex Skills

这个目录存放 `local-hardware-development` 的仓库级 Codex Skills。

这些 skills 服务于当前协作工作流：

1. 用户先和 ChatGPT / GPT Project 讨论需求、拆任务、设计方案。
2. ChatGPT 生成 Codex 指令、GitHub Issue / Codex Task 或 review 意见。
3. Codex 默认在 `main` 上执行代码修改、运行测试、提交修改，并 push 到 `origin/main`。
4. GitHub Issue 作为任务中转站；只有用户明确要求时才创建临时分支或 Pull Request。
5. Codex 执行结果再由用户和 ChatGPT review。

## Skills

- `ai-hardware-project-onboarding`：用于新开 Codex thread、接手仓库、恢复项目上下文，或只要求理解项目不修改代码时。
- `ai-hardware-task-intake`：用于接收 ChatGPT / GPT Project 生成的任务单、GitHub Issue 或自然语言需求后，先做任务理解、边界检查、风险检查和执行计划。
- `ai-hardware-task-implementation`：用于任务已经明确并且用户要求执行时，按仓库规则完成小而可验证的代码或文档修改。
- `ai-hardware-result-review`：用于 review Codex 或其他 agent 产生的 PR、diff、修改摘要、测试输出、benchmark 输出。
- `ai-hardware-benchmark-reporting`：用于整理、比较和解释 benchmark 结果，尤其是 CPU matmul、tiled matmul、SIMD、多线程、GPU/NPU 等实验数据。

## AGENTS.md 与 Skills 的分工

- `AGENTS.md` 是仓库长期规则，定义安全边界、项目结构、构建测试命令、direct-main 交付要求和 review 标准。
- Skills 是特定任务的操作流程，帮助 Codex 在不同场景下选择正确工作模式。

## Direct-main 默认工作流

- 这是个人学习仓库，默认交付方式是 direct-main。
- Codex 默认在 `main` 上工作，完成后 commit 并 push 到 `origin/main`。
- Codex 默认不创建 `codex/*` 临时分支，也默认不创建 PR。
- 只有用户明确要求“使用分支”“使用 Worktree”“创建 PR”时，才使用临时分支、worktree 或 PR。
- 每次 commit 或 push 前必须运行 `git status`，检查 staged 文件列表和修改范围。
- 禁止提交 build 输出、二进制、缓存、临时文件、本地机器专属文件、密钥、token 或隐私信息。

GPT Project 应该知道这些 skill 名称。生成 Codex 指令时，可以显式写出要使用的 skill，例如“使用 `ai-hardware-task-intake` 先分析这个 Issue”或“使用 `ai-hardware-task-implementation` 执行这个任务”。
