# CPU Affinity And Logical Processor Benchmark

这个文档说明 `bench_cpu_affinity`。它用于观察当前线程运行在哪个逻辑处理器上、Windows 是否会迁移线程、以及把线程绑定到不同逻辑处理器后，单线程小 benchmark 是否有差异。

## 为什么做这个实验

这台机器使用 Intel Core Ultra 5 125H，属于混合核心 CPU。它不是所有核心都完全一样，而是同时包含偏性能和偏能效的核心。矩阵乘法和内存访问 benchmark 都是单线程实验时，线程实际跑在哪个逻辑处理器上会影响结果。因此需要先观察 Windows 报告的 CPU 拓扑和 affinity 行为。

## 基本术语

- 物理核心 core：CPU 里真实的执行核心。
- 逻辑处理器 logical processor：操作系统看到的可调度 CPU 编号。一个支持 SMT / 超线程的物理核心可能对应两个逻辑处理器。
- 线程 thread：程序里被操作系统调度执行的执行流。本实验只使用当前 benchmark 主线程，不做多线程计算。
- CPU affinity：把线程限制在某个或某些逻辑处理器上运行的设置。
- 线程迁移：操作系统把同一个线程从一个逻辑处理器调度到另一个逻辑处理器。
- P 核：Performance core，通常偏高性能。
- E 核：Efficiency core，通常偏能效。
- LP E 核：Low Power Efficiency core，通常更偏低功耗场景。
- EfficiencyClass：Windows 在 `GetLogicalProcessorInformationEx` 中报告的性能/能效等级。它只能作为参考，不能硬编码成“某个数值一定是 P 核或 E 核”。一般来说，较高值更可能代表性能更强、能效更低；较低值更可能代表更偏能效。

Intel Core Ultra 5 125H 标称 14 核 / 18 线程，是因为不同类型核心的数量和 SMT 支持不同：部分性能核心可能提供两个逻辑处理器，而部分能效核心通常只提供一个逻辑处理器。

## 如何构建和运行

在仓库根目录运行：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\bench_cpu_affinity.exe --quick
.\build\bench_cpu_affinity.exe --cpu 0
```

默认运行等同于遍历 group 0 中的所有逻辑处理器：

```powershell
.\build\bench_cpu_affinity.exe
```

参数：

- `--all`：遍历所有支持的逻辑处理器。默认就是这个行为。
- `--cpu N`：只绑定并测试逻辑处理器 `N`。
- `--quick`：使用更小的数据规模和更少迭代，方便快速试跑。

## 如何看输出

输出是 CSV-like 文本，分成三段。

### current_thread

显示程序刚开始时，当前线程运行在哪个 processor group 和逻辑处理器上：

```text
section,current_thread
group,processor
0,3
```

### cpu_topology

显示 Windows 报告的核心关系：

```text
section,cpu_topology
core_index,efficiency_class,flags,smt_hint,group,mask_hex,logical_processors
0,9,1,1,0,0x0000000000000003,"0;1"
```

- `core_index`：本程序枚举到的物理核心序号。
- `efficiency_class`：Windows 报告的 EfficiencyClass。
- `flags`：Windows 报告的处理器关系 flags。
- `smt_hint`：如果 Windows flags 中包含 SMT 标志，则为 1，否则为 0。
- `mask_hex`：该核心对应的逻辑处理器 bit mask。
- `logical_processors`：该核心包含哪些逻辑处理器编号。

### affinity_sweep

每一行先把当前线程绑定到 `logical_cpu`，再运行一个 64 MiB 顺序读取小 benchmark：

```text
section,affinity_sweep
logical_cpu,before_group,before_processor,after_group,after_processor,best_ms,ns_per_access,gb_per_s,sink
0,0,3,0,0,3.100,0.370,21.600,12345
```

`after_processor` 理想情况下应等于 `logical_cpu`。如果不相等，要如实记录，这可能说明 affinity 设置失败或线程调度行为不符合预期。

## 注意事项

- 这个程序只做初步观察，不会完美证明某个逻辑处理器一定是 P 核、E 核或 LP E 核。
- 结果会受电源模式、温度、后台任务、Windows 调度策略和线程迁移影响。
- 不建议新手为了 benchmark 去 BIOS 里乱改核心、电源或调度相关设置。
- 更可靠的分析需要多次重复运行，并结合任务管理器、HWiNFO 或厂商文档一起看。

## 后续方向

- 增加 `--csv <file>` 输出。
- 增加 pointer chasing affinity sweep。
- 给 `bench_memory_access` 增加 `--pin-cpu` 参数。
- 按 EfficiencyClass 分组重复测试。
- 对照 Windows 任务管理器或 HWiNFO 的逻辑处理器编号显示。
