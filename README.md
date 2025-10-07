此项目记录了Litchi从MPI入门基础开始的所有学习路径，  
其中 code1 为《高性能计算并行编程技术-MPI并行程序设计》一书中Fortran代码的c++改写版本，具体如下：    
code1 对应 程序21：用MPI_SEND和MPI_RECV实现的Jacobi迭代  
code1_1 对应 程序22：用MPI_SENDRECV实现的Jacobi迭代  
code1_2 对应 程序23：用虚拟进程实现的Jacobi迭代  
code1_3 对应 程序24：矩阵向量乘  
code1_4 对应 程序25：主进程按续与乱续打印从进程的消息  
code1_5 对应 程序26：使用缓存通信模式发送消息  
code1_6 对应 程序27：同步模式的消息发送  
code1_7 对应 程序28：阻塞通信模式实例  

code2_1 对应任务2.1 任意进程数Jacobi迭代阻塞版本实现，结果汇总输出（输出⼀个table，以直接打印、txt、excel格式均可）  
code2_2 对应任务2.2 基础性能剖析，通过MPI_Wtime等计时⼯具，分析程序各部分耗时（初始化、计算等）  
code2_2_1 对应code2_2的二维形式  
code2_4 对应任务2.4 任意进程数Jacobi迭代非阻塞版本实现（通过MPI通信与计算的重叠实现），与阻塞版本对比耗时  
code2_4_1 对应code2_4的二维形式  

其中 code3 为《openmp-examples-5-0-1》一书中的代码编写，具体如下：  
code3_1_1 对应1.1 一个简单的并行循环，强调循环迭代变量 i 默认是私有的，每个线程都会有自己独立的 i  
code3_1_2 对应1.2 并行架构：创建并行区域，程序生成多个线程，线程数默认等于机器的逻辑CPU核数  
code3_1_3 对应1.3 openmp 中的 team 的概念 
code3_1_4 对应1.4 嵌套并行的运行方式  
code3_1_5 对应1.5 介绍 num_threads 子句和 omp_set_dynamic 函数的常见搭配  
code3_1_6 对应1.7 介绍使用 nowait 子句进行性能优化的条件及特殊情况  
code3_1_7 对应1.8 介绍使用 collapse 子句的用法：将多层循环展开到一维空间内分组进行迭代  
code3_1_8 对应1.9 介绍使用 linear 子句的用法，如果 for 循环中有不止一个变量随着循环的迭代而变化，使用 linear 子句实现  
code3_1_9 对应1.10 介绍使用 #pragma omp section 子句把一组互相独立的任务（非循环迭代）分配给多个线程去执行  
code3_1_10 对应1.11 介绍firstprivate 子句的用法  
code3_1_11 对应1.12 介绍 single 子句的用法  
code3_1_12 对应1.14 介绍 master 子句的用法：只有主线程执行这段代码，其他线程直接跳过  
code3_1_13 对应1.15 介绍 #pragma omp loop 与 #pragma omp for 有类似的功能，但是不止止支持 for 循环  
code3_1_14 对应1.16 介绍并行随机访问迭代器循环  
code3_1_15 对应1.17 介绍使用 omp_set_dynamic and omp_set_num_threads 实现线程数量的动态调整的具体实现方法  
code3_1_16 对应1.18 介绍 omp_get_num_threads 子句的作用范围：只在并行内部使用才能得到当前并行线程的数量  