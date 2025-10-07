// 嵌套并行
# include <stdio.h>
# include <omp.h>

int main(void){
    // omp_set_nested(1);           // 老接口：启用嵌套并行
    omp_set_max_active_levels(2);   // 设置最大并行嵌套层数为2层
    omp_set_dynamic(0);             // 禁止动态线程调整，确保运行时不会自动调整线程数量，保证固定线程数

    // 外层并行区域
    #pragma omp parallel
    {
        #pragma omp single          // 只让并行区中一个线程执行代码块，其他线程在结束处默认同步等待
        {
            // 此时还在外层并行区域内，返回外层线程数
            printf("Inner:num_thds=%d\n",omp_get_num_threads());
        }

        #pragma omp barrier         // 确保所有外层线程都完成了第一个single区域，避免某些线程已经开始内层并行而其他线程还在修改
        // omp_set_nested(0);       // 禁用嵌套并行：内层parallel被序列化（单线程执行）
        
        // 内层并行区域
        #pragma omp parallel
        {
            #pragma omp single
            {
                printf("Inner:num_thrds=%d\n",omp_get_num_threads());
            }
        }
        #pragma omp barrier
        #pragma omp single
        {
            printf("Outer:num_thrds=%d\n",omp_get_num_threads());
        }
    }
    return 0;
}