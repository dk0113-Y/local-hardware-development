# Runtime Cleanup Plan

这个文档只给出清理方案，不执行删除。已有实验代码仍然有学习价值，也能作为 tiny runtime 的对照组、硬件观察工具或后续 backend 验证工具。

## 建议保留

- `include/aihw/matmul.hpp`、`src/matmul.cpp`、`tests/test_matmul.cpp`：runtime 的 `matmul` 第一版复用这些实现和测试思路。
- `benchmarks/bench_matmul.cpp`：继续作为矩阵乘法 baseline、blocked IKJ 和后续 backend 对照。
- `benchmarks/bench_cpu_affinity.cpp` 和 `docs/cpu-affinity.md`：后续 runtime 会需要 `--pin-cpu`、P/E 核选择和 affinity sweep。
- `benchmarks/bench_memory_access.cpp` 和 `docs/cpu-memory-access.md`：保留为理解 cache、RAM、pointer chasing 和访问模式的基础工具。
- `docs/phase1.md`：保留为 matmul 阶段的实验记录。

## 建议迁移或归档

- 后续可以把早期实验说明迁移到 `docs/archive/`，例如把纯 matmul 历史结果和 block size sweep 从主线文档中拆出去。
- 后续可以把硬件观察类 benchmark 统一放在 `tools/` 或保持在 `benchmarks/`，但在 README 中标注它们是 support tools，不是 runtime 主线。
- 如果 runtime 文档越来越多，可以新增 `docs/runtime/`，把 `tiny-runtime.md`、backend 设计、CPU pinning 设计放进去。

## 建议后续删除

当前不建议删除任何文件。

如果未来删除，应满足这些条件：

- 文件已经有替代实现或已经迁移到 archive。
- 删除不会破坏 runtime 的对照测试或 benchmark。
- 用户明确确认删除。

可能的未来删除候选：

- 过时的重复 benchmark 结果表格：如果已经迁移到更结构化的 benchmark 记录中，可以从主线文档删减。
- 过时的实验说明：如果和 runtime 主线冲突，先迁移到 archive，再考虑删除。

删除风险：

- 失去性能演化对照。
- 失去初学阶段的解释上下文。
- 让后续 backend 优化缺少 baseline。

## 建议的新目录结构

短期保持当前结构：

- `include/aihw/`：public headers，包括 runtime Tensor / ops 和 CPU matmul API。
- `src/`：runtime 和 CPU kernel 实现。
- `tests/`：correctness tests。
- `benchmarks/`：runtime benchmark 和硬件实验 benchmark。
- `examples/`：可运行示例。
- `docs/`：主线文档和实验说明。

未来如果文件变多，可以考虑：

- `docs/runtime/`：runtime 主线设计文档。
- `docs/archive/`：早期阶段实验记录。
- `tools/`：硬件观察工具和辅助脚本。
- `benchmarks/`：只保留稳定 benchmark 入口。
