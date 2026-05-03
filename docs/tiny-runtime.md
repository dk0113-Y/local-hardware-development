# Tiny AI Runtime MVP

当前阶段定义为 `tiny runtime MVP`。MVP 是 Minimum Viable Product（最小可用产品）的缩写：这里表示 runtime 已经不只是固定 demo，而是可以表达一个小型分类推理 pipeline，并且 benchmark workload 可以通过命令行配置。

## 当前支持

- 1D / 2D row-major `Tensor`。
- `matmul`：2D 矩阵乘法。
- `relu`：逐元素 ReLU 激活函数。
- `add`：相同 shape 的逐元素加法。
- `softmax`：1D 全向量 softmax，2D 逐行 softmax。
- `layer_norm`：1D 全向量 layer normalization，2D 逐行 layer normalization。
- `tiny_classifier` pipeline：`input -> matmul -> relu -> matmul -> layer_norm -> softmax -> probabilities`。
- `bench_tiny_runtime` configurable workload：支持通过 CLI 配置 batch、输入维度、隐藏层维度、输出维度和迭代次数。
- `bench_tiny_runtime --pin-cpu N`：在 Windows 上尝试把当前 benchmark 线程绑定到指定 logical CPU。

## 当前不支持

- 多线程。
- SIMD。
- backend 选择。
- OpenBLAS / oneMKL。
- OpenVINO / ONNX。
- transformer。
- 模型加载。
- 训练。
- 自动求导。
- 复杂计算图。

## softmax 是什么

`softmax` 常用于分类模型的最后一步。它把一行 logits（模型原始分数）转换成概率分布：每个输出值大于 0，并且同一行的概率和接近 1。本项目使用数值稳定写法：先减去每行最大值，再计算 `exp` 和归一化，避免指数计算过大。

## layer_norm 是什么

`layer_norm` 是 layer normalization（层归一化）。它对同一行数据计算均值和方差，然后执行 `(x - mean) / sqrt(variance + epsilon)`，让输出均值接近 0、方差接近 1。当前第一版不支持 gamma / beta 参数，只做标准化。

## configurable workload 的意义

可配置 workload 让同一个 benchmark 可以测试不同问题规模。例如小 batch 适合快速正确性检查，大维度适合观察 CPU 计算耗时和 `--pin-cpu` 对结果的影响。这比固定 demo 更接近后续做 backend 对照和硬件实验的需求。

## 构建和运行

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\tiny_classifier.exe
.\build\bench_tiny_runtime.exe
.\build\bench_tiny_runtime.exe --batch 16 --input-dim 1024 --hidden-dim 2048 --output-dim 512 --iterations 10
.\build\bench_tiny_runtime.exe --batch 16 --input-dim 1024 --hidden-dim 2048 --output-dim 512 --iterations 10 --pin-cpu 0
.\build\bench_tiny_runtime.exe --batch 16 --input-dim 1024 --hidden-dim 2048 --output-dim 512 --iterations 10 --pin-cpu 2
```

这个文档说明 `local-hardware-development` 的新主线：一个本机硬件感知的 tiny AI runtime。

## 目标

第一版 runtime 的目标不是做高性能，也不是做完整 AI 框架，而是建立一个干净、可扩展、可测试的小型运行时骨架。旧的 standalone CPU / memory / affinity 实验入口已经清理，核心经验迁移到 runtime 主线中。

## 为什么从 Tensor、matmul、relu 和 tiny MLP 开始

- Tensor 是 AI runtime 的基本数据容器。
- matmul 是神经网络和 Transformer 中最核心的算子之一。
- relu 是最简单的逐元素激活函数，适合验证 elementwise operator 的组织方式。
- tiny MLP 可以把多个算子串起来，形成一个最小可运行 pipeline。

## 和旧实验的关系

- 旧 matmul 实验验证了 IKJ 访问顺序的重要性；runtime 现在在 `src/ops.cpp` 内部保留一个简单的 contiguous IKJ matmul helper。
- 旧 memory access 实验帮助理解顺序访问、跨步访问和 pointer chasing；这些经验会用于后续 Tensor layout 和 backend 设计。
- 旧 CPU affinity 实验帮助理解 logical processor 和 Windows pinning；runtime benchmark 现在保留 `--pin-cpu` 入口。

## 当前支持

- 1D / 2D row-major `Tensor`。
- `matmul`：只支持 2D Tensor。
- `relu`：支持任意当前 Tensor shape 的逐元素 ReLU。
- `add`：只支持完全相同 shape，不支持 broadcast。
- `tiny_mlp` 示例：`input -> matmul -> relu -> matmul`。
- `bench_tiny_runtime`：测量 tiny MLP pipeline 的 best ms，并支持实验性的 `--pin-cpu N`。

## 当前不支持

- 不支持 stride、view、slice 或 broadcast。
- 不支持自动求导。
- 不支持训练。
- 不支持复杂计算图。
- 不支持多线程、SIMD、OpenBLAS、oneMKL、OpenVINO 或 ONNX。
- 不支持 Transformer block。
- 不支持旧 standalone matmul / memory / affinity benchmark 入口。

## 构建和运行

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
.\build\tiny_mlp.exe
.\build\bench_tiny_runtime.exe
.\build\bench_tiny_runtime.exe --pin-cpu 0
```

## CPU pinning experiment

`bench_tiny_runtime` 支持实验性的 `--pin-cpu N` 参数。它不是默认性能建议，而是用来观察同一个 tiny MLP pipeline 绑定到不同 logical CPU 后的表现。

示例：

```powershell
.\build\bench_tiny_runtime.exe
.\build\bench_tiny_runtime.exe --pin-cpu 0
.\build\bench_tiny_runtime.exe --pin-cpu 2
.\build\bench_tiny_runtime.exe --pin-cpu 16
```

在 Intel 混合架构上，不同 logical CPU 可能对应 P 核、E 核或低功耗 E 核。`--pin-cpu` 会限制 Windows 调度器把当前 benchmark 线程放到指定 logical CPU 上，这有助于做实验，但不建议普通应用长期硬绑定。

输出中的 `tiny_runtime_affinity` 段会记录 requested CPU、pin 是否成功、绑定前后的 group / processor。后续如果重新引入 CPU topology helper，可以把这些结果和 P/E core 分组做更谨慎的分析。

## 后续路线

1. 增加可配置 workload。
2. 增加 softmax。
3. 增加 layer norm。
4. 增加 backend 选择。
5. 接入 P/E core selection。
6. 增加 thread pool。
7. 增加 SIMD backend。
8. 增加 OpenBLAS / oneMKL 对照。
