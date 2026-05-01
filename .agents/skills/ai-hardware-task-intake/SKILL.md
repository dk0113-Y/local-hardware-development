---
name: ai-hardware-task-intake
description: Use this repository-level skill when Codex receives a ChatGPT or GPT Project task brief, GitHub Issue, Codex Task, or natural-language request and should first analyze scope, boundaries, risks, and execution plan before writing code; trigger on requests such as receive this Codex task, analyze this task first, check task boundaries, start from this GPT task brief, or evaluate this GitHub Issue.
---

# AI Hardware Task Intake

Use this skill to decide whether a task is clear, small, safe, and ready for implementation. The default behavior is analysis only.

## Workflow

1. Read `AGENTS.md`.
2. Read the provided Issue, task brief, Codex Task, or user request.
3. If needed, read:
   - `README.md`
   - `docs/phase1.md`
   - `CMakeLists.txt`
4. Note that the default delivery mode is direct-main: Codex should work on `main`, commit to `main`, and push to `origin/main` unless the user explicitly asks for a branch, worktree, or PR.
5. Decide whether the task is small enough for one focused direct-main commit.
6. If the task is too large, recommend a split.
7. If the task is high risk, broad in scope, or likely to break the main line, recommend a temporary branch or PR, but do not create one unless the user agrees.
8. If the task is unclear, ask concise clarifying questions before editing files.
9. Do not modify files unless the user explicitly switches to implementation.

## Intake Report

Output in Chinese and include:

- 任务理解。
- 预计修改范围。
- 不应修改的内容。
- 预计涉及文件。
- 风险点。
- 测试计划。
- direct-main 是否合适；如果不合适，说明为什么建议临时分支或 PR。
- 是否建议进入实现阶段。

Keep the report concrete enough that it can become a GitHub Issue or Codex implementation instruction.
