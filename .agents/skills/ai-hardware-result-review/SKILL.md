---
name: ai-hardware-result-review
description: Use this repository-level skill to review Codex or another agent's PR, diff, patch, implementation summary, test output, or benchmark output for local-hardware-development; trigger on requests such as review Codex results, check this PR, inspect this diff, check test output, or decide whether this implementation can merge.
---

# AI Hardware Result Review

Use this skill for review. Do not modify code unless the user explicitly asks to enter a fix or implementation phase.

## Workflow

1. Read `AGENTS.md`.
2. If PR, Issue, diff, test output, or benchmark output is provided, review it before drawing conclusions.
3. Check whether the change:
   - Solves the original goal.
   - Stays on topic.
   - Is unnecessarily large.
   - Modifies unrelated files.
   - Breaks existing behavior.
   - Lacks required tests.
   - Commits build outputs, binaries, caches, or temporary files.
   - Leaks local paths, secrets, tokens, or private information.
   - Actually ran the claimed tests.
   - Includes benchmark data for performance-related changes.
   - Interprets benchmark results cautiously and reproducibly.

## Review Output

Output in Chinese and include:

- 总体判断：可合并 / 需要修改 / 不建议合并。
- 主要问题。
- 次要问题。
- 建议的后续 Codex 指令。

Prioritize concrete file and line references when reviewing code. Treat accidental build artifacts, binaries, secrets, unrelated large files, or unsupported benchmark claims as serious issues.
