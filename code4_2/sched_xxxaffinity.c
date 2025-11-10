// // 通过 sched_xxxaffinity 接口实现通过编程绑定 CPU 核心，该接口既能绑定进程又能绑定线程
// // 创建两个线程，分别将它们绑定在不同的 CPU 核，定期打印它们当前在哪个 CPU 核上运行
// // cpu_set_t cpu_mask;
// // CPU_ZERO(&cpu_mask);
// // CPU_SET(cpu_id, &cpu_mask);
// // sched_getaffinity(gettid(), sizeof(cpu_mask), &cpu_mask);

// #define _GNU_SOURCE                                                       // 告诉编译器启用 GNU 扩展与额外接口声明
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>                                                       // getpid()、sysconf()、sleep() 等的声明
// #include <pthread.h>                                                      // pthread_create()、pthread_t 等的声明
// #include <sched.h>                                                        // shed_setaffinituy、sched_getaffinity 等的声明

// // 通过 ps 命令查询当前线程的实际运行 CPU
// void print_running_cpu()
// {
//     char qry_cmd[1024] = {0};                                             // 分配一个 1024 字节的字符数组并全部初始化为0，用来保存 shell 命令字符串。初始化为0可避免未定义尾部字符

//     // 将格式化后的命令写入 qry_cmd
//     // 命令作用：显示该进程下所有线程及其 psr（当前运行 CPU）
//     /*
//     -T:打印该进程下所有线程
//     psr:当前正在运行的CPU ID
//     grep <tid>:找出当前线程 ID 行
//     awk '{print $3}':提取第三列，也就是 CPU 编号
//     */
//     sprintf(qry_cmd, "ps -o pid,spid,psr -T -p %d | grep %d | tail -n 1 | awk '{print $3}'", getpid(), gettid());

//     FILE *fp = popen(qry_cmd, "r");                                       // 使用 popen 启动一个 shell 执行上面构造的命令，打开一个管道读取命令的标准输出。返回 FILE* 便于用 fgets() 读取输出

//     // 若失败则返回，不打印任何信息
//     if (fp == NULL)
//     {
//         return;
//     }

//     char cpu_id_str[200] = {0};                                           // 分配一个 200 字节的缓冲区来存储命令输出（CPU编号文本），并用0初始化
//     fgets(cpu_id_str, 80, fp);                                            // 从 fp （管道）读取最多 79 字节（加上结尾\0）到 cpu_id_str

//     fclose(fp);                                                           // 关闭管道

//     // 输出：atoi(cpu_id_str) 将从 ps 取得的 cpu id 字符串转换为整数并打印
//     printf("[%d] : current thread(%d@%d) is running on cpu(%d)\n", gettid(), gettid(), getpid(), atoi(cpu_id_str));
// }

// // 打印线程当前允许运行的 CPU 集合
// void print_thread_affinity()
// {
//     cpu_set_t cpu_mask;                                                   // 声明一个 cpu_set_t 类型变量，内核和库用来表示 CPU 集合（掩码）的结构

//     CPU_ZERO(&cpu_mask);                                                  // 宏，将 cpu_mask 清空（表示没有 CPU 被设置），为后续 sched_getaffinity 填充
//     // 在 cpu_mask 之前调用它，保证没有残留垃圾位

//     sched_getaffinity(gettid(), sizeof(cpu_mask), &cpu_mask);             // 调用系统函数获取指定线程（通过 TID）当前的 CPU 亲和性掩码，结果写入

//     printf("[%d] : current thread (%d@%d) can be running at cpu(", gettid(), gettid(), getpid());

//     int cpu_num = sysconf(_SC_NPROCESSORS_CONF);                          // 使用 sysconf 获取系统配置的处理器数量，这用于遍历掩码中可能的位

//     for (int i = 0; i < cpu_num; ++i)
//     {
//         if (CPU_ISSET(i, &cpu_mask))                                       // 判断 cpu_mask 中第i位是否被设置（即线程是否允许在 CPU i 上运行）
//         {
//             printf("%d", i);
//         }
//     }
//     printf(")\n");
// }

// // 把当前线程绑定到指定的 CPU 核心
// void bind_thread_to_cpu(int cpu_id)
// {
//     cpu_set_t cpu_mask;                                                   // 声明用于保存将要设置的亲和性掩码的变量
//     CPU_ZERO(&cpu_mask);                                                  // 清空掩码

//     CPU_SET(cpu_id, &cpu_mask);                                           // 在掩码中设置第 cpu_id 位，表示允许在该 CPU 上运行
//     print_thread_affinity();                                              // 打印绑定前的允许范围

//     // 打印绑定动作的日志
//     printf("[%d] : binding current thread(%d@%d) to cpu(%d)\n",gettid(),gettid(),getpid(),cpu_id);

//     sched_setaffinity(gettid(), sizeof(cpu_mask), &cpu_mask);                 // 调用 sched_setaffinity 进行绑定
//     print_thread_affinity();                                                  // 打印绑定后的允许范围
// }

// // 每个线程执行的任务函数
// void *thread_func(void *p_arg)
// {
//     printf("[%d] : ----------------\n",gettid());
//     printf("[%d] : setting cpu affinity for thread(%d@%d) ...\n",gettid(),gettid(),getpid());
//     int bind_cpu_id = *(int *)p_arg;
//     bind_thread_to_cpu(bind_cpu_id);                                      // 把当前线程绑定到指定的 CPU 核心上运行
//     printf("[%d] : ----------------\n\n",gettid());

//     sleep(1);

//     print_running_cpu();

//     while (1)
//     {
//         long loop = 4000000000;
//         while (loop--)
//             ;
//         sleep(0);
//         printf("[%d] : ----------------\n",gettid());
//         printf("[%d] : switched !\n",gettid());
//         print_running_cpu();
//     }
// }

// // 负责创建两个线程，线程1绑定 CPU0，线程2绑定 CPU3
// int main()
// {
//     int cpu_id_0 = 0;
//     int cpu_id_1 = 1;
//     int cpu_id_2 = 2;
//     int cpu_id_3 = 3;

//     pthread_t thrd_id_1, thrd_id_2;

//     pthread_create(&thrd_id_1, NULL, thread_func, &cpu_id_0);

//     sleep(1);
//     pthread_create(&thrd_id_2, NULL, thread_func, &cpu_id_3);

//     // 主线程空转防止退出
//     while (1)
//     {
//         sleep(1);
//     }
//     return 0;
// }