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

Codex should run the relevant commands before finishing.

Suggested baseline commands:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bench_matmul.exe
If these commands cannot be run, Codex must explain why.

Codex instruction

@codex please implement this issue with minimal, focused changes.

Follow AGENTS.md.

Requirements:

Do not push directly to main.
Work on a separate branch.
Open a pull request when finished.
Do not commit generated build outputs or binaries.
Keep the PR focused on this issue.
Include a Chinese summary of changed files, commands run, test results, and remaining risks.
