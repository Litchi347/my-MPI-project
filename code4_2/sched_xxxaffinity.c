// 通过 sched_xxxaffinity 接口实现通过编程绑定 CPU 核心，该接口既能绑定进程又能绑定线程
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

// 打印当前线程正在运行的 CPU 编号
void print_running_cpu()
{
    char qry_cmd[1024] = {0};
    sprintf(qry_cmd, "ps -o pid,spid,psr -T -p %d | grep %d | tail -n 1 | awk '{print $3}'", getpid(), gettid());
    FILE *fp = popen(qry_cmd, "r");
    if (fp == NULL)
    {
        return;
    }
    char cpu_id_str[200] = {0};
    fgets(cpu_id_str, 80, fp);

    fclose(fp);
    printf("[%d] : current thread(%d@%d) is running on cpu(%d)\n", gettid(), gettid(), getpid(), atoi(cpu_id_str));
}

// 打印当前线程可运行的 CPU 集合
void print_thread_affinity()
{
    cpu_set_t cpu_mask; // 位掩码，用来表示一组 CPU 核心

    CPU_ZERO(&cpu_mask); // CPU_ZERO 是一个宏，用来把 cpumask 清空，把所有位设置为 0
    // 在 cpu_mask 之前调用它，保证没有残留垃圾位

    sched_getaffinity(gettid(), sizeof(cpu_mask), &cpu_mask); // 读取 pid 的 CPU 亲和性，把结果写入 mask
    // 后续用 cpu_isset 检查每个核是否允许

    printf("[%d] : current thread (%d@%d) can be running at cpu(", gettid(), gettid(), getpid());

    int cpu_num = sysconf(_SC_NPROCESSORS_CONF); // 查询系统上配置的 CPU 数量

    for (int i = 0; i < cpu_num; ++i)
    {
        if (CPU_ISSET(i, &cpu_mask))
        { // 检查某个 CPU 是否在允许列表中
            printf("%d", i);
        }
    }
    printf(")\n");
}

// 把当前线程绑定到指定的 CPU 核心
void bind_thread_to_cpu(int cpu_id)
{
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);

    CPU_SET(cpu_id, &cpu_mask); // 设置指定 CPU
    print_thread_affinity();    // 打印绑定前的允许范围
    // printf("[%d] : binding current thread(%d@%d) to cpu(%d)\n",gettid(),gettid(),getpid(),cpu_id);
    sched_setaffinity(gettid(), sizeof(cpu_mask), &cpu_mask); // 调用 sched_setaffinity 进行绑定
    print_thread_affinity();                                  // 打印绑定后的允许范围
}

// 线程入口函数，新线程启动后从这里执行
void *thread_func(void *p_arg)
{
    // printf("[%d] : ----------------\n",gettid());
    // printf("[%d] : setting cpu affinity for thread(%d@%d) ...\n",gettid(),gettid(),getpid());
    int bind_cpu_id = *(int *)p_arg;
    bind_thread_to_cpu(bind_cpu_id); // 把当前线程绑定到指定的 CPU 核心上运行
    // printf("[%d] : ----------------\n\n",gettid());

    sleep(1);

    print_running_cpu();

    while (1)
    {
        long loop = 4000000000;
        while (loop--)
            ;
        sleep(0);
        // printf("[%d] : ----------------\n",gettid());
        // printf("[%d] : switched !\n",gettid());
        print_running_cpu();
    }
}

int main()
{
    int cpu_id_0 = 0;
    int cpu_id_1 = 1;
    int cpu_id_2 = 2;
    int cpu_id_3 = 3;

    pthread_t thrd_id_1, thrd_id_2;

    pthread_create(&thrd_id_1, NULL, thread_func, &cpu_id_0);

    sleep(1);
    pthread_create(&thrd_id_2, NULL, thread_func, &cpu_id_3);

    while (1)
    {
        sleep(1);
    }
    return 0;
}