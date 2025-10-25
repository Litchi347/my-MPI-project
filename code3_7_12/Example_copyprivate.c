# include <stdio.h>
# include <stdlib.h>
# include <omp.h>

float x,y;
#pragma omp threadprivate(x,y)

void init(float a,float b){
    #pragma omp single copyprivate(a,b,x,y)
    {
        scanf("%f %f %f %f",&a,&b,&x,&y);
    }
}

// 若主线程不支持 copyprivate 子句，那么就无法将这些变量的值广播到其他线程。
// 但可以广播变量所在的地址
float read_next(){
    float * tmp;
    float return_val;

    #pragma omp single copyprivate(tmp)
    {
        tmp = (float *)malloc(sizeof(float));
    }
    #pragma omp master
    {
        scanf("%f",tmp);
    }

    #pragma omp barrier
    return_val = *tmp;
    #pragma omp barrier

    #pragma omp single nowait
    {
        free(tmp);
    }
    return return_val;
}

omp_lock_t *new_lock()
{
    omp_lock_t *lock_ptr;

    // 在锁的数量要在并行区内动态决定时，可以用 copyprivate 子句把 single 线程分配的锁变量广播给其他线程，从而实现所有线程共享同一批锁，而不需要在进入并行区前确定锁的数量
    #pragma omp single copyprivate(lock_ptr)
    {
        lock_ptr = (omp_lock_t *)malloc(sizeof(omp_lock_t));
        omp_init_lock(lock_ptr);
    }
    return lock_ptr;
}