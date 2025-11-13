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
| `code3_1_1` | 并行循环：迭代变量 `i` 默认私有 |
| `code3_1_2` | 创建并行区域，线程数默认等于逻辑 CPU 核数 |
| `code3_1_3` | 介绍 `team` 概念 |
| `code3_1_4` | 嵌套并行示例 |
| `code3_1_5` | `num_threads` 与 `omp_set_dynamic` 搭配使用 |
| `code3_1_6` | `nowait` 子句性能优化 |
| `code3_1_7` | `collapse` 子句展开多层循环 |
| `code3_1_8` | `linear` 子句多变量迭代 |
| `code3_1_9` | `section` 子句并行独立任务 |
| `code3_1_10` | `firstprivate` 子句用法 |
| `code3_1_11` | `single` 子句示例 |
| `code3_1_12` | `master` 子句（仅主线程执行） |
| `code3_1_13` | `omp loop` 与 `omp for` 的区别 |
| `code3_1_14` | 并行随机访问迭代器 |
| `code3_1_15` | 动态调整线程数 |
| `code3_1_16` | `omp_get_num_threads` 使用范围 |

### 🧠 线程绑定与亲和性
| 代码 | 内容说明 |
|------|----------|
| `code3_2_1` | 三种线程亲和性形式 |
| `code3_2_2` | 亲和性参数与使用方式 |
| `code3_2_3` | 显示与调试线程亲和性的方法 |
| `code3_2_4` | 多层嵌套并行示例 |

### 🧱 同步机制与原子操作
| 代码 | 内容说明 |
|------|----------|
| `code3_6_1` ~ `code3_6_11` | 各种同步机制（critical、barrier、atomic、锁等）详解 |
| `code3_7_1` ~ `code3_7_12` | 数据共享属性（private、firstprivate、copyin、reduction 等） |

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
