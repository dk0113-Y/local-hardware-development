# New Chat Prompt

我想在 `C:\Users\27319\Desktop\computer\local-hardware-development` 这个仓库中继续推进一个长期的本地硬件感知 AI / 系统开发实验项目。请先读取当前目录、`README.md`、`AGENTS.md`、`docs/phase1.md` 和 `CMakeLists.txt`，然后根据仓库现状和我的电脑硬件条件制定小步推进计划。

我的电脑硬件背景如下：

- 设备：Xiaomi Redmi Book Pro 14 2024
- CPU：Intel Core Ultra 5 125H，14 核 / 18 线程
- GPU：Intel Arc 集成显卡，不支持 CUDA
- NPU：Intel AI Boost，适合 OpenVINO / ONNX / DirectML 推理部署探索
- 内存：32 GB LPDDR5X
- 存储：1 TB NVMe SSD
- 系统：Windows 11

项目目标不是简单调用 PyTorch，而是做更底层、更有硬件意识的 AI / 系统开发实验。请优先考虑这些方向：

1. CPU：用 C/C++ 实现并优化 AI 基础算子，例如 matmul、softmax、layernorm、attention；逐步加入 SIMD、多线程、cache-aware 优化和性能分析。
2. GPU：用 Intel Arc 可用的技术栈做 GPU compute，例如 SYCL、OpenCL、DirectX Compute 或 Vulkan Compute。
3. NPU：用 OpenVINO / ONNX 做 NPU 编译、推理、量化和 profiling，研究哪些模型结构能跑在 Intel AI Boost NPU 上。
4. 内存 / 存储：研究 mmap、异步 I/O、缓存命中、内存布局、KV cache 和数据加载 pipeline。
5. 系统层：如果涉及 kernel 或驱动实验，优先使用 WSL2 或虚拟机隔离风险。

请不要一开始写大工程。先做小而清晰、可验证的实验路线，例如：

1. 检查当前项目结构。
2. 理解已有 CPU baseline，例如 C++ 矩阵乘法。
3. 检查 correctness test 和 benchmark。
4. 再逐步优化到 tiled / blocked matmul、SIMD、多线程。
5. 最后再考虑 GPU / NPU 对比实验。

协作要求：

- 先检查当前目录和现有文件，不要覆盖已有内容。
- 使用本机真实环境做判断，必要时运行命令确认工具链。
- 每一步都解释为什么这样设计。
- 代码要能在 Windows 上运行。
- 如果需要安装依赖，先说明用途和影响。
- 优先做可验证的小实验，每完成一步运行测试或 benchmark。
- 不要执行破坏性命令，不要重置或删除无关文件。
- 按 `AGENTS.md` 工作：不要直接 push 到 `main`，每个任务使用独立分支，完成后建议通过 GitHub Issue / Pull Request 中转。

请从检查目录、确认当前阶段和提出下一步计划开始。
