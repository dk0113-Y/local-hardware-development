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

## Blocked IKJ Baseline Results

Date: 2026-05-01. These are local Release measurements for the first
`matmul_blocked_ikj` baseline using a fixed block size of 64. They are still
single-run style benchmark results and should be rechecked before drawing broad
conclusions.

| Shape | Kernel | Best ms | GFLOP/s | Max diff |
| --- | --- | ---: | ---: | ---: |
| 256x256x256 | ijk_baseline | 14.364 | 2.336 | 0 |
| 256x256x256 | ikj_cache_order | 1.651 | 20.326 | 0 |
| 256x256x256 | blocked_ikj_bs64 | 1.531 | 21.914 | 0 |
| 512x512x512 | ijk_baseline | 103.416 | 2.596 | 0 |
| 512x512x512 | ikj_cache_order | 12.061 | 22.257 | 0 |
| 512x512x512 | blocked_ikj_bs64 | 12.772 | 21.017 | 0 |
| 1024x1024x1024 | ijk_baseline | 3364.977 | 0.638 | 0 |
| 1024x1024x1024 | ikj_cache_order | 104.364 | 20.577 | 0 |
| 1024x1024x1024 | blocked_ikj_bs64 | 107.835 | 19.915 | 0 |

In this run, blocking improves the 256 case slightly but is slower than plain
`ikj_cache_order` for 512 and 1024. The likely next step is testing multiple
block sizes such as 16, 32, 64, and 128 before changing the default approach.

## Block Size Sweep Results

Date: 2026-05-01. These are local Release measurements comparing blocked IKJ
block sizes 16, 32, 64, and 128. The 2048 case was also run, but it is much
slower because the benchmark still includes `ijk_baseline`.

| Shape | Kernel | Best ms | GFLOP/s | Max diff |
| --- | --- | ---: | ---: | ---: |
| 256x256x256 | ijk_baseline | 14.709 | 2.281 | 0 |
| 256x256x256 | ikj_cache_order | 1.426 | 23.524 | 0 |
| 256x256x256 | blocked_ikj_bs16 | 2.660 | 12.612 | 0 |
| 256x256x256 | blocked_ikj_bs32 | 1.720 | 19.506 | 0 |
| 256x256x256 | blocked_ikj_bs64 | 1.501 | 22.353 | 0 |
| 256x256x256 | blocked_ikj_bs128 | 1.464 | 22.926 | 0 |
| 512x512x512 | ijk_baseline | 104.311 | 2.573 | 0 |
| 512x512x512 | ikj_cache_order | 12.728 | 21.090 | 0 |
| 512x512x512 | blocked_ikj_bs16 | 21.552 | 12.455 | 0 |
| 512x512x512 | blocked_ikj_bs32 | 14.751 | 18.197 | 0 |
| 512x512x512 | blocked_ikj_bs64 | 12.613 | 21.283 | 0 |
| 512x512x512 | blocked_ikj_bs128 | 12.445 | 21.569 | 0 |
| 1024x1024x1024 | ijk_baseline | 3147.854 | 0.682 | 0 |
| 1024x1024x1024 | ikj_cache_order | 99.681 | 21.544 | 0 |
| 1024x1024x1024 | blocked_ikj_bs16 | 180.151 | 11.920 | 0 |
| 1024x1024x1024 | blocked_ikj_bs32 | 127.445 | 16.850 | 0 |
| 1024x1024x1024 | blocked_ikj_bs64 | 111.077 | 19.333 | 0 |
| 1024x1024x1024 | blocked_ikj_bs128 | 117.595 | 18.262 | 0 |
| 2048x2048x2048 | ijk_baseline | 85647.928 | 0.201 | 0 |
| 2048x2048x2048 | ikj_cache_order | 2383.211 | 7.209 | 0 |
| 2048x2048x2048 | blocked_ikj_bs16 | 1479.401 | 11.613 | 0 |
| 2048x2048x2048 | blocked_ikj_bs32 | 1034.150 | 16.613 | 0 |
| 2048x2048x2048 | blocked_ikj_bs64 | 944.290 | 18.193 | 0 |
| 2048x2048x2048 | blocked_ikj_bs128 | 903.277 | 19.019 | 0 |

For 256 and 512, block size 128 is the best blocked variant and is close to
plain `ikj_cache_order`. For 1024, plain `ikj_cache_order` wins. For 2048,
blocking helps substantially and block size 128 is best in this run. The next
benchmark improvement should be a way to skip `ijk_baseline` for large sizes so
large blocked-kernel comparisons are less dominated by the slow reference run.
