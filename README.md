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

## Current Direction

The current main line is a tiny AI runtime MVP. Earlier standalone CPU,
memory, and core-observation experiments have been cleaned up so the repository
can focus on the runtime path. Their main lessons now feed into the runtime
implementation and the `--pin-cpu` benchmark option.

The MVP runtime includes `Tensor`, `matmul`, `relu`, `add`, `softmax`,
`layer_norm`, a tiny classifier example, and configurable benchmark workloads.

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\tiny_mlp.exe
.\build\tiny_classifier.exe
.\build\bench_tiny_runtime.exe
.\build\bench_tiny_runtime.exe --batch 2 --input-dim 16 --hidden-dim 32 --output-dim 8 --iterations 3
.\build\bench_tiny_runtime.exe --pin-cpu 0
```

See `docs/tiny-runtime.md` for the tiny AI runtime direction.
