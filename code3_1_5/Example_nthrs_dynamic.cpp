// num_threads子句可以显式指定并行区域要开多少线程
// omp_set_dynamic函数用来控制OpenMP运行时是否动态调整线程数（禁止OpenMP私自改动，尽量按需求分配线程）

# include<omp.h>

int main(){
    omp_set_dynamic(0);                    // 禁止动态调整线程数
    #pragma omp parallel num_threads(10)
    {
        // do work here
    }
    return 0;
}