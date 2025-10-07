# include <iostream>
# include <omp.h>

using namespace std;

void work(){
    int thread_id = omp_get_thread_num();                      // 获取当前线程编号
    int num_threads = omp_get_num_threads();                   // 获取总线程数

    #ifdef _GNU_SOURCE
        #include<sched.h>                                      // 包含处理CPU调度相关功能的头文件
        int cpu_id = sched_getcpu();                           // 获取当前线程所在的CPU编号
        cout << "Thread " << thread_id << " of " << num_threads << " running on CPU " << cpu_id << endl;
    #else
        cout >> "Thread " << thread_id " of " << num_threads << endl;
    #endif

}

int main(){
    // #pragma omp parallel proc_bind(spread) num_threads(4)      // 线程会被均匀地分配到多个处理器上
    // #pragma omp parallel num_threads(25) proc_bind(spread)
    #pragma omp parallel proc_bind(close) num_threads(4)
    {
        work();
    }
    return 0;
}