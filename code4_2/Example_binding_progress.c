// # include <stdio.h>
// # include <stdlib.h>
// # include <unistd.h>                        // 引入 POSIX 函数（ gretpid()、sleep()、popen() 等）

// // 用于查询并打印当前进程运行所在的 CPU 编号
// void print_running_cpu(){
//     char qry_cmd[1024] = {0};               // 分配缓冲区并初始化为 0 ，用来存放要执行的 shell 命令

//     // 拼接出完整的 shell 命令，取出 ps 输出中显示的 CPU 列并打印该列值
//     // 列出进程号为 %d 的进程和它当前的运行 CPU
//     // tail -n 1:去掉表头，只保留数据行
//     // awk '{print $2}'只输出第二列（CPU核心号）
//     // 执行结果为一个整数，表示当前运行在哪一个 CPU 核心上
//     sprintf(qry_cmd,"ps -o pid,psr -p %d | tail -n 1 | awk '{print $2}'",getpid());

//     FILE *fp = popen(qry_cmd,"r");          // 使用 popen() 启动一个子进程执行上面的命令并建立一个管道读取命令输出

//     if (fp == NULL){                        // 如果 popen 失败，直接返回。确保命令执行正常
//         return;
//     }

//     char cpu_id_str[200] = {0};             // 分配 200 字节缓冲区用于读取 awk 地输出字符串
//     fgets(cpu_id_str,80,fp);                // 从命令输出中读入 CPU 核心号字符串存入 cpu_id_str，
//     fclose(fp);                             // 关闭管道

//     // atoi 把字符串转成整数，打印结果
//     printf("current process %d is running on cpu(%d)\n",getpid(),atoi(cpu_id_str));
// }

// // 演示一个普通进程如何被操作系统调度到不同 CPU 核心上运行
// int main(){
//     print_running_cpu();                    // 程序启动时查看当前开始时进程在哪个 CPU 上运行

//     while (1)                               // 进入无限循环，程序会不断做重负载并周期性检查运行的 CPU
//     {
//         long loop = 4000000000;
//         while(loop--)
//             ;                               // 循环执行空语句：CPU占用率达 100% ，忙等，使程序持续占满一个 CPU 核心以便观察该线程在哪个核上运行
//         sleep(0);                           // 主动放弃 cpu ，操作系统有机会切换核心
//         printf("----------------\n");
//         printf("switched !\n");
//         print_running_cpu();
//     }
//     return 0;
    
// }