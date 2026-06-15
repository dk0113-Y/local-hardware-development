# local-hardware-development

## Project Overview / 项目简介

`local-hardware-development` is a small C++17 / CMake lab for learning
hardware-aware AI runtime development on a local Windows laptop.

The current repository focuses on a tiny CPU runtime prototype rather than a
full AI framework. It implements a simple `Tensor` container, basic neural
network operators, correctness tests, runnable examples, and configurable
benchmarks. The goal is to make low-level runtime ideas visible and testable:
data layout, operator boundaries, numerical checks, command-line benchmark
workloads, and CPU pinning experiments.

简而言之：这个项目不是“调包跑模型”，而是从 C++ 基础组件开始，搭建一个可构建、可测试、可 benchmark 的端侧 AI runtime 原型。

## Motivation / 项目动机

Embedded software, edge AI, robotics, and autonomous-driving simulation tools
often need engineers who can understand both software structure and hardware
constraints. Even before deploying to an MCU, NPU, GPU, or production device,
useful preparation includes:

- organizing C++ code into headers, source files, tests, examples, and
  benchmarks;
- understanding memory layout and CPU-friendly operator loops;
- validating numerical behavior with small deterministic tests;
- measuring runtime behavior with reproducible benchmark commands;
- documenting what is implemented, what is experimental, and what is not yet
  supported.

This repository is a learning and portfolio artifact for those foundations.

## What This Repository Is / 仓库定位

- A C++17 runtime prototype for tiny AI workloads.
- A CMake / Ninja based build and test project.
- A correctness-focused implementation of small tensor operators.
- A benchmark playground for CPU-side runtime workloads.
- A beginner-friendly engineering record for hardware-aware AI development.
- A foundation for future edge AI runtime exploration.

## What This Repository Is Not / 边界说明

- It is not a commercial embedded product.
- It is not an MCU, RTOS, CAN, vehicle-grade, or production deployment project.
- It does not currently include GPU or NPU kernels.
- It does not currently include OpenVINO, ONNX, CUDA, SYCL, OpenCL, or oneMKL
  integration.
- It does not train models, load external model files, or implement autograd.
- It is not a complete transformer or language-model runtime.

When this README mentions embedded software or edge AI, it means foundational
skills and runtime prototype exploration, not completed hardware deployment.

## Tech Stack / 技术栈

- Language: C++17.
- Build system: CMake 3.20+ with Ninja.
- Runtime code: small row-major `Tensor`, CPU operator implementations, and
  Windows CPU affinity helper.
- Tests: CTest running a C++ correctness executable.
- Benchmarks: standalone C++ executables with configurable command-line
  workload sizes.
- Platform currently used: Windows 11 on an Intel Core Ultra laptop.
- External runtime dependencies: none beyond the C++ standard library and the
  platform compiler toolchain.

## Repository Structure / 仓库结构

```text
.
|-- include/aihw/          Public C++ headers for the tiny runtime
|-- src/                   Runtime implementations
|-- tests/                 Correctness tests registered with CTest
|-- examples/              Small runnable demos
|-- benchmarks/            Benchmark executables and CLI workload parsing
|-- docs/                  Design notes and experiment explanations
|-- .github/               GitHub issue template for Codex tasks
|-- CMakeLists.txt         Build targets and test registration
|-- AGENTS.md              Repository workflow and safety rules
|-- NEW_CHAT_PROMPT.md     Local handoff prompt for future Codex sessions
`-- README.md              Portfolio-oriented project entry point
```

## Core Components / 核心组件

### Runtime Library: `aihw_runtime`

Defined in `CMakeLists.txt`, the `aihw_runtime` library is built from:

- `src/tensor.cpp`
- `src/ops.cpp`
- `src/cpu_affinity.cpp`

It exposes headers under `include/aihw/`.

### Tensor

`include/aihw/tensor.hpp` and `src/tensor.cpp` define a small row-major tensor
container.

Current support:

- 1D and 2D shapes;
- contiguous `std::vector<float>` storage;
- shape, size, row, and column accessors;
- bounds-checked element access through `operator[]` and `at2d`.

Current limitations:

- no strides, slices, views, broadcasting, quantization, or device memory;
- no ownership model for external buffers.

### Operators

`include/aihw/ops.hpp` and `src/ops.cpp` implement:

- `matmul`: 2D matrix multiplication with an internal contiguous IKJ loop;
- `relu`: elementwise ReLU activation;
- `add`: same-shape elementwise addition;
- `softmax`: 1D or row-wise 2D softmax;
- `layer_norm`: 1D or row-wise 2D normalization;
- `scaled_dot_product_attention`: single-head attention with optional causal
  masking;
- `transformer_block`: a tiny pre-norm transformer block made from the
  operators above.

### CPU Affinity Helper

`include/aihw/cpu_affinity.hpp` and `src/cpu_affinity.cpp` provide a Windows
thread pinning helper for benchmark experiments:

- inspect current processor group and logical processor;
- attempt to pin the current benchmark thread to a requested logical CPU;
- report success, before/after processor location, and error text.

This is an experiment aid, not a general scheduling recommendation.

### Tests

`tests/test_tiny_runtime.cpp` is registered as the CTest test
`tiny_runtime_correctness`. It checks:

- tensor shape and indexing behavior;
- matmul known outputs;
- ReLU, add, softmax, and layer norm properties;
- tiny classifier pipeline shape and probability sum;
- attention shape and causal behavior;
- transformer block shape and deterministic behavior;
- invalid-shape exception paths.

### Examples

The examples show how the runtime pieces are composed:

- `tiny_mlp`: `input -> matmul -> relu -> matmul`;
- `tiny_classifier`: `input -> matmul -> relu -> matmul -> layer_norm ->
  softmax`;
- `tiny_transformer_block`: fixed-size tiny transformer block smoke demo.

### Benchmarks

The benchmark targets are:

- `bench_tiny_runtime`: configurable tiny classifier workload;
- `bench_tiny_transformer`: configurable tiny transformer block workload.

Both benchmark executables print CSV-like sections and include optional
`--pin-cpu N` support on Windows.

## Build and Test / 构建与测试

Run commands from the repository root.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The build creates the runtime library plus test, example, and benchmark
executables under `build/`.

Useful executables after a successful build:

```powershell
.\build\test_tiny_runtime.exe
.\build\tiny_mlp.exe
.\build\tiny_classifier.exe
.\build\tiny_transformer_block.exe
.\build\bench_tiny_runtime.exe
.\build\bench_tiny_transformer.exe
```

Optional local CPU tuning can be enabled on non-MSVC compilers:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DAIHW_ENABLE_NATIVE=ON
```

