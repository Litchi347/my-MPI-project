// 使用同步消息发送，
// 发送进程分别以同步方式发送1个和4个数据，消息标识分别为1和2。
// 接收进程用标准接收操作接收它，并检查接收到数据的个数
# include <stdio.h>
# include <mpi.h>
# define SIZE 10

static int src = 0;
static int dest = 1;

int main(int argc,char* argv[]){
    int act_size = 0;
    int buffer[SIZE];
    MPI_Status status1,status2;
    int count1,count2;

    MPI_Init(&argc,&argv);
    int rank,np;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&np);

    if(np != 2){
        fprintf(stderr,"*** This program uses exactly 2 processes!***\n");     // 与print类似，通常用来打印错误信息
        MPI_Abort(MPI_COMM_WORLD,1);                                           // 紧急终止MPI里的所有进程，并返回错误代码1
    }
    act_size = 5;                                                              // 消息最大长度
    if(rank == src){                                                           // 当前进程为发送进程
        act_size = 1;
        MPI_Ssend(buffer,act_size,MPI_INT,dest,1,MPI_COMM_WORLD);              // 同步消息发送，发送一个整数型，tag标识为1
                                                                               // 和普通的 MPI_Send 不一样，MPI_Ssend 会等到接收方已经开始接收，才会返回
        fprintf(stderr,"MPI_Ssend %d data,tag=1\n",act_size);
        
        act_size = 4;
        MPI_Ssend(buffer,act_size,MPI_INT,dest,2,MPI_COMM_WORLD);              // 同步发送4个整数型，tag标识为2
        fprintf(stderr,"MPI_Ssend %d data,tag=2\n",act_size);
    }else if(rank == dest){                                                    // 当前进程为接收进程
        MPI_Recv(buffer,act_size,MPI_INT,src,1,MPI_COMM_WORLD,&status1);       // 标准消息接收，这里指定的消息长度act_size是最大信息长度，tag为1
        MPI_Recv(buffer,act_size,MPI_INT,src,2,MPI_COMM_WORLD,&status2);       // 标准消息接收，这里指定的消息长度act_size是最大信息长度，tag为2

        MPI_Get_count(&status1,MPI_INT,&count1);
        fprintf(stderr,"receive %d data,tag=%d\n",count1,status1.MPI_TAG);     // 消息1包含的数据的个数
        MPI_Get_count(&status2,MPI_INT,&count2);
        fprintf(stderr,"receive %d data,tag=%d\n",count2,status2.MPI_TAG);     // 消息2包含的数据个数
    }
    MPI_Finalize();
    return 0;
}