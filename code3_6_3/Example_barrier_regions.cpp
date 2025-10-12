// 使用 barrier 实现屏障同步：同一并行区域所有线程必须执行到 barrier 并完成之前的任务才能继续向后执行
# include <iostream>
# include <omp.h>

void work(int n){
    printf("%d is working in sub1 %d!\n",omp_get_thread_num(),n);
}

void sub3(int n){
    work(n);
    #pragma omp barrier
    work(n);
}

void sub2(int k){
    #pragma omp parallel shared(k)
    {
        sub3(k);
    }
}

void sub1(int n){
    int i;
    #pragma omp parallel num_threads(8) private(i) shared(n)
    {
        #pragma omp for
        {
            for(i = 0;i < n;i++){
                // 这里面也有并行区域。一般情况下，OpenMP不做循环嵌套，内部的 #pragma omp parallel 会被忽略，只用当前线程执行sub3(k)
                // for 循环被分配给多个线程执行，每个线程在执行时候进入 sub2 函数中，执行 work()
                sub2(i);
            }
        }
    }
}

int main()
{
    sub1(2);
    // sub2(2);
    // sub3(2);
    return 0;
}