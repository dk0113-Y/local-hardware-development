# AI Hardware Lab

This folder is reserved for local experiments around low-level AI and hardware-aware development on this machine.

Machine context:

- Laptop: Xiaomi Redmi Book Pro 14 2024
- CPU: Intel Core Ultra 5 125H, 14 cores / 18 threads
- GPU: Intel Arc integrated graphics
- NPU: Intel AI Boost
- Memory: 32 GB LPDDR5X
- Storage: 1 TB NVMe SSD
- OS: Windows 11

Recommended development tracks:

1. CPU performance engineering: C/C++, SIMD, multithreading, cache-aware kernels.
2. Intel Arc GPU compute: SYCL, OpenCL, DirectX Compute, or Vulkan Compute.
3. Intel NPU deployment: OpenVINO, ONNX, NPU profiling, quantization, graph compilation.
4. Memory and storage experiments: mmap, async I/O, cache behavior, model data pipelines.
5. Safe kernel/system experiments: WSL2 or virtual machine first.

Use `NEW_CHAT_PROMPT.md` to start a new Codex chat with the intended context.

## Current experiment

Phase 1 starts with a minimal C++ CPU matrix multiplication baseline. It is
designed to run on Windows with the detected MinGW-w64 GCC, CMake, and Ninja
toolchain.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bench_matmul.exe
```

See `docs/phase1.md` for the experiment rationale and next steps.
