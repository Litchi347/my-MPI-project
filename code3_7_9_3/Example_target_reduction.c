# include <stdio.h>

int f(int);
int g(int);

// // 外部函数
// int f(int i){
//     return i + 1;
// }
// int g(int i){
//     return i * 2;
// }

// // 隐式映射行为：当在 target 构造上对 sum1 或 sum2 使用 reduction 时，OpenMP 会自动把这些变量映射到设备上，并在设备执行结束后把值传回主机；
// // 保证了第一个 target 上计算出的结果，会在第二个上被继续使用，不会丢失
// int main(){
//     int sum1 = 0,sum2 = 0;
//     const int n = 100;

//     // target : offload 到设备（通常是 target）执行
//     // teams 在设备上创建若干 teams，每个 teams 包含若干执行单元
//     // distribute 把随后的迭代分配到各个 teams 
//     #pragma omp target teams distribute reduction(+:sum1)
//     for(int i = 0;i < n;i ++){
//         sum1 += f(i);
//     }

//     // 再次 offload ，使用上一个结果 sum1
//     #pragma omp target teams distribute reduction(+:sum2)
//     for(int i = 0;i < n;i ++){
//         sum2 += g(i) * sum1;
//     }
//     printf("sum1 = %d,sum2 = %d\n",sum1,sum2);
//     return 0;
//  }

int main()
{
    int sum1 = 0,sum2 = 0;
    const int n = 100;

    // 数据保留在设备上：
    // target data 创建一个设备数据区间，sum1 和 sum2 被映射到设备，在这个区域内的所有 target 构造会使用设备副本，不会每次进行重新拷贝
    // 保证了 sum1 和 sum2 在设备上的值与主机同步
    #pragma omp target data map(sum1,sum2)
    {
        #pragma omp target teams distribute reduction(+:sum1)
        for(int i = 0;i < n;i ++){
            sum1 += f(i);                        // 计算完成后，sum1 的设备副本已更新
        }

        // 显式告诉 OpenMP 使用设备上的 sum1 ，若不写，默认会当成 firstpriavte ，设备上不会保留之前的计算结果
        #pragma omp target teams distribute map(sum1) reduction(+:sum2)
        for(int i = 0;i < n;i ++){
            sum2 += g(i) * sum1;
        }
    }
    printf("sum1= %d,sum2 = %d\n",sum1,sum2);
    return 0;
}

int f(int res){return res * 2;}
int g(int res){return res * 3;}
