# 🌿 Litchi 并行计算学习路径总览

此项目记录了 **Litchi 从 MPI 入门基础开始的完整学习路径**。  
内容涵盖：  
- MPI 并行编程的基础与实践  
- OpenMP 并行示例  
- 性能分析与调试工具（gprof、VTune、GDB 等）  
- CPU 绑定与线程调度策略  

---

## 📘 Code 1：MPI 并行编程基础（书本改写）
基于《高性能计算并行编程技术 - MPI 并行程序设计》一书的 Fortran 代码，改写为 C++ 实现。

| 代码目录 | 对应原书程序 | 内容说明 |
|-----------|---------------|----------|
| `code1` | 程序21 | 用 `MPI_SEND` 和 `MPI_RECV` 实现 Jacobi 迭代 |
| `code1_1` | 程序22 | 用 `MPI_SENDRECV` 实现 Jacobi 迭代 |
| `code1_2` | 程序23 | 用虚拟进程实现 Jacobi 迭代 |
| `code1_3` | 程序24 | 矩阵向量乘法 |
| `code1_4` | 程序25 | 主进程按顺序与乱序打印从进程消息 |
| `code1_5` | 程序26 | 使用缓存通信模式发送消息 |
| `code1_6` | 程序27 | 同步模式消息发送 |
| `code1_7` | 程序28 | 阻塞通信模式示例 |

---

## 🧩 Code 2：MPI 任务与性能分析

| 代码目录 | 任务说明 |
|-----------|-----------|
| `code2_1` | 任意进程数 Jacobi 阻塞版本实现，输出结果汇总（表格、txt 或 excel） |
| `code2_2` | 基础性能剖析，使用 `MPI_Wtime` 分析程序各部分耗时 |
| `code2_2_1` | `code2_2` 的二维版本 |
| `code2_4` | 任意进程数 Jacobi 非阻塞版本（通信与计算重叠），与阻塞版对比 |
| `code2_4_1` | `code2_4` 的二维版本 |

---

## ⚙️ Code 3：OpenMP 示例集

基于《openmp-examples-5-0-1》示例编写。以下为章节与代码对应关系。

### 🧮 并行结构与基本语法
| 代码 | 内容说明 |
|------|----------|
| `code3_1_1` | 并行循环：循环迭代变量 `i` 默认私有，每个线程都会有以及独立的 `i` |
| `code3_1_2` | 创建并行区域，程序生成多个线程，线程数默认等于逻辑 CPU 核数 |
| `code3_1_3` | 介绍 `team` 概念 |
| `code3_1_4` | 嵌套并行示例 |
| `code3_1_5` | `num_threads` 与 `omp_set_dynamic` 搭配使用 |
| `code3_1_6` | 使用`nowait` 子句进行性能优化的条件及特殊情况 |
| `code3_1_7` | `collapse` 子句将多层循环展开到一维空间内分组进行迭代 |
| `code3_1_8` | `linear` 子句，如果 `for` 循环中有不止一个变量随着循环的迭代变化，使用 `linear` 子句实现 |
| `code3_1_9` | `section` 子句并行独立任务：把一组互相独立的任务（非循环迭代）分配给多个线程去执行 |
| `code3_1_10` | `firstprivate` 子句用法 |
| `code3_1_11` | `single` 子句示例 |
| `code3_1_12` | `master` 子句：只有主线程执行这段代码，其他线程跳过 |
| `code3_1_13` | `omp loop` 与 `omp for` 的区别：`omp loop` 不止支持 `for` 循环|
| `code3_1_14` | 并行随机访问迭代器循环 |
| `code3_1_15` | 使用 `omp_set_dynamic` and `omp_set_num_threads` 实现线程数量的动态调整的具体实现方法 |
| `code3_1_16` | `omp_get_num_threads` 使用范围：只能在并行内部使用才能得到当前并行线程的数量 |

### 🧠 线程绑定与亲和性
| 代码 | 内容说明 |
|------|----------|
| `code3_2_1` | 三种线程亲和性（Thread Affinity）形式 |
| `code3_2_2` | 亲和性（affinity）参数与使用方式 |
| `code3_2_3` | 显示与调试线程亲和性的方法 |
| `code3_2_4` | 多层嵌套并行示例 |

