# local-hardware-development

## Project Overview

`local-hardware-development` is a small C++17 and CMake repository for
hardware-aware AI runtime learning.

The current project is a CPU-only tiny runtime prototype. It contains a small
`Tensor` container, several neural-network operators, correctness tests,
runnable examples, and configurable benchmark programs.

The purpose is to make low-level runtime ideas visible and testable:

- data layout;
- operator boundaries;
- numerical correctness checks;
- reproducible benchmark commands;
- CPU pinning experiments on Windows.

In Chinese terms, this is not a "call a framework and run a model" repository.
It is a small, buildable, testable C++ runtime prototype for learning the
foundations behind edge AI and hardware-aware software.

## Motivation

Embedded software, edge AI, robotics, autonomous-driving simulation, and AI
toolchain work often require engineers who can connect software structure with
hardware constraints.

Before deploying anything to an MCU, NPU, GPU, or production device, useful
preparation includes:

- organizing C++ code into headers, source files, tests, examples, and
  benchmarks;
- understanding row-major memory layout and CPU-friendly loop ordering;
- validating numerical behavior with deterministic tests;
- measuring runtime behavior with repeatable commands;
- documenting what is implemented and what is still out of scope.

This repository is a learning artifact for those foundations.

## What This Repository Is

This repository is:

- a C++17 runtime prototype for tiny AI workloads;
- a CMake and Ninja based build project;
- a correctness-focused implementation of small tensor operators;
- a benchmark playground for CPU-side runtime workloads;
- a beginner-friendly engineering record for hardware-aware AI development;
- a foundation for future edge AI runtime exploration.

## What This Repository Is Not

This repository is not:

- a commercial embedded product;
- an MCU, RTOS, CAN, vehicle-grade, or production deployment project;
- a GPU or NPU kernel project;
- an OpenVINO, ONNX, CUDA, SYCL, OpenCL, or oneMKL integration;
- a model training framework;
- an autograd system;
- a complete transformer runtime;
- a language-model inference engine.

When this README mentions embedded software or edge AI, it means foundational
skills and runtime prototype exploration. It does not mean completed hardware
deployment.

## Tech Stack

- Language: C++17.
- Build system: CMake 3.20+.
- Preferred generator: Ninja.
- Tests: CTest running a C++ correctness executable.
- Runtime storage: contiguous `std::vector<float>` tensor data.
- Runtime operators: CPU implementations in ordinary C++.
- Benchmarks: standalone C++ executables with command-line workload settings.
- Platform currently used: Windows 11 on an Intel Core Ultra laptop.
- Runtime dependencies: only the C++ standard library and compiler toolchain.

## Repository Structure

