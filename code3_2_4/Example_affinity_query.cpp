// OpenMP多层嵌套并行示例
# include <stdio.h>
# include <omp.h>

void socket_init(int socket_num){
    int n_procs;

    n_procs = omp_get_place_num_procs(socket_num);

    #pragma omp parallel num_threads(n_procs) proc_bind(close)
    {
        printf("Reporting in from socket num,thread num:%d %d\n",socket_num,omp_get_thread_num());
    }
}

int main(){
    int n_sockets,socket_num;

    omp_set_nested(1);                                 // 启用嵌套并行
    omp_set_max_active_levels(2);                      // 指定最多两层并行

    n_sockets = omp_get_num_places();                  // 取得可用的 place 的数量

    // 在每个 socket 上启动一个线程
    // 系统只有一个插槽，模拟“两插槽系统”效果，在环境变量中人为定义两个place来模拟双 socket
    /*
    export OMP_PLACES="{0:10},{10,10}"
    export OMP_PROC_BIND=spread
    */
    #pragma omp parallel num_threads(n_sockets) private(socket_num) proc_bind(spread)
    {
        socket_num = omp_get_place_num();
        socket_init(socket_num);
    }
    return 0;
}