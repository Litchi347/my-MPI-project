#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 10000                                            // 外层循环规模（任务数）
#define M 5000                                             // 内层数组规模上限

// 非平衡工作负载：每个任务的计算量不同
int main(){
    double start, end;                                     // 计时变量
    double serial_time;                                    // 串行执行时间
    double parallel_time;                                  // 并行执行时间

    // 串行执行
    start = omp_get_wtime();
    for (int i = 0; i < N; i++) {
        int inner_size = (i % M) + 1;                      // 非平衡任务：每次循环规模不同
        double *a = (double*)malloc(inner_size * sizeof(double));
        double *b = (double*)malloc(inner_size * sizeof(double));
        double *c = (double*)malloc(inner_size * sizeof(double));
        for(int j = 0;j < inner_size;j++){
            a[j] = j * 0.5;
            b[j] = j * 0.25;
            c[j] = a[j] + b[j];
        }
        free(a);
        free(b);
        free(c);
    }
    end = omp_get_wtime();
    serial_time = end - start;
    printf("串行时间：%.6f 秒\n",serial_time);
    printf("\n");

    // 并行执行
    int thread_counts[] = {1,2,4,8};
    int num_configs = sizeof(thread_counts) / sizeof(thread_counts[0]);
    const char* schedules[] = {"static","dynamic","guided"};
    int num_schedules = 3;

    for(int t = 0;t < num_configs;t++){                    // 设置线程数
        int num_threads = thread_counts[t];
        for(int s = 0;s < num_schedules;s++){              // 设置调度策略
            omp_set_num_threads(num_threads);

            omp_sched_t sched_type;
            switch (s)
            {
                case 0:
                    sched_type = omp_sched_static;
                    break;
                case 1:
                    sched_type = omp_sched_dynamic;
                    break;
                case 2:
                    sched_type = omp_sched_guided;
                    break;
            }
            omp_set_schedule(sched_type,1);
            start = omp_get_wtime();
            #pragma omp parallel
            {
                #pragma omp for schedule(runtime)
                for(int i = 0;i < N;i ++){
                    int inner_size = (i % M) + 1;
                    double *a = (double*)malloc(inner_size * sizeof(double));
                    double *b = (double*)malloc(inner_size * sizeof(double));
                    double *c = (double*)malloc(inner_size * sizeof(double));
                    for(int j = 0;j < inner_size;j ++){
                        a[j] = j * 0.5;
                        b[j] = j * 0.25;
                        c[j] = a[j] + b[j];
                    }
                    free(a);
                    free(b);
                    free(c);
                }
            }
            end = omp_get_wtime();
            parallel_time = end - start;

            double speedup = serial_time / parallel_time;
            double efficiency = speedup / num_threads;

            printf("[线程数 %2d] %-7s 运行时间：%.6f 秒,加速比:%.2f,效率:%.af\n",num_threads,schedules[s],parallel_time,speedup,efficiency);
        }
        printf("\n");
    }   
    return 0;
}