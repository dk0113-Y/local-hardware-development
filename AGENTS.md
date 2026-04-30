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

## Safety rules

- Do not push directly to `main`.
- Work on a new branch for each task.
- Open a pull request when finished.
- Keep each pull request focused on one issue.
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

- which command failed;
- the exact error;
- what the user should install or configure next.

## Testing expectations

Before finishing a task:

- Run relevant correctness tests when code behavior changes.
- Run the benchmark when performance-related code changes.
- If tests or benchmarks cannot be run, explain why.
- Do not claim that tests passed unless they were actually run.

## Pull request expectations

Every PR should include:

1. What changed.
2. Why it changed.
3. Files changed.
4. Commands run.
5. Test results.
6. Known risks or follow-up work.

Use Chinese for the user-facing summary.

## Review guidelines

When reviewing a PR, check:

- Does the change solve the issue?
- Is the change unnecessarily large?
- Does it modify unrelated files?
- Are tests updated or added when behavior changes?
- Are generated files or build outputs accidentally committed?
- Are benchmark results meaningful and reproducible?
- Does the change keep the project beginner-friendly and easy to continue?

Treat accidental commits of build outputs, binaries, secrets, or unrelated large files as serious review issues.
