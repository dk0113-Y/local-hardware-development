---
name: ai-hardware-task-implementation
description: Use this repository-level skill when a local-hardware-development task is already clear and the user explicitly asks Codex to execute, implement a Codex task, modify code or docs, implement an Issue, start implementation, or make focused repository changes with tests and an implementation summary.
---

# AI Hardware Task Implementation

Use this skill to make small, reviewable changes after the task is clear.

## Before Editing

1. Read and follow `AGENTS.md`.
2. If the task comes from a GitHub Issue, read the Issue content first.
3. Work on a separate branch or worktree. Do not push directly to `main`.
4. Keep changes focused on the task.

## Edit Rules

- Do not do unrelated refactors.
- Do not commit generated build outputs, binaries, caches, temporary files, or local machine-specific files.
- Do not modify files under `build/`, `build-native/`, `out/`, or `cmake-build-*`.
- Do not include `.exe`, `.obj`, `.a`, or similar build artifacts.
- Preserve beginner-friendly structure and explanations.

## Verification

When C++ behavior changes, run:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

For performance-related changes, also run:

```powershell
.\build\bench_matmul.exe
```

If a command cannot run, report the exact command, the exact error, and what the user should install or configure next. Do not claim tests passed unless they actually ran.

## Final Summary

Output in Chinese and include:

- 修改了哪些文件。
- 为什么这样改。
- 运行了哪些命令。
- 测试结果。
- benchmark 结果，如果相关。
- 剩余风险。
- 是否建议创建 PR。
