// 以下两个示例展示了如何通过两种不同方式使用 taskloop reduction 实现数组归约
# include <stdio.h>
# define N 100

int array_sum(int n,int *v){
    int i;
    int res = 0;

    // // 将一个循环拆分成多个任务，并对变量 res 进行归约操作
    // #pragma omp taskloop reduction(+:res)
    // for(i = 0;i < n; ++i){
    //     res += v[i];
    // }

    // 定义一个任务归约作用域，所有在其中声明 inreduction(+:res) 的任务，都会把自己的部分结果累加到 res
    #pragma omp taskgroup task_reduction(+:res)
    {
        if (n > 0){
            #pragma omp task in_reduction(+:res)
            res = res + v[0];

            // nogroup 表示不要隐式生成 taskgroup ，外层已有
            #pragma omp taskloop in_reduction(+:res) nogroup
            for(i = 1;i<n;i++){
                res += v[i];
            }
        }
    }
    return res;
}

// int main(int arvc,char *argv[])
// {
//     int n = 10;
//     int v[10] = {1,2,3,4,5,6,7,8,9,10};

//     #pragma omp parallel

//     // 只有一个线程执行，其他线程跳过
//     #pragma omp single
//     {
//         int res = array_sum(n,v);
//         printf("The result is %d\n",res);
//     }
//     return 0;
// }


// task 和 taskloop
// reduction 和 in_reduction
int main(){
    int a[N],asum = 0;

    // 初始化
    for(int i = 0;i < N;i++){
        a[i] = i;
    }

    // taskloop reductions
    #pragma omp parallel master
    #pragma omp taskloop reduction(+:asum)                   // taskloop 1
                                                             // 主线程将循环分成若干个任务（task 只表创建一个任务）
    for(int i = 0;i < N;i ++){
        asum += a[i];
    }

    #pragma omp parallel reduction(task,+:asum)              // 所有带 in_reduction(+:asum) 的任务、taskloop 的局部和都属于这个归约集合
    {
        #pragma omp master                                   // 第一个部分，没有 in_reduction(+:asum) ，直接访问主线程的 asum

        #pragma omp task in_reduction(+:asum)                // task 2
        for(int i = 0;i < N;i++){
            asum += a[i];
        }

        #pragma omp master taskloop in_reduction(+:asum)     // taskloop 2
                                                             // 属于上层的 reduction(task,+:asum)，每个任务计算自己的局部和，最终在并行区域结束时归约到主线程的 asum
        for(int i = 0; i < N;i ++){
            asum += a[i];
        }
    }

    // taskloop simd reductions
    #pragma omp parallel master
    #pragma omp taskloop simd reduction(+:asum)              // taskloop simd 3
                                                             // 加入 simd ，每个任务内部的循环使用向量化
    for(int i = 0;i < N; i++){
        asum += a[i];
    }

    #pragma omp parallel reduction(task,+:asum)
    {
        #pragma omp master                                   // 主线程创建任务
        #pragma omp task in_reduction(+:asum)                // task 4 ，其他线程分配任务并参加归约
        for(int i = 0;i < N;i ++){
            asum += a[i];
        }
        #pragma omp master taskloop simd in_reduction(+:asum)// taskloop simd 4
        for(int i = 0;i < N;i ++){
            asum += a[i];
        }
    }
    printf("asum=%d\n",asum);
}