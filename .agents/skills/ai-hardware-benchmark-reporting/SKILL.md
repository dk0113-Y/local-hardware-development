---
name: ai-hardware-benchmark-reporting
description: Use this repository-level skill to organize, compare, and explain benchmark results for local-hardware-development, especially CPU matmul, tiled or blocked matmul, SIMD, multithreading, GPU compute, NPU/OpenVINO, or local inference experiments; trigger on requests to summarize benchmarks, compare matmul results, generate a performance report, update experiment results, or interpret bench_matmul output.
---

# AI Hardware Benchmark Reporting

Use this skill to turn benchmark output into a careful performance report.

## Principles

- Separate correctness from performance.
- Avoid over-interpreting a single run.
- Prefer reproducible details over broad claims.
- If updating docs, prefer `docs/phase1.md` for current phase notes or a new `docs/benchmark-*.md` file for a focused report.

## Extract Or Record

Capture as many of these fields as the input supports:

- 实现名称。
- 矩阵尺寸。
- 数据类型。
- build type。
- 编译选项。
- 运行时间。
- GFLOP/s，如果可计算。
- max diff / correctness 结果。
- 测试次数。
- 机器环境或备注。

## Interpretation Checklist

Mention factors that may affect results:

- Debug vs Release。
- CPU 频率。
- 温度。
- 后台任务。
- 矩阵尺寸。
- cache locality。
- 编译器优化。

## Output

Output in Chinese. Include a concise summary, a cautious conclusion, and recommended next experiments such as larger matrix sizes, repeated runs, native build comparison, tiled matmul, SIMD, multithreading, or GPU/NPU comparison when appropriate.
