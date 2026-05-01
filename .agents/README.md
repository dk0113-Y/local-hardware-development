# Repository Codex Skills

This directory stores repository-level Codex Skills for `local-hardware-development`.

These skills support the current collaboration workflow:

1. The user discusses requirements, task breakdowns, and designs with ChatGPT or a GPT Project.
2. ChatGPT generates Codex instructions, GitHub Issues, Codex Tasks, or review notes.
3. Codex works on `main` by default, modifies files, runs relevant checks, commits, and pushes to `origin/main`.
4. GitHub Issues act as task handoff records.
5. Pull requests, temporary branches, or worktrees are used only when the user explicitly asks.
6. The user and ChatGPT review Codex results after execution.

## Skills

- `ai-hardware-project-onboarding`: Use when starting a new Codex thread, taking over the repository, restoring project context, or understanding the project without modifying code.
- `ai-hardware-task-intake`: Use when receiving a ChatGPT or GPT Project task brief, GitHub Issue, or natural-language request and first analyzing scope, boundaries, risks, and plan.
- `ai-hardware-task-implementation`: Use when a task is clear and the user asks Codex to make focused, verifiable code or documentation changes.
- `ai-hardware-result-review`: Use when reviewing a Codex result, main-branch commit, diff, test output, benchmark output, or optional PR.
- `ai-hardware-benchmark-reporting`: Use when organizing, comparing, and explaining benchmark results for CPU matmul, tiled matmul, SIMD, multithreading, GPU/NPU, or other experiments.

## AGENTS.md And Skills

- `AGENTS.md` defines long-term repository rules, safety boundaries, project structure, build/test commands, direct-main delivery policy, and review standards.
- Skills define task-specific operating procedures so Codex can choose the right workflow for each situation.

## Direct-main Default Workflow

- This is a personal learning repository.
- The default delivery mode is direct-main.
- Codex works on `main` by default.
- Codex commits completed changes on `main`.
- Codex pushes completed changes to `origin/main` by default.
- Codex does not create `codex/*` temporary branches by default.
- Codex does not create pull requests by default.
- Only use a branch, worktree, or pull request when the user explicitly asks for it.
- Before every commit or push, run `git status` and inspect the staged file list.
- Never commit build outputs, binaries, caches, temporary files, local machine-specific files, secrets, tokens, or private information.

GPT Project instructions should know these skill names. For example, a task can explicitly say: "Use `ai-hardware-task-intake` to analyze this Issue first" or "Use `ai-hardware-task-implementation` to execute this task."
