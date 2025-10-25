// copyin 子句：在各线程变量副本进入并行区域时初始化，初始值为主线程的值
# include <stdlib.h>

float* work;
int size;
float tol;

#pragma omp threadprivate(work,size,tol)

void build(){
    work = (float*)malloc(sizeof(float)*size);
    for(int i = 0;i < size;++i){
        work[i] = tol;
    }
}

void copyin_example(float t,int n){
    // 主线程给全局变量赋初值
    tol = t;
    size = n;

    // 在各线程进入并行区域时，把主线程中 threadprivate 变量的复制给每个线程的对应副本
    #pragma omp parallel copyin(tol,size)
    {
        build();
    }
}