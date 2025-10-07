# include <stdio.h>

#define N 10

// extern float average(float,float,float);
float average(float a,float b,float c){
    return (a+b+c) / 3.0f;
}

void master_example(float* x,float* xold,int n,float tol)                 // n 迭代次数，tol 误差的允许范围
{
    int c = 0;                                                            // 记录迭代次数
    int toobig;                                                           // 超过误差范围的值的个数
    float y;                                                              // 临时变量，存储之前x数组中的值
    #pragma omp parallel
    {
        do{
            // 并行复制数组
            #pragma omp for private(i)
            for(int i = 1;i < n-1;++ i){
                xold[i] = x[i];
            }

            // 单线程重置计数器
            #pragma omp single
            {
                toobig = 0;
            }

            // 并行迭代计算
            #pragma omp for private(i,y,error) reduction(+:toobig)        // reduction(+:toobig) 表示每个线程在自己私有的变量中更改toobig的值，避免竞态条件；循环结束后，Openmp将所有线程的toobog加在一起
            for(int i = 1; i < n-1; ++i){
                y = x[i];
                x[i] = average(xold[i-1],x[i],xold[i+1]);
                if(y - x[i] > tol || y - x[i] < -tol){
                    ++toobig;
                }
            }

            // 只有master(Thread 0)执行这段代码，其他线程直接跳过
            #pragma omp master
            {
                ++c;
                printf("iteration %d,toobig=%d\n",c,toobig);
            }
        }while(toobig > 0);
    }
}

int main(){
    float x[N], xold[N];

    // 初始化数组
    for (int i = 0; i < N; ++i) {
        x[i] = (float)i;
        xold[i] = 0.0f;
    }

    master_example(x, xold, N, 0.01f);
    return 0;
}