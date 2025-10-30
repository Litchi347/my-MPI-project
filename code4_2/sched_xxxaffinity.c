// // 通过 sched_xxxaffinity 接口实现通过编程绑定 CPU 核心，该接口既能绑定进程又能绑定线程
// # define _GNU_SOURCE
// # include <stdio.h>
// # include <stdlib.h>
// # include <unistd.h>
// # include <pthread.h>
// # include <sched.h>

// void print_running_cpu(){
//     char qry_cmd[1024] = { 0 };
//     sprintf(qry_cmd, "ps -o pid,spid,psr -T -p %d | grep %d | tail -n 1 | awk '{print $3}'",getpid(),gettid());
//     FILE *fp=popen(qry_cmd,"r");
//     if(fp == NULL){
//         return ;
//     }
//     char cpu_id_str[200] = { 0 };
//     fgets(cpu_id_str,80,fp);

//     fclose(fp);
//     printf("[%d] : current thread(%d@%d) is running on cpu(%d)\n",gettid(),gettid(),getpid(),atoi(cpu_id_str));
// }

// void print_thread_affinity(){
//     cpu_set_t cpu_mask;

//     CPU_ZERO(&cpu_mask);
//     sched_getaffinity(gettid(),sizeof(cpu_mask),&cpu_mask);

//     printf("[%d] : current thread (%d@%d) can be running at cpu(",gettid(),gettid(),getpid());
//     int cpu_num = sysconf(_SC_NPROCESSORS_CONF);
//     for(int i = 0;i < cpu_num; ++i){
//         if(CPU_ISSET(i,&cpu_mask)){
//             printf("%d",i);
//         }
//     }
//     printf(")\n");
// }

// void bind_thread_to_cpu(int cpu_id){
//     cpu_set_t cpu_mask;
//     CPU_ZERO(&cpu_mask);

//     CPU_SET(cpu_id,&cpu_mask);
//     print_thread_affinity();
//     printf("[%d] : binding current thread(%d@%d) to cpu(%d)\n",gettid(),gettid(),getpid(),cpu_id);
//     sched_setaffinity(gettid(),sizeof(cpu_mask),&cpu_mask);
//     print_thread_affinity();
// }

// void* thread_func(void* p_arg){
//     printf("[%d] : ----------------\n",gettid());
//     printf("[%d] : setting cpu affinity for thread(%d@%d) ...\n",gettid(),gettid(),getpid());
//     int bind_cpu_id = *(int *)p_arg;
//     bind_thread_to_cpu(bind_cpu_id);
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

// int main(){
//     int cpu_id_0 = 0;
//     int cpu_id_1 = 1;
//     int cpu_id_2 = 2;
//     int cpu_id_3 = 3;

//     pthread_t thrd_id_1,thrd_id_2;

//     pthread_create(&thrd_id_1,NULL,thread_func,&cpu_id_1);

//     sleep(1);

//     pthread_create(&thrd_id_2,NULL,thread_func,&cpu_id_3);

//     while(1){
//         sleep(1);
//     }
//     return 0;

// }