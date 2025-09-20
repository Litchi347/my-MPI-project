# include<stdio.h>
# include <mpi.h>
# define SIZE 2

static int src = 0;
static int dest = 1;

int main(int argc,char* argv[]){
    int act_size = 0;
    int flag,rval;
    int buffer[SIZE];
    MPI_Status status,status1,status2;
    int count1,count2;

    MPI_Init(&argc,&argv);
    int rank,np;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&np);

    if(np != 2){
        fprintf(stderr,"*** This program uses exactly 2 processes!***\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    act_size = 5;                                                              // 消息最大长度
    if(rank == src){                                                           // 当前进程为发送进程
        act_size = 1;
        MPI_Ssend(buffer,act_size,MPI_INT,dest,1,MPI_COMM_WORLD);
        

    }
}