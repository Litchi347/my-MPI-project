// 基于 release /acquire 语义的同步
# include <stdio.h>
# include <omp.h>

// 依靠 critical 隐含同步
// int main()
// {
//     int x = 0,y = 0;
//     #pragma omp parallel num_threads(2)
//     {
//         int thrd = omp_get_thread_num();
//         if (thrd == 0){
//             x = 10;
//             // 隐含 flush 操作：进入时线程会更新自己的缓存，退出时将自己对共享变量的写入更新传播到内存中
//             #pragma omp critical
//             {
//                 y = 1;
//             }
//         }else{
//             int tmp = 0;
//             while (tmp == 0)
//             {
//                 #pragma omp critical
//                 {
//                     tmp = y;
//                 }
//             }
//             printf("x = %d\n",x);      // 此时线程1已经可以看到线程0对x的更改
//         }
//     }
//     return 0;
// }

// 基于 release/acquire 的显式同步
int main(){
    int x = 0,y = 0;
    #pragma omp parallel num_threads(2)
    {
        int thrd = omp_get_thread_num();
        if(thrd == 0){
            x = 10;
            #pragma omp atomic write release       // 此线程之前所有写操作在该点前必须对其他线程可见
                y = 1;
        }else{
            int tmp = 0;
            while(tmp == 0){
                #pragma omp atomic read acquire    // 此线程之后所有读操作能看到 release 线程的更新
                    tmp = y;
            }
            printf("x = %d\n",x);
        }
    }
    return 0;
}

// 使用 flush + atomic
// int main(){
//     int x = 0,y = 0;
//     #pragma omp parallel num_threads(2)
//     {
//         int thrd = omp_get_thread_num();
//         if(thrd == 0){
//             x = 10;
//             #pragma omp flush                      // 使所有可访问的 shared 变量刷新
//             #pragma omp atomic write
//                 y = 1;
//         }else{
//             int tmp = 0;
//             while (tmp == 0)
//             {
//                 #pragma omp atomic read
//                     tmp = y;
//             }
//             #pragma omp flush
//                 printf("x= %d\n",x);
//         }
//     }
//     return 0;
// }

// 错误示范
int main()
{

    // !!! THIS CODE WILL FAIL TO PRODUCE CONSISTENT RESULTS !!!!!!!
    // !!! DO NOT PROGRAM SYNCHRONIZATION THIS WAY !!!!!!!

    int x = 0, y;
    #pragma omp parallel num_threads(2)
    {
        int thrd = omp_get_thread_num();
        if (thrd == 0) {
            // x 和 y 的写没有强制顺序保证
            #pragma omp critical
            { 
                x = 10; 
            }
            #pragma omp atomic write
                y = 1;
        } else {
            int tmp = 0;
            while (tmp == 0) {
                #pragma omp atomic read acquire
                    tmp = y;
            }
            #pragma omp critical
            { 
                printf("x = %d\n", x); 
            } // !! NOT ALWAYS 10
        }
    }
    return 0;
}