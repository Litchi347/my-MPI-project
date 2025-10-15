# include <stdio.h>
# include <omp.h>

float foo(int i){
    return (float)i * 1.0f;
}

float bar(float a,float b){
    return a + b * 0.5f;
}

float baz(float b){
    return b * 2.0f;
}

void work(int N,float *A,float *B,float *C)
{
    // 循环中存在一级依赖网络，单变量依赖
    #pragma omp for ordered(1)
    for(int i = 1;i < N;i ++){
        A[i] = foo(i);

        // sink 依赖：表示该 ordered 区块要等待 i-1 的 source 执行完
        #pragma omp ordered depend(sink: i-1)
            B[i] = bar(A[i],B[i-1]);
            printf("Thread %d computes B[%d] = %.2f\n",omp_get_thread_num(),i,B[i]);

        // source 依赖：表示当前迭代 i 的结果可以被后续 i+1 使用
        #pragma omp ordered depend(source)
            C[i] = baz(B[i]);
            printf("Thread %d computes C[%d] = %.2f\n",omp_get_thread_num(),i,C[i]);
    }
}

int main(){
    int N = 10;
    float A[10],B[10],C[10];

    B[0] = 1.0f;
    work(N,A,B,C);

    printf("\nFinal result:\n");
    for(int i = 1;i < N;i ++){
        printf("C[%d] = %.2f\n",i,C[i]);
    }

    return 0;
}