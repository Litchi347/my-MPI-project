// 当 #pragma omp flush 不带变量列表时，哪些共享变量会被同步、哪些不会
// #pragma omp flush 只会作用于当前线程作用域中“可访问”且为 shared 的变量
# include <stdio.h>
# include <omp.h>

// p 是指向 int 的指针
// *p 是访问指针 p 指向的内容，相当于访问 x
int x,*p = &x;

// 显式 flush
void f1(int *q){
    *q = 1;
    #pragma omp flush
    // x,p和*q都被刷新：x 和 *p 是 shared 并且在此函数中可以访问，q 本身不是 shared，所以不被刷新
    // *q 作为局部变量没有被线程之间共享
}

// 隐式 flush
void f2(int *q){
    #pragma omp barrier
    *q = 2;
    #pragma omp barrier          // barrier 隐含一个flush
}

int g(int n){
    int i = 1,j,sum = 0;
    *p = 1;

    #pragma omp parallel reduction(+: sum) num_threads(4)
    {
        f1(&j);                  // i,n,sum 在 f1 中不可访问，没有被 flush; j 被 flush
        sum += j;
        #pragma omp single
        {
            printf("Result of f1 = %d\n",sum);
        }
        
        f2(&j);                  // i,n,sum 在 f1 中不可访问，没有被 flush; j 被 flush
        // sum += i + j + *p + n;
        sum += j;
        #pragma omp single
        {
            printf("Result of f2 = %d\n",sum);
        }
    }
    return sum;
}

int main(){
    int result = g(7);
    // printf("Result = %d\n",result);
    return 0;
}