### 🧱 同步机制与原子操作
| 代码 | 内容说明 |
|------|----------|
| `code3_6_1` | 同步机制中临界区的概念 |
| `code3_6_2` | 解释 `critical` 与 `工作共享结构` 之间的嵌套关系 |
| `code3_6_3` | 使用 `barrier` 实现屏障同步机制 |
| `code3_6_4` | 使用 `atomic` 避免竞态问题并给出了一个 `atomic` 在简单自旋锁上的应用 |
| `code3_6_5` | 列举一个错误的例子说明"所有对同一内存位置执行的原子操作，必须是相同类型、相同语义的操作"这一规范 |
| `code3_6_6` | 说明当 `#pragma omp flush` 不带变量列表时，哪些共享变量会被同步、哪些不会 |
| `code3_6_7` | 描述基于 `release /acquire` 语义的同步 |
| `code3_6_8` | 使用 `order` 子句指定代码必须按顺序执行 |
| `code3_6_9` | 演示了 OpenMP 中的依赖对象 `depend object` 机制 |
| `code3_6_10` | 跨迭代依赖示例 |
| `code3_6_11` | 锁机制 |
| `code3_7_1` | `threadprivate` 的使用方法 |
| `code3_7_2` | `default(none)` 子句的作用 |
| `code3_7_3` | `private` 子句的作用并说明全局变量与线程私有副本的区别 |
| `code3_7_7` | `firstprivate` 子句为指针变量分配线程副本时为浅拷贝，所有线程指向同一地址 |
| `code3_7_8` | `lastprivate` 子句的用法 |
| `code3_7_9_1` | `reduction` 子句的用法 |
| `code3_7_9_2` | 对应7.9.2 的任务归约机制，展示了如何在并行任务 `task` 和工作共享循环 `for` 中进行任务级别的归约操作 |
| `code3_7_9_3` | 对应了7.9.3 的隐性映射行为，保证了设备与主机上数据的一致性 |
| `code3_7_9_4` | 展示了如何把设备端任务 `target task` 纳入任务归约 `task reduction` 中的三种方法 |
| `code3_7_9_5` | 展示了如何通过两种不同方式使用 `taskloop reduction` 实现数组归约  |
| `code3_7_9_6` | 展示了用户自定义归约 `declare reduction` 的用法 |
| `code3_7_10` | `inscan` 子句的作用和用法 |
| `code3_7_11` | `copyin` 子句的作用：在各线程进入并行区域时，把主线程中 `threadprivate`  变量的复制给每个线程的对应副本   |
| `code3_7_12` | `copyin` 子句的作用（在各线程进入并行区域时，把主线程中 `threadprivate` 变量的复制给每个线程的对应副本）和 `copyprivate` 子句使用的注意事项   |

---

## 🧮 Code 4：OpenMP 实践任务
| 代码 | 内容说明 |
|------|----------|
| `code4_1` | 非平衡循环计算：不同调度策略与线程数对比 |
| `code4_2` | CPU 绑定实现（命令行与 `sched_xxxaffinity`、`pthread` 接口示例） |

---

## 🔬 Code 5：性能调试与分析工具

以 `code2_4_1` 为实例代码，记录性能分析常用命令及作用。

### 🧰 1. GNU 工具链 (gprof)
| 命令 | 作用 |
|------|------|
| `mpicxx -O2 -g -pg -o jacobi_mpi_profile jacobi_mpi.cpp` | 编译程序并启用 gprof 性能分析 |
| `./jacobi_mpi_profile` | 运行程序，生成 `gmon.out` |
| `gprof ./jacobi_mpi_profile gmon.out > analysis.txt` | 生成性能分析报告 |

### 🪲 2. GDB 调试
| 命令 | 作用 |
|------|------|
| `break func_name` | 设置断点 |
| `run` | 启动程序 |
| `next` / `step` | 单步执行 |
| `print var` | 打印变量 |
| `info threads` | 查看线程 |
| `thread <id>` | 切换线程 |
| `continue` | 继续执行 |
| `make breakpoint pending on future shared library load?` | 延迟断点提示 |

### ⚙️ 3. CPU 亲和性与线程绑定
| 命令 | 作用 |
|------|------|
| `pthread_self()` | 获取当前线程句柄 |
| `gettid()` | 获取线程系统 ID |
| `sched_setaffinity(pid, sizeof(mask), &mask)` | 绑定进程/线程到指定核心 |
| `pthread_setaffinity_np(pthread, sizeof(mask), &mask)` | 绑定 pthread 线程到核心 |
| `CPU_ZERO(&mask)` | 初始化 CPU 掩码 |
| `CPU_SET(n, &mask)` | 设置 CPU 掩码 |
| `sched_getcpu()` | 查询当前线程运行的 CPU 核心 |

### 🧵 4. OpenMP
| 命令 | 作用 |
|------|------|
| `#pragma omp parallel for schedule(static|dynamic|guided) num_threads(n)` | 循环并行化 |
| `shared(var)` | 线程共享变量 |
| `private(var)` | 线程私有变量 |
| `reduction(+:var)` | 归约操作 |

### 🔎 5. Intel VTune Profiler
| 命令 | 作用 |
|------|------|
| `source /opt/intel/oneapi/vtune/latest/env/vars.sh` | 设置 VTune 环境 |
| `vtune -collect hotspots -result-dir vtune_result ./jacobi_mpi_profile` | 热点分析 |
| `vtune -collect threading -result-dir vtune_thread ./jacobi_mpi_profile` | 线程分析 |
| `vtune -collect memory-access -result-dir vtune_mem ./jacobi_mpi_profile` | 内存访问分析 |
| `vtune -report summary -result-dir vtune_result` | 生成汇总报告 |
| `vtune-gui` | 打开图形界面查看结果 |

### 💻 6. MPI
| 命令 | 作用 |
|------|------|
| `mpicxx -O2 -g -pg -o jacobi_mpi_profile jacobi_mpi.cpp` | 编译 MPI 程序 |
| `mpirun -np 8 ./jacobi_mpi_profile` | 启动 8 个进程并行执行 |

### 🧭 7. 系统验证与监控
| 命令 | 作用 |
|------|------|
| `htop` | 查看 CPU 核与线程分布 |
| `ps -L` | 查看线程及 CPU 核心分配 |

---

> ✨ 本项目旨在通过实践全面掌握 MPI、OpenMP 与性能调优工具的使用。  
> 所有代码均可直接运行，并配有注释与性能分析结果。
