---
name: Codex Task
about: Create a focused implementation task for Codex
title: "[Codex Task] "
labels: codex-ready
assignees: ""
---

## Background

<!-- Explain the current situation. What problem are we solving? -->

## Goal

<!-- Describe the desired result clearly. -->

## Scope

<!-- What files, modules, or behaviors should Codex focus on? -->

## Non-goals

<!-- What should Codex avoid changing? -->

## Suggested files or search directions

<!-- Examples:
- include/aihw/
- src/
- tests/
- benchmarks/
- docs/

Or write: Let Codex search the repository first.
-->

## Acceptance criteria

- [ ]
- [ ]
- [ ]

## Testing requirements

Codex should run the relevant commands before finishing:

- `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `.\build\bench_matmul.exe`

If these commands cannot be run, Codex must explain why.

## Codex instruction

@codex please implement this issue with minimal, focused changes.

Follow `AGENTS.md`.

Requirements:

- This is a personal learning repository using direct-main delivery by default.
- Work on `main` by default.
- Commit completed changes directly on `main`.
- Push completed changes to `origin/main` by default.
- Do not create a temporary branch or pull request unless the user explicitly asks.
- Before committing or pushing, run `git status` and inspect the staged file list.
- Do not commit generated build outputs, binaries, caches, temporary files, local machine-specific files, secrets, credentials, tokens, or private information.
- If the working tree contains suspicious files, stop and ask the user before committing.
- Include a Chinese summary of changed files, commands run, test results, push status, and remaining risks.
