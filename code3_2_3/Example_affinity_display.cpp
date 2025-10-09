// // 线程亲和性显示和调试
// /* 
// 显示亲和性的三种方法：
// 方法1：设置环境变量
// 方法2：自定义输出格式
// 方法3：API调用显示
// */
// # include <stdio.h>
// # include <omp.h>

// int main(void){
//     omp_display_affinity(NULL);                                    // 主线程亲和性设置，参数为NULL表示使用默认消息格式

//     #pragma omp parallel num_threads(omp_get_num_procs())
//     {         
//         // 第一个并行区域：创建与处理器数量相等的线程数
//         if(omp_get_thread_num() == 0){                             // 线程0打印消息
//             printf("1st Parallel Region -- Affinity Reported \n");
//         }
//     }

//     #pragma omp parallel num_threads(omp_get_num_procs())
//     {         
//         // 第二个并行区域：使用相同的线程数和亲和性设置。根据OpenMP规范，不会重复显示亲和性信息
//         if(omp_get_thread_num() == 0){
//             printf("%s%s\n","Same Affinity as in Previous Parallel Region"," -- no Affinity Reported\n");
//         }
//     }

//     #pragma omp parallel num_threads (omp_get_num_procs()/2)
//     {      
//         // 使用一半的线程数，亲和性改变，会再次显示亲和性信息
//         if(omp_get_thread_num() == 0){
//             printf("Report affinity for using 1/2 of max threads.\n");
//         }
//     }

//     return 0;
// }

// # include <stdio.h>
// # include <stdlib.h>
// # include <omp.h>

// void socket_work(int socket_num,int n_thrds);

// // 线程分配：
// // 第一级:
// // 线程0 → Socket 0 (可运行在核心0,2,4,6)
// // 线程1 → Socket 1 (可运行在核心1,3,5,7)

// // 第二级（嵌套）:
// // Socket 0内: 线程0-3 → 核心0,2,4,6
// // Socket 1内: 线程0-3 → 核心1,3,5,7
// int main(void)
// {
//     int n_sockets,socket_num,n_thrds_on_socket;

//     omp_set_nested(1);                                      // 或者直接设置环境变量 OMP_NESTED = true，启用嵌套并行
//     omp_set_max_active_levels(2);                           // 或者直接设置环境变量 OMP_MAX_ACTIVE_LEVELS = 2，允许两级嵌套

//     n_sockets = omp_get_num_places();
//     n_thrds_on_socket = omp_get_place_num_procs(0);

//     #pragma omp parallel num_threads(n_sockets) private(socket_num)
//     {
//         socket_num = omp_get_place_num();

//         if(socket_num == 0){
//             printf("LEVEL 1 AFFINITIES 1 thead/socket.%d sockets:\n\n",n_sockets);
//         }
//         omp_display_affinity(NULL);
//         /*
//         输出结果解析：
//         nest_lavel = 1：第一级并行
//         parent_thrd_num = 0:由主线程创建
//         thrd_num = 0/1：线程编号
//         thrd_affinity：线程可以在socket0的所有核心上运行
//         */
//         socket_work(socket_num,n_thrds_on_socket);
//         /*
//         输出结果解析：
//         nest_lavel = 1：第二级嵌套
//         parent_thrd_num = 0:父线程编号（来自第一级）
//         thrd_num = 0/1：线程编号
//         */
//     }
//     return 0;
// }

// void socket_work(int socket_num,int n_thrds)
// {
//     #pragma omp parallel num_threads(n_thrds)
//     {
//         if(omp_get_thread_num() == 0){
//             printf("LEVEL 2 AFFINITIES,%d threads on socket %d\n",n_thrds,socket_num);
//         }
//         omp_display_affinity(NULL);
//     }
// }

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <omp.h>

#define FORMAT_STORE 80
#define BUFFER_STORE 80

int main(void)
{
    int i,n,thrd_num,max_req_store;
    size_t nchars;

    char default_format[FORMAT_STORE];
    char my_format[] = "host=%20H thrd_num=%0.4n binds_to=%A";
    char **buffer;

    nchars = omp_get_affinity_format(default_format,(size_t)FORMAT_STORE);
    printf("Default Affinity Format is:%s\n",default_format);

    if(nchars >= FORMAT_STORE){
        printf("Caution:Reported Format is truncated. Increase\n");
        printf("    FORMAT_STORE to %d.\n",nchars + 1);
    }
    omp_set_affinity_format(my_format);
    printf("Affinity Format set to:%s\n",my_format);

    n = omp_get_num_procs();
    buffer = (char **)malloc(sizeof(char *) * n);
    for(int i = 0;i < n;i++){
        buffer[i] = (char *)malloc(sizeof(char) * BUFFER_STORE);
    }
    max_req_store = 0;
    #pragma omp parallel private(thrd_num,nchars) reduction(max:max_req_store)
    {
        if(omp_get_num_threads() > n){
            exit(1);
        }
        thrd_num = omp_get_thread_num();
        nchars = omp_capture_affinity(buffer[thrd_num],(size_t)BUFFER_STORE,NULL);
        if(nchars > max_req_store){
            max_req_store = nchars;
        }
        for(i = 0;i < n;i++){
            printf("thrd_num= %d,affinity:%s\n",i,buffer[i]);
        }
        if(max_req_store >= BUFFER_STORE){
            printf("Caution:Affinity string truncated.  Increase\n");
            printf("    BUFFER_STORE to %d\n",max_req_store + 1);
        }
        for(int i =0 ;i<n;i++){
            free(buffer[i]);
        }
        return 0;
    }
}