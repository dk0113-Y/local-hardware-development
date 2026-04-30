# Phase 1: CPU Matmul Baseline

This project starts with a small CPU experiment rather than a large framework.
The first target is a correct and measurable FP32 row-major matrix
multiplication kernel.

## Local Environment Snapshot

- CPU: Intel Core Ultra 5 125H, 14 cores / 18 threads
- GPU: Intel Arc integrated graphics
- OS: Windows 11
- C++ compiler found: MinGW-w64 GCC 15.2.0
- Build tools found: CMake 4.2.3 and Ninja 1.13.2
- MSVC and Clang were not found in PATH during initial setup
- WSL is not required for this first CPU phase
- `g++ -march=native` exposed AVX2, FMA, AVX-VNNI, BMI/BMI2, and SSE4.2
  macros on this machine

## Why This Shape

- A plain C++ kernel gives a transparent baseline before vendor libraries or
  compiler intrinsics hide the cost model.
- Row-major FP32 tensors match common inference layouts and keep the first
  experiment simple.
- `matmul_ijk` is intentionally direct and easy to inspect.
- `matmul_ikj` keeps the same math but streams rows of `B` and `C`, making cache
  behavior visible without adding SIMD or threads yet.
- The benchmark reports both runtime and GFLOP/s so later SIMD, threading, GPU,
  and NPU comparisons have a stable reference point.

## Build And Run

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\bench_matmul.exe
```

Run a larger case:

```powershell
.\build\bench_matmul.exe 512 512 512 3
```

Enable local CPU code generation for comparison:

```powershell
cmake -S . -B build-native -G Ninja -DCMAKE_BUILD_TYPE=Release -DAIHW_ENABLE_NATIVE=ON
cmake --build build-native
.\build-native\bench_matmul.exe 512 512 512 3
```

## Next Experiments

1. Add blocked/tiled matmul to improve cache reuse.
2. Add SIMD kernels using AVX2/FMA or AVX-512 only after checking supported
   instruction sets on this machine.
3. Add thread-level parallelism across rows or tiles.
4. Add profiling notes: CPU frequency, elapsed time, GFLOP/s, and memory access
   pattern.
5. Compare against an Intel Arc path with SYCL/OpenCL/Vulkan Compute after the
   CPU baseline is stable.

## Initial Results

Date: 2026-04-22. These are single-run baseline measurements and will vary
with CPU frequency, thermals, and background load.

Default Release build:

| Shape | Kernel | Best ms | GFLOP/s | Max diff |
| --- | --- | ---: | ---: | ---: |
| 256x256x256 | ijk_baseline | 15.007 | 2.236 | 0 |
| 256x256x256 | ikj_cache_order | 1.544 | 21.727 | 0 |
| 512x512x512 | ijk_baseline | 114.207 | 2.350 | 0 |
| 512x512x512 | ikj_cache_order | 15.273 | 17.575 | 0 |

Native Release build with `AIHW_ENABLE_NATIVE=ON`:

| Shape | Kernel | Best ms | GFLOP/s | Max diff |
| --- | --- | ---: | ---: | ---: |
| 256x256x256 | ijk_baseline | 14.371 | 2.335 | 0 |
| 256x256x256 | ikj_cache_order | 1.388 | 24.180 | 7.63e-06 |
| 512x512x512 | ijk_baseline | 110.915 | 2.420 | 0 |
| 512x512x512 | ikj_cache_order | 6.614 | 40.588 | 1.05e-05 |
