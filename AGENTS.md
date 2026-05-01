# AGENTS.md

## Repository purpose

This repository is `local-hardware-development`.

It is a local hardware-aware AI/system development lab focused on:

- C/C++ CPU performance experiments.
- Matrix multiplication and other AI primitive kernels.
- Benchmarking and correctness tests.
- Later exploration of SIMD, multithreading, Intel Arc GPU compute, and Intel NPU/OpenVINO deployment.

The current phase starts with a minimal C++ CPU matrix multiplication baseline.

## User context

The user is learning Git, GitHub, Codex, CMake, and low-level AI/hardware development.

When summarizing work for the user:

- Use Chinese.
- Explain technical terms briefly when they are likely to be unfamiliar.
- Prefer step-by-step explanations.
- Be explicit about commands that were run or should be run.

## Direct-main delivery policy

- This is a personal learning repository.
- The default delivery mode is direct-main.
- Codex should work on `main` by default.
- Codex should commit completed changes directly on `main`.
- Codex should push completed changes to `origin/main` by default.
- Codex should not create temporary task branches unless the user explicitly asks.
- Codex should not create pull requests unless the user explicitly asks.
- Only use a branch, worktree, or pull request when the user explicitly requests it.
- Before committing or pushing, run `git status` and inspect the staged file list.
- If the working tree contains suspicious files, stop and ask the user before committing.
- If tests are relevant, run the appropriate tests before committing.
- Do not claim tests passed unless they were actually run.

## Safety rules

- Do not rewrite Git history unless the user explicitly asks.
- Do not delete files unless the issue explicitly asks for deletion.
- Do not modify secrets, credentials, tokens, private keys, or local machine-specific configuration.
- Do not commit generated build outputs, binaries, cache files, or temporary files.
- Do not modify files under build directories such as `build/`, `build-native/`, `out/`, or `cmake-build-*`.

## Coding rules

- Prefer small, reviewable changes.
- Do not perform broad refactors unless the issue explicitly asks for them.
- Keep public APIs simple and documented.
- Use C++17 unless the project is intentionally upgraded.
- Follow the existing project structure:
  - Public headers go under `include/aihw/`.
  - Implementations go under `src/`.
  - Correctness tests go under `tests/`.
  - Benchmarks go under `benchmarks/`.
  - Design notes and experiment notes go under `docs/`.

## Build and test commands

Use the repository root as the working directory.

Preferred clean build commands:

- `cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `.\build\bench_matmul.exe`

If the local environment uses a different build directory, such as `build-native/`, explain that in the final summary.

If a required tool is missing, do not silently skip the step. Report:

- Which command failed.
- The exact error.
- What the user should install or configure next.

## Testing expectations

Before finishing a task:

- Run relevant correctness tests when code behavior changes.
- Run the benchmark when performance-related code changes.
- If tests or benchmarks cannot be run, explain why.
- Do not claim that tests passed unless they were actually run.

## Direct-main summary expectations

Every completed change should include:

1. What changed.
2. Why it changed.
3. Files changed.
4. Commands run.
5. Test results.
6. Known risks or follow-up work.
7. Commit hash and push status.

Only create a pull request if the user explicitly asks for one.

Use Chinese for the user-facing summary.

## Review guidelines

When reviewing a commit, diff, or pull request, check:

- Does the change solve the issue?
- Is the change unnecessarily large?
- Does it modify unrelated files?
- Are tests updated or added when behavior changes?
- Are generated files or build outputs accidentally committed?
- Were binaries, caches, secrets, credentials, tokens, or local machine-specific files accidentally committed?
- Are benchmark results meaningful and reproducible?
- Does the change keep the project beginner-friendly and easy to continue?
- Should a bad `main` commit be reverted or followed by a fix commit?

Treat accidental commits of build outputs, binaries, secrets, or unrelated large files as serious review issues.
