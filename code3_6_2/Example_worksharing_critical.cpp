// 解释 critical 临界区 与 工作共享结构 之间的嵌套关系
# include <stdio.h>
# include <omp.h>

void critical_work()
{
    int i = 1;
    // 外层并行 sections
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            // 临界区，互斥执行
            #pragma omp critical (name)
            {
                printf("Thread %d entered critical region\n",omp_get_thread_num());
                // 内层并行区
                #pragma omp parallel
                {
                    // single 确保只有一个线程执行下面的语句
                    #pragma omp single
                    {
                        printf("Thread %d executing single region\n",omp_get_thread_num());
                        i++;
                    }
                }
                printf("Thread %d leaving critical region,i = %d\n",omp_get_thread_num(),i);
            }
        }
    }
}

int main(){
    critical_work();
    return 0;
}