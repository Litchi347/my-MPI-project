# include "Module.h"
# include <iostream>
# include <iomanip>
# include <fstream>
# include <sstream>
# include <mpi.h>

using namespace std;

int main(){
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);

    // 计算局部尺寸和起始索引
    int block_size = ni_global / num_procs;
    ni_local = block_size;
    ni_start = my_rank * block_size;

    // 处理剩余的网格点，分配给最后一个进程
    if(my_rank == num_procs - 1){
        ni_local += ni_global % num_procs;
    }

    // 如果进程数大于 ni_global，可能需要更复杂的划分或退出
    if(ni_local <= 0 && ni_global > 0){
        if(my_rank == 0){
            cerr << "Error: Too many processes for the given grid size " << endl;
        }
        MPI_Finalize();
        return 1;
    }

    call_mesh2d();
    call_initia();
    call_cflcon();
    call_bounda();

    // 主时间循环
    do{
        call_solver();
        time0 = time0 + dt;

        if (my_rank == 0){
            cout << "Time=" << "\t" << time0 << endl;
        }
        
        if(my_rank == 0){
            call_output(time0/dt);
        }

    } while(time0 <= tout);

    call_releasespace();
    MPI_Finalize();
    return 0;
}