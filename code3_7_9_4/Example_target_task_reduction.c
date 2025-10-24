// 如何把设备端任务(target task)纳入任务归约(task reduction)中

// 让 section 里的显式任务参与归约

// # include <stdio.h>

// void device_compute(int *);
// void host_compute(int *);

// // device_compute 可以在 target 区域被调用
// # pragma omp declare target to(device_compute)

// int main()
// {
//     int sum = 0;
//     #pragma omp parallel master
//     #pragma omp taskgroup task_reduction(+:sum)
//     {
//         // 设备端修改的 sum 值会被自动归约
//         #pragma omp target in_reduction(+:sum) nowait
//             device_compute(&sum);
//         #pragma omp task in_reduction(+:sum)
//             host_compute(&sum);
//     }
//     printf("sum = %d\n",sum);
//     return 0;
// }

// void device_compute(int *sum){*sum = 1;}
// void host_compute(int *sum){*sum = 1;}



// # include <stdio.h>

// extern void device_compute(int *);
// extern void host_compute(int *);

// #pragma omp declare target to(device_compute)

// int main(){
//     int sum = 0;

//     // section 和 task 的区别：
//     // section 是固定、静态、简单的并行划分
//     // task 是灵活、动态、可依赖的任务系统
//     #pragma omp parallel sections reduction(task,+:sum)
//     {
//         // 第一段 section 由线程 0 执行，在 GPU 上
//         #pragma omp section
//         {
//             #pragma omp target in_reduction(+:sum)
//             device_compute(&sum);
//         }

//         // 第二段在线程 1 上执行，在 CPU 上
//         #pragma omp section
//         {
//             host_compute(&sum);
//         }
//     }
//     printf("sum = %d\n",sum);
//     return 0;
// }

// void device_compute(int *sum){*sum = 1;}
// void host_compute(int *sum){*sum = 1;}


# include <stdio.h>

extern void device_compute(int *);
extern void host_compute(int *);

#pragma omp declare target to(device_compute)

int main(){
    int sum = 0;
    // 直接在 parallel master 上使用 reduction 
    // 内部一个 target 任务，一个主线程任务（隐式任务
    // 两者同样参加 sum 的任务归约
    #pragma omp parallel master reduction(task,+:sum)
    {
        #pragma omp target in_reduction(+:sum) nowait
        device_compute(&sum);

        host_compute(&sum);
    }
    printf("sum = %d\n",sum);
    return 0;
}

void device_compute(int *sum){*sum = 1;}
void host_compute(int *sum){*sum = 1;}