// 以下两个示例展示了如何通过两种不同方式使用taskloop reduction实现数组归约
// 第一个示例将 reduction 子句应用于 taskloop 结构体。任务归约分为两个组成部分：由taskgroup区域定义的归约作用域，以及参与归约的任务
// 本示例中，reduction 子句同时定义了两者语义：
// 首先指明与 taskloop 结构体关联的隐式 taskgroup 区域作为归约作用域；
// 其次将 taskloop 创建的所有任务定义为归约参与者
// 关于第一个特性需特别注意，若向taskloop结构体添加 nogroup 子句将导致代码非合规——本质上是由于存在一组参与未定义归约的任务
# include <stdio.h>

int array_sum(int n,int *v){
    int i;
    int res = 0;

    // // 将一个循环拆分成多个任务，并对变量 res 进行归约操作
    // #pragma omp taskloop reduction(+:res)
    // for(i = 0;i < n; ++i){
    //     res += v[i];
    // }

    // 定义一个任务归约作用域，所有在其中声明 inreduction(+:res) 的任务，都会把自己的部分结果累加到 res
    #pragma omp taskgroup task_reduction(+:res)
    {
        if (n > 0){
            #pragma omp task in_reduction(+:res)
            res = res + v[0];

            // nogroup 表示不要隐式生成 taskgroup ，外层已有
            #pragma omp taskloop in_reduction(+:res) nogroup
            for(i = 1;i<n;i++){
                res += v[i];
            }
        }
    }
    return res;
}

int main(int arvc,char *argv[])
{
    int n = 10;
    int v[10] = {1,2,3,4,5,6,7,8,9,10};

    #pragma omp parallel

    // 只有一个线程执行，其他线程跳过
    #pragma omp single
    {
        int res = array_sum(n,v);
        printf("The result is %d\n",res);
    }
    return 0;
}