`AIHW_ENABLE_NATIVE` enables local CPU tuning flags such as `-march=native`
where supported. It is off by default so the baseline build remains portable.

## Benchmarks and Examples / Benchmark 与示例

Run a minimal example:

```powershell
.\build\tiny_mlp.exe
```

Run a tiny classifier demo:

```powershell
.\build\tiny_classifier.exe
```

Run a transformer block demo:

```powershell
.\build\tiny_transformer_block.exe
```

Run the tiny runtime benchmark with default settings:

```powershell
.\build\bench_tiny_runtime.exe
```

Run a small configurable benchmark:

```powershell
.\build\bench_tiny_runtime.exe --batch 2 --input-dim 16 --hidden-dim 32 --output-dim 8 --iterations 3
```

Run the transformer benchmark:

```powershell
.\build\bench_tiny_transformer.exe --seq-len 8 --model-dim 16 --ff-dim 32 --iterations 3
```

Try Windows CPU pinning:

```powershell
.\build\bench_tiny_runtime.exe --pin-cpu 0
.\build\bench_tiny_transformer.exe --pin-cpu 0
```

The reported time is useful for local comparison under the same machine and
build settings. It should not be interpreted as a cross-machine performance
claim.

## Current Status and Limitations / 当前状态与限制

Implemented:

- C++17 runtime library with small tensor and operator APIs;
- CMake build targets for runtime, tests, examples, and benchmarks;
- correctness tests through CTest;
- tiny MLP, classifier, and transformer block examples;
- configurable benchmark workloads;
- Windows CPU pinning experiment hook.

Not implemented yet:

- SIMD, multithreading, tiled matmul, or backend selection;
- GPU, NPU, OpenVINO, ONNX, CUDA, SYCL, OpenCL, or oneMKL integration;
- MCU, RTOS, driver, CAN, or vehicle-grade deployment;
- model loading, training, autograd, tokenizer, embedding, KV cache, or
  autoregressive generation;
- production-level error handling, profiling infrastructure, or CI matrix.

## Internship Skill Mapping / 求职能力映射

This repository can support internship discussions around the following skills:

| Internship skill area | Evidence in this repository |
| --- | --- |
| C++ engineering organization | Public headers in `include/aihw/`, implementations in `src/`, standalone tests, examples, and benchmarks. |
| CMake / Ninja build workflow | `CMakeLists.txt` defines a library, executables, compiler warnings, C++17 standard settings, and CTest registration. |
| Unit and correctness testing | `tests/test_tiny_runtime.cpp` checks operator outputs, numerical properties, and exception paths. |
| Benchmark design | `benchmarks/bench_tiny_runtime.cpp` and `benchmarks/bench_tiny_transformer.cpp` expose configurable workload sizes and CSV-like output. |
| Tiny runtime prototype | `Tensor`, `matmul`, `relu`, `add`, `softmax`, `layer_norm`, attention, and transformer block form a minimal inference-style pipeline. |
| Edge AI runtime foundations | The project demonstrates how small AI operators can be composed without relying on a heavyweight framework. |
| Hardware-aware development | The matmul loop uses contiguous row-major access, and benchmarks can record Windows logical CPU pinning results. |
| Engineering documentation | `README.md`, `docs/tiny-runtime.md`, `AGENTS.md`, and `.github/ISSUE_TEMPLATE/codex-task.md` document scope, usage, and workflow. |

These are foundational capabilities for embedded software, edge AI, robotics
runtime components, simulation tooling, and AI toolchain internships. They
should be presented as learning-backed engineering evidence, not as proof of
completed embedded deployment.

## Development Environment / 本地开发环境

Local machine used for development:

- Laptop: Xiaomi Redmi Book Pro 14 2024
- CPU: Intel Core Ultra 5 125H, 14 cores / 18 threads
- GPU: Intel Arc integrated graphics
- NPU: Intel AI Boost
- Memory: 32 GB LPDDR5X
- Storage: 1 TB NVMe SSD
- OS: Windows 11

This hardware context explains why the repository is interested in CPU
benchmarking, future Intel GPU/NPU exploration, and edge AI runtime concepts.
The current code path, however, is CPU-only.