```text
.
|-- include/
|   `-- aihw/
|       |-- cpu_affinity.hpp
|       |-- ops.hpp
|       `-- tensor.hpp
|-- src/
|   |-- cpu_affinity.cpp
|   |-- ops.cpp
|   `-- tensor.cpp
|-- tests/
|   `-- test_tiny_runtime.cpp
|-- examples/
|   |-- tiny_classifier.cpp
|   |-- tiny_mlp.cpp
|   `-- tiny_transformer_block.cpp
|-- benchmarks/
|   |-- bench_tiny_runtime.cpp
|   `-- bench_tiny_transformer.cpp
|-- docs/
|   `-- tiny-runtime.md
|-- .github/
|   `-- ISSUE_TEMPLATE/
|       `-- codex-task.md
|-- .agents/
|   `-- skills/
|-- AGENTS.md
|-- CMakeLists.txt
|-- NEW_CHAT_PROMPT.md
`-- README.md
```

Directory roles:

- `include/aihw/` contains public C++ headers.
- `src/` contains runtime implementations.
- `tests/` contains correctness tests registered with CTest.
- `examples/` contains small runnable demos.
- `benchmarks/` contains benchmark executables.
- `docs/` contains design notes and experiment explanations.
- `.github/` contains the GitHub issue template.
- `.agents/` contains repository-level Codex workflow skills.

## Core Components

### Runtime Library

The main CMake library target is `aihw_runtime`.

It is built from:

- `src/tensor.cpp`;
- `src/ops.cpp`;
- `src/cpu_affinity.cpp`.

It exposes public headers from:

- `include/aihw/tensor.hpp`;
- `include/aihw/ops.hpp`;
- `include/aihw/cpu_affinity.hpp`.

### Tensor

`Tensor` is a small row-major tensor container.

It currently supports:

- 1D shapes;
- 2D shapes;
- contiguous `std::vector<float>` storage;
- shape and size queries;
- row and column queries for 2D tensors;
- bounds-checked flat access with `operator[]`;
- bounds-checked 2D access with `at2d`.

It currently does not support:

- strides;
- views;
- slices;
- broadcasting;
- quantized data;
- external buffer ownership;
- device memory.

### Operators

The runtime operator layer implements the following functions:

- `matmul`;
- `relu`;
- `add`;
- `softmax`;
- `layer_norm`;
- `scaled_dot_product_attention`;
- `transformer_block`.

The current `matmul` path is a CPU implementation for 2D tensors. Internally it
uses a contiguous IKJ loop order.

`softmax` and `layer_norm` support 1D tensors and row-wise 2D tensors.

`scaled_dot_product_attention` is a single-head attention implementation with
an optional causal mask.

`transformer_block` is a tiny pre-norm block assembled from the existing
operators. It is a runtime prototype component, not a full language-model
runtime.

### CPU Affinity Helper

The CPU affinity helper is used by benchmark programs.

It can:

- inspect the current Windows processor group and logical processor;
- attempt to pin the current benchmark thread to a requested logical CPU;
- report whether pinning was requested;
- report whether pinning succeeded;
- report before and after processor locations;
- report error text when pinning fails.

This is an experiment aid. It is not a general recommendation to pin
application threads permanently.

### Tests

`tests/test_tiny_runtime.cpp` is registered as the CTest test:

```text
tiny_runtime_correctness
```

The test executable checks:

- tensor shape and indexing behavior;
- known `matmul` outputs;
- `relu` behavior;
- same-shape `add`;
- 1D and 2D `softmax` properties;
- 1D and 2D `layer_norm` properties;
- tiny classifier pipeline shapes;
- probability row sums;
- attention output shape;
- causal attention behavior;
- transformer block output shape;
- deterministic transformer block behavior;
- invalid-shape exception paths.

### Examples

The example programs show how the runtime pieces compose.

`tiny_mlp` runs:

```text
input -> matmul -> relu -> matmul
```

`tiny_classifier` runs:

```text
input -> matmul -> relu -> matmul -> layer_norm -> softmax
```

`tiny_transformer_block` runs a fixed-size tiny transformer block smoke demo.

### Benchmarks

The benchmark programs are:

- `bench_tiny_runtime`;
- `bench_tiny_transformer`.

`bench_tiny_runtime` measures a configurable tiny classifier workload.

`bench_tiny_transformer` measures a configurable tiny transformer block
workload.

Both benchmark programs print CSV-like sections and support optional
`--pin-cpu N` on Windows.

Benchmark output is useful for local comparison under the same machine,
compiler, build type, and workload settings. It should not be treated as a
cross-machine performance claim.

## Build and Test

Run all commands from the repository root.

Configure a Release build:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
```

Build all targets:

```powershell
cmake --build build
```

Run correctness tests:

```powershell
ctest --test-dir build --output-on-failure
```

After a successful build, useful executables include:

```text
build/test_tiny_runtime.exe
build/tiny_mlp.exe
build/tiny_classifier.exe
build/tiny_transformer_block.exe
build/bench_tiny_runtime.exe
build/bench_tiny_transformer.exe
```

