// 使用 nowait 子句进行优化：在两个 for 循环中间默认有一个同步点保证所有线程必须等到上一步操作完成之后才能继续向下执行
// 如果当前操作与上一个操作没有依赖关系，那么实际上没必要强制在上个操作结束之后把所有线程都对齐
# include<math.h>

void nowait_example(int n,int m,float *a,float *b,float *y,float *z){
    int i;
    #pragma omp parallel
    {
        #pragma omp for nowait
            for(int i = 1;i < n;i++){
                b[i] = (a[i] + a[i-1]) / 2.0;
            }
        #pragma omp for nowait
            for(i = 0 ;i < m;i++){
                y[i] = sqrt(z[i]);
            }
    }
}

// 一般来说，nowait只能在两个循环没有依赖关系时使用，有一种特殊情况如下：静态调度
// 当schedule(static)时，OpenMP会固定地把相同的迭代号分配给同一个线程，
// 这种情况下，在第二个循环中，同一个线程继续处理相同编号的迭代，即使没有barrier，线程也不会去读别的线程的数据
#include <math.h>

void nowait_example2(int n,float *a,float *b,float *c,float *y,float *z){
    int i;
    #pragma omp parallel
    {
        #pragma omp for schedule(static) nowait
            for(int i = 1;i < n;i++){
                c[i] = (a[i] + b[i]) / 2.0f;
            }
        #pragma omp for schedule(static) nowait
            for(i = 0 ;i < n;i++){
                z[i] = sqrtf(c[i]);
            }
        #pragma omp for schedule(static) nowait
            for(i = 1 ;i <= n;i++){
                y[i] = z[i - 1] + a[i];
            }
    }
}

