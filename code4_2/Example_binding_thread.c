// #define _GNU_SOURCE
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <pthread.h>

// void print_running_cpu()
// {
//     char qry_cmd[1024] = {0};
//     sprintf(qry_cmd, "ps -o pid,spid,psr -T -p %d | grep %d | tail -n 1 | awk '{print $3}'", getpid(), gettid());
//     FILE *fp = popen(qry_cmd, "r");
//     if (fp == NULL)
//     {
//         return;
//     }
//     char cpu_id_str[200] = {0};
//     fgets(cpu_id_str, 80, fp);
//     fclose(fp);

//     printf("[%d] : current thread(%d@%d) is running on cpu(%d)\n", gettid(), gettid(), getpid(), atoi(cpu_id_str));
// }

// void *thread_func(void *p_arg)
// {
//     print_running_cpu();
//     while (1)
//     {
//         long loop = 4000000000;
//         while (loop--)
//         {
//             ;
//         }
//         sleep(0);

//         printf("[%d] : ----------------\n", gettid());
//         printf("[%d] : switched !\n", gettid());
//         print_running_cpu();
//     }
// }

// int main()
// {
//     print_running_cpu();
//     pthread_t thr_id_1, thr_id_2;

//     pthread_create(&thr_id_1, NULL, thread_func, NULL); // 创建一个新线程，从 thread_func() 开始运行，线程号保存在 thrd_id1 中
//     pthread_create(&thr_id_2, NULL, thread_func, NULL);

//     while (1)
//     {
//         sleep(1);
//     }
//     return 0;
// }