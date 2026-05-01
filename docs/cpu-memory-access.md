# CPU Memory Access Benchmark

这个文档说明 `bench_memory_access` 这个小实验。它用于观察 CPU 访问内存时，访问模式和数据规模怎样影响性能。

## 为什么做这个实验

矩阵乘法优化不只是“乘加次数”的问题，也强烈依赖数据怎么从内存进入 CPU。之前的 `matmul_ikj` 和 blocked IKJ 实验已经说明，访问顺序会明显影响速度。这个 benchmark 把矩阵乘法拆开，只观察更基础的内存访问行为：

1. 顺序访问为什么通常很快。
2. 跨步访问为什么可能变慢。
3. 随机链式访问为什么更容易暴露内存延迟。
4. 数组大小超过不同缓存层级后，平均访问时间如何变化。

## 术语

- Cache：CPU 附近的高速缓存，比 RAM 快很多，但容量更小。
- L1 / L2 / L3：不同层级的 Cache。L1 最靠近 CPU 核心，通常最快也最小；L2 更大一些；L3 通常更大、更慢，并可能由多个核心共享。
- RAM：主内存，容量大，但访问延迟比 Cache 高。
- sequential access：顺序访问，例如从数组第 0 个元素一路读到最后一个元素。
- strided access：跨步访问，例如每隔 64 字节或 4096 字节读一次。
- pointer chasing：链式随机访问，每次下一步依赖当前读出的结果，CPU 很难提前预取下一次要访问的位置。
- ns/access：平均每次访问需要多少纳秒，越低越好。
- GB/s：粗略数据吞吐量，越高通常表示连续读带宽越好。

## 如何构建和运行

在仓库根目录运行：

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\bench_memory_access.exe
```

输出是 CSV-like 文本，可以复制到表格里分析。字段包括：

- `benchmark`：实验名称。
- `array_kib`：数组大小，单位 KiB。
- `access_pattern`：访问模式。
- `stride_bytes`：跨步大小，单位 byte；不适用时为 0。
- `iterations`：重复次数。
- `total_accesses`：计时区域内的访问次数。
- `best_ms`：多次运行中最快的一次耗时。
- `ns_per_access`：平均每次访问耗时。
- `gb_per_s`：粗略吞吐量；pointer chasing 不适合解释为带宽，因此输出 0。
- `sink`：用于防止编译器把读取循环优化掉的结果值。

## 实验内容

### sequential_read

顺序读取 `uint64_t` 数组并累加。这个访问模式通常最容易被 CPU cache 和硬件预取器优化，所以小数组和连续大数组通常会有较好的 GB/s。

### strided_read

固定使用 64 MiB 数组，用 8、16、32、64、128、256、512、1024、4096 字节等不同 stride 读取。stride 变大后，每次访问更可能跳到新的 cache line 或新页面，吞吐可能下降，`ns/access` 可能上升。

### pointer_chasing

先把数组索引随机打乱，再串成一个环。每次访问都要先读出当前元素，才能知道下一次访问哪里。这会削弱 CPU 预取器的作用，更容易看到内存访问延迟，尤其是数组变大、无法留在 Cache 里时。

## 如何解读结果

- 小数组如果能放进 L1/L2/L3 Cache，`ns/access` 通常较低。
- 数组变大后，如果超过某级 Cache 容量，`ns/access` 可能上升。
- sequential_read 的 GB/s 通常比 pointer_chasing 更高，因为顺序访问更适合预取和批量搬运。
- strided_read 的结果要看 stride 大小。小 stride 接近顺序访问，大 stride 更容易浪费 cache line。
- pointer_chasing 的 `gb_per_s` 输出为 0，因为它主要观察延迟，不适合当作连续带宽测试。

## 注意事项

第一次结果不要当作硬件绝对真值。结果会受 CPU 频率、温度、后台任务、电源模式、编译器优化和系统调度影响。更可靠的做法是重复运行多次，并记录机器状态。
