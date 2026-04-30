# New Chat Prompt

我想在 `C:\Users\27319\Desktop\computer\ai-hardware-lab` 这个文件夹中开始一个长期的本地硬件深度开发实验项目。请你先读取这个目录和 `README.md`，然后根据我的电脑硬件条件制定开发路线并逐步实现代码。

我的电脑硬件背景如下：

- 设备：Xiaomi Redmi Book Pro 14 2024
- CPU：Intel Core Ultra 5 125H，14 核 18 线程
- GPU：Intel Arc 集成显卡，不支持 CUDA
- NPU：Intel AI Boost，适合 OpenVINO/ONNX/DirectML 推理部署，不适合裸 kernel 编程
- 内存：32GB LPDDR5X
- 存储：1TB NVMe SSD
- 系统：Windows 11
- 已知状态：没有 NVIDIA GPU，因此本机不能使用 CUDA；BIOS 虚拟化已开启；WSL 当前可能尚未安装

我的目标不是简单调用 PyTorch，而是做更底层、更有硬件意识的 AI/系统开发。请优先考虑这些方向：

1. CPU：用 C/C++ 实现并优化 AI 基础算子，例如 matmul、softmax、layernorm、attention；逐步加入 SIMD、多线程、cache-aware 优化和性能剖析。
2. GPU：用 Intel Arc 可用的技术栈做 GPU compute，例如 SYCL、OpenCL、DirectX Compute 或 Vulkan Compute；目标是实现类似 CUDA kernel 学习路线的矩阵乘和 Transformer 子算子。
3. NPU：用 OpenVINO/ONNX 做 NPU 编译、推理、量化和 profiling；研究哪些模型结构能跑在 Intel AI Boost NPU 上，哪些会 fallback 到 CPU/GPU。
4. 内存/存储：研究 mmap、异步 I/O、缓存命中、内存布局、KV cache、数据加载 pipeline。
5. 系统层：如果涉及 kernel 或驱动实验，请优先使用 WSL2/虚拟机隔离风险，不要直接破坏主系统。

请先不要一上来写大工程。先做一个小而清晰的实验路线，例如：

1. 创建项目结构。
2. 写一个 CPU baseline，例如 C++ 矩阵乘法。
3. 添加 benchmark 和正确性测试。
4. 再逐步优化到 SIMD、多线程。
5. 然后再考虑 GPU/NPU 对比实验。

请遵循这些协作要求：

- 先检查当前目录和现有文件，不要覆盖我已有内容。
- 使用本机真实环境做判断，必要时运行命令确认工具链。
- 每一步都要解释为什么这样设计。
- 代码要能在 Windows 上运行。
- 如果需要安装依赖，先说明用途和影响。
- 优先做可验证的小实验，每完成一步运行测试或 benchmark。
- 不要做破坏性命令，不要重置或删除无关文件。

请从检查目录、确认可用编译工具链和提出第一阶段计划开始。