Optional local CPU tuning can be enabled on non-MSVC compilers:

```powershell
cmake `
  -S . `
  -B build `
  -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DAIHW_ENABLE_NATIVE=ON
```

`AIHW_ENABLE_NATIVE` enables local CPU tuning flags such as `-march=native`
where supported. It is off by default so the baseline build remains portable.

## Benchmarks and Examples

Run the minimal MLP example:

```powershell
.\build\tiny_mlp.exe
```

Run the tiny classifier example:

```powershell
.\build\tiny_classifier.exe
```

Run the tiny transformer block example:

```powershell
.\build\tiny_transformer_block.exe
```

Run the tiny runtime benchmark with default settings:

```powershell
.\build\bench_tiny_runtime.exe
```

Run a small configurable tiny runtime benchmark:

```powershell
.\build\bench_tiny_runtime.exe `
  --batch 2 `
  --input-dim 16 `
  --hidden-dim 32 `
  --output-dim 8 `
  --iterations 3
```

Run the transformer benchmark:

```powershell
.\build\bench_tiny_transformer.exe `
  --seq-len 8 `
  --model-dim 16 `
  --ff-dim 32 `
  --iterations 3
```

Try Windows CPU pinning with the tiny runtime benchmark:

```powershell
.\build\bench_tiny_runtime.exe --pin-cpu 0
```

Try Windows CPU pinning with the transformer benchmark:

```powershell
.\build\bench_tiny_transformer.exe --pin-cpu 0
```

## Current Status and Limitations

Implemented:

- C++17 runtime library with small tensor and operator APIs;
- CMake build targets for runtime, tests, examples, and benchmarks;
- correctness tests through CTest;
- tiny MLP example;
- tiny classifier example;
- tiny transformer block example;
- configurable benchmark workloads;
- Windows CPU pinning experiment hook.

Not implemented yet:

- SIMD;
- multithreading;
- tiled or blocked matmul;
- backend selection;
- GPU kernels;
- NPU kernels;
- OpenVINO integration;
- ONNX integration;
- CUDA, SYCL, OpenCL, or oneMKL integration;
- MCU deployment;
- RTOS integration;
- driver or CAN bus work;
- vehicle-grade deployment;
- model loading;
- training;
- autograd;
- tokenizer;
- embedding;
- KV cache;
- autoregressive generation;
- production-level profiling infrastructure;
- CI matrix.

## Engineering Coverage

| Area | Evidence in this repository |
| --- | --- |
| C++ engineering organization | Public headers, source files, tests, examples, and benchmarks are separated. |
| CMake and Ninja workflow | `CMakeLists.txt` defines the library, executables, warnings, and CTest entry. |
| Correctness testing | `tests/test_tiny_runtime.cpp` checks outputs, shapes, and exception paths. |
| Benchmark design | Runtime and transformer benchmarks expose configurable workload sizes. |
| Tiny runtime prototype | Tensor, operator, attention, and transformer block pieces compose a pipeline. |
| Edge AI runtime foundations | Small AI operators are composed without a heavyweight framework. |
| Hardware-aware development | Row-major matmul and CPU pinning reports support local hardware experiments. |
| Engineering documentation | README, design notes, agent rules, and issue templates document the workflow. |

These entries describe the current code and documentation surface. They are
not evidence of completed embedded deployment.

## Development Environment

Local machine used for development:

- Laptop: Xiaomi Redmi Book Pro 14 2024.
- CPU: Intel Core Ultra 5 125H, 14 cores / 18 threads.
- GPU: Intel Arc integrated graphics.
- NPU: Intel AI Boost.
- Memory: 32 GB LPDDR5X.
- Storage: 1 TB NVMe SSD.
- OS: Windows 11.

This hardware context explains why the repository is interested in CPU
benchmarking, future Intel GPU or NPU exploration, and edge AI runtime ideas.

The current code path is CPU-only.
