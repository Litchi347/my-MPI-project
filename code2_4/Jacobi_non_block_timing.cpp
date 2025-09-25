# include <mpi.h>
# include <iostream>
# include <vector>
# include <fstream>
# include <iomanip>

using namespace std;

int main(int argc,char* argv[]){
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);                                                 // 进程总数

    const int N = 100000000;                                                             // 全局区间大小
    const int MAX_ITERS = 100;                                                           // 迭代次数

    int local_n = N / size;                                                              // 每个进程分到的局部区间大小
    int remainder = N % size;
    if(rank < remainder){                                                                // 以解决每个进程无法平分所有区间的问题
        local_n ++;                                                                      // 把余下的点平均分配给前 remainder 个进程
    }
    int offset = (N / size) * rank + min(rank,remainder);                                // offset指的是第i个进程的数据在全局数组里的起始下标

    vector<double> x(local_n + 2,0.0);
    vector<double> new_x(local_n + 2,0.0);                                               // 临时区间存储数据

    // 初始化边界条件，给所有边界值赋值为1.0
    if(offset == 0){
        x[1] = 1.0;
    }
    if(offset + local_n == N){
        x[local_n] = 1.0;
    }

    double t_start,t_end;
    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

    // Jacob迭代
    for(int iter = 0;iter < MAX_ITERS;iter++){
        MPI_Request reqs[4];
        int req_count = 0;
        if(rank > 0){
            MPI_Isend(&x[1],1,MPI_DOUBLE,rank - 1,0,MPI_COMM_WORLD,&reqs[req_count++]);
            MPI_Irecv(&x[0],1,MPI_DOUBLE,rank - 1,0,MPI_COMM_WORLD,&reqs[req_count++]);  // MPI_STATUS_IGNORE表示不关心这个进程的实际状态，只要收到消息即可，因为在Recv模式中前后进程信息已经非常明确，不需要再通过status来确定了
        }
        if(rank < size -1 ){
            MPI_Isend(&x[local_n],1,MPI_DOUBLE,rank + 1,0,MPI_COMM_WORLD,&reqs[req_count++]);
            MPI_Irecv(&x[local_n + 1],1,MPI_DOUBLE,rank + 1,0,MPI_COMM_WORLD,&reqs[req_count++]);
        }
        // 计算“内部点”，不依赖邻居
        for(int i = 2;i < local_n;i ++){
            new_x[i] = 0.5 * (x[i - 1] + x[i + 1]);
        }
        // 等待边界通信完成
        MPI_Waitall(req_count,reqs,MPI_STATUS_IGNORE);

        new_x[1] = 0.5 * (x[0] + x[2]);
        new_x[local_n] = 0.5 * (x[local_n - 1] + x[local_n + 1]);

        x.swap(new_x);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t_end = MPI_Wtime();

    double local_time = t_end - t_start;
    double max_time;
    MPI_Reduce(&local_time,&max_time,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);                // 比较所有进程的local_time，根据木桶效应取最长的时间

    // // 收集结果
    // vector<int> recvcounts(size);                                                        // recvcounts表示第i个进程贡献多少个数据给根进程
    // for(int i = 0;i < size;i++){
    //     recvcounts[i] = N / size;
    //     if(i < remainder){
    //         recvcounts[i] ++;
    //     }
    // }
    // vector<int> displs(size);                                                            // displs表示第i个进程的数据在全局数组里的起始下标（位移offset)
    // displs[0] = 0;
    // for(int i = 1;i < size;i++){
    //     displs[i] = displs[i-1] + recvcounts[i-1];
    // }
    // vector<double> global_x;
    // if(rank == 0){
    //     global_x.resize(N);
    // }
    // MPI_Gatherv(&x[1],local_n,MPI_DOUBLE,                                                // MPI的可变长度收集操作
    //             global_x.data(),recvcounts.data(),displs.data(),MPI_DOUBLE,
    //             0,MPI_COMM_WORLD);
    // if(rank == 0){
    //     for(int i=0;i<N;i++){
    //         cout << i << "\t" << global_x[i] << "\n";
    //     }
    // }

    if(rank == 0){
        cout << "Time of calculation=" << fixed << setprecision(6) << max_time << " s\n";
    }
    MPI_Finalize();
    return 0;
}