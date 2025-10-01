#include <stdio.h>
#include <omp.h>

void sub0(float *a){
    // 使用 collapse(2)，把 k 和 j 两重循环展平后切分给线程
    #pragma omp parallel for collapse(2)
    for(int k = 1;k <= 2;k += 1){
        for(int j = 1;j <= 3;j += 1){
            for(int i = 1;i <= 2;i += 1){
                printf("Thread %d -> (k=%d, j=%d, i=%d)\n",omp_get_thread_num(), k, j, i);
            }
        }
    }
}

// example2 
void sub1 (){
    int jlast,klast;
    #pragma omp parallel
    {
        // 使用 lastprivate 记录最后一次迭代的数据
        #pragma omp for collapse(2) lastprivate(jlast,klast)
        for(int k = 1;k <= 2;k ++)
            for(int j = 1;j <= 3;j ++){
                printf("Thread %d -> (k=%d, j=%d)\n",omp_get_thread_num(), k, j);
                jlast = j;
                klast = k;
            }
        #pragma omp single
        printf("%d %d\n",klast,jlast);
    }
}

// example3 给出 order 子句用来按序输出
void sub2(){
    #pragma omp parallel num_threads(2)
    {
        // 将下面两层for迭代空间平展成一维空间进行分配，并按序分配，每个线程分配3个迭代
        #pragma omp for collapse(2) ordered private(j,k) schedule(static,3)
        for(int k = 1;k <= 3;k++){
            for(int j = 1;j <= 2;j++){
                #pragma omp ordered
                printf("%d %d %d\n",omp_get_thread_num(),k,j);
            }
        }
    }
}


int main(){
    // float a[10];
    // sub0(a);

    sub1();
    return 0;
}