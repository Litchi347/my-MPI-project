# include "Module.h"

void call_non_blockking_comm(){
    int req_count = 0;
    int tag = 100;
    int left_proc = my_rank - 1;
    int right_proc = my_rank + 1;
    int sendcount = nj;                // 每次交换一整列数据

    // 左边界通信
    if(my_rank > 0){
        // 从左邻居接收数据
        MPI_Irecv(&f[IDX(0,0)],sendcount,MPI_DOUBLE,left_proc,tag,MPI_COMM_WORLD,&request[req_count++]);

        // 向左邻居发送数据
        MPI_Isend(&f[IDX(1,0)],sendcount,MPI_DOUBLE,left_proc,tag,MPI_COMM_WORLD,&request[req_count++]);
    }

    // 右边界通信
    if(my_rank < num_procs - 1){
        // 从右邻居接收数据
        MPI_Irecv(&f[IDX(ni_local + 1,0)],sendcount,MPI_DOUBLE,right_proc,tag,MPI_COMM_WORLD,&request[req_count++]);
        // 向右邻居发送数据
        MPI_Isend(&f[IDX(ni_local,0)],sendcount,MPI_DOUBLE,right_proc,tag,MPI_COMM_WORLD,&request[req_count++]);
    }

    // 等待通信完成
    if (req_count > 0){
        MPI_Waitall(req_count,request,status);
    }
}