---
name: ai-hardware-project-onboarding
description: Use this repository-level skill when starting or resuming work in local-hardware-development, onboarding a new Codex thread, understanding the project before changes, checking the current project structure, or when the user says to read/start the project, understand the project first, do onboarding, or not modify code and only summarize.
---

# AI Hardware Project Onboarding

Use this skill to rebuild project context without changing files.

## Workflow

1. Read these files from the repository root:
   - `README.md`
   - `AGENTS.md`
   - `NEW_CHAT_PROMPT.md`
   - `docs/phase1.md`
   - `CMakeLists.txt`
2. Inspect the top-level directory structure.
3. Do not modify files.
4. Output in Chinese.

## Summary Requirements

Include:

- 项目目标。
- 当前技术阶段。
- 当前硬件 / 本地开发背景。
- 目录结构。
- 构建命令。
- 测试命令。
- benchmark 命令。
- 当前适合推进的下一步。
- 需要用户确认的问题。

If any document mentions an old path, old project name, or outdated information, point it out clearly.

Do not fix outdated information unless the user explicitly asks.
