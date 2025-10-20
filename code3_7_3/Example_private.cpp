// private 子句
# include <stdio.h>
# include <assert.h>
# include <assert.h>

void priv_example3();

int main(){
    int i,j;
    int *ptr_i,*ptr_j;

    i = 1;
    j = 2;

    ptr_i = &i;
    ptr_j = &j;

    // firstprivate(j) 每个线程有自己独立的 j 副本，并且进入并行区时，j 的初值等于并行区原来的值 
    #pragma omp parallel private(i) firstprivate(j)
    {
        i = 3;
        j = j + 2;
        // assert 是一个宏，如果括号内语句为假，打印错误信息；真不做任何操作
        assert (*ptr_i == 1 && *ptr_j == 2);
    }
    assert(i == 1 && j == 2);

    priv_example3();
    return 0;
}

// 循环内部对 a 值的修改，只修改线程自己的副本
int a;                                           // 全局变量 a
void g(int k){
    a = k;                                       // 修改的是全局 a
}
void f(int n){
    int a = 0;
    #pragma omp parallel for private(a)
    for(int i = 1;i < n;i ++){
        a = i;                                   // 每个线程的私有副本
        g(a * 2);                                // g 使用的是全局变量 a,不是线程私有的 a
    }
}

void priv_example3(){
    int i,a;

    #pragma omp parallel private(a)
    {
        a = 1;
        #pragma omp parallel for private(a)
        for(i = 0;i < 10;i ++){
            a = 2;
        }
        assert(a == 1);
    }
}