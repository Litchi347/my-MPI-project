#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

double * alloc_init_B(double *A,int N){
    double *B = (double*)malloc(N* sizeof(double));
    if(B == NULL){
        return NULL;
    }
    // 初始化
    for(int i = 0;i < N;i++){
        B[i] = A[i] * 2.0;
    }
    return B;
}

void compute_on_B(double *B,int N){
    // 计算
    for(int i = 0;i < N;i++){
        B[i] = B[i] + 1.0;
    }
}

void task_affinity(double *A,int N){
    double * B;

    // #pragma omp task 创建一个显式任务并加入任务池，由OpenMP运行时系统调度执行
    // 依赖输出：产生变量B的值，后续依赖B的任务必须等待这个任务完成
    // shared(B)：所有任务共享同一个B指针，不使用firstprivate复制B的初始值
    // affinity 子句是一个提示，不是强制命令：该任务最好在变量物理存储的位置附近执行，优化数据局部性
    #pragma omp task depend(out:B) shared(B) affinity(A[0:N])     // 必须指定范围
    {
        B = alloc_init_B(A,N);
    }

    // shared(B) affinity(A[0:N])：使用最新的B值
    #pragma omp task depend(in:B) shared(B) affinity(A[0:N])
    {
        compute_on_B(B,N);
    }

    #pragma omp taskwait
}

int main(){
    const int N = 100;
    double *A = (double*)malloc(N* sizeof(double));
    task_affinity(A,N);
    return 0;
}