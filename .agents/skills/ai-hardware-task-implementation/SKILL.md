---
name: ai-hardware-task-implementation
description: Use this repository-level skill when a local-hardware-development task is already clear and the user explicitly asks Codex to execute, implement a Codex task, modify code or docs, implement an Issue, start implementation, or make focused repository changes with tests and an implementation summary.
---

# AI Hardware Task Implementation

Use this skill to make small, reviewable changes after the task is clear.

## Default Direct-main Flow

1. Confirm the current branch is `main`.
2. Read and follow `AGENTS.md`.
3. Understand the task. If the task comes from a GitHub Issue, read the Issue content first.
4. Modify files with focused changes.
5. Run necessary checks or tests.
6. Run `git status` and inspect the modification scope.
7. Run `git add` only for relevant files.
8. Run `git commit`.
9. Run `git push origin main`.
10. Summarize changed files, test results, push status, and risks in Chinese.

Only create a temporary branch, worktree, or PR if the user explicitly asks.

## Edit Rules

- Do not do unrelated refactors.
- Do not commit generated build outputs, binaries, caches, temporary files, or local machine-specific files.
- Do not modify files under `build/`, `build-native/`, `out/`, or `cmake-build-*`.
- Do not include `.exe`, `.obj`, `.a`, or similar build artifacts.
- Do not commit secrets, credentials, tokens, private keys, or privacy-sensitive information.
- Before committing or pushing, inspect `git status` and the staged file list.
- If the working tree contains suspicious files, stop and ask the user before committing.
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
- commit hash。
- push 到 `origin/main` 的状态。
- 剩余风险。
- 是否需要后续修复提交。
