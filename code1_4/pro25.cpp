// 主进程按序与乱序打印从进程的消息
# include<stdio.h>
# include<mpi.h>
# include<string.h>

# define MSG_EXIT 1
# define MSG_PRINT_ORDERED 2                                                     // 定义按序打印标识
# define MSG_PRINT_UNORDERED 3                                                   // 定义乱序打印标识

void master_io();
void slave_io();

int main(int argc,char* argv[]){
    int rank;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if(rank == 0){
        master_io();                                                             // 进程0作为主程序
    }else{
        slave_io();                                                              // 其他程序作为从程序
    }
    MPI_Finalize();
    return 0;
}

// 下面的函数为主程序执行的程序
void master_io(){
    int size;
    int nslave,firstmsg;

    char buf[256];
    char buf2[256];
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD,&size);                                         // 得到总的进程数
    nslave = size - 1;                                                           // 得到从进程的进程数
    while(nslave > 0){                                                           // 只要还有从进程，则执行下面的接收与打印
        MPI_Recv(buf,256,MPI_CHAR,MPI_ANY_SOURCE,MPI_ANY_TAG,
            MPI_COMM_WORLD,&status);                                             // 从任意从进程接收任意标识的信息
        switch(status.MPI_TAG){
            case MSG_EXIT:                                                       // 若该从进程要求退出，则将总的从进程个数减1
                nslave --;
                break;
            case MSG_PRINT_UNORDERED:                                            // 若该从进程要求乱序打印，则直接将该消息打印
                fputs(buf,stdout);
                break;
            case MSG_PRINT_ORDERED:                                              // 从进程要求按序打印，这种打印方式处理起来复杂一些，首先需要对收到的信息进行排序，若有些消息还没有收到，则需要调用接收语句接收相应的有序消息
                firstmsg = status.MPI_SOURCE;
                for(int i = 1;i<size;i++){                                       // 标识号从小到大开始打印
                    if(i == firstmsg){
                        fputs(buf,stdout);                                       // 若接收的消息恰巧是需要打印的消息，则直接打印
                    }else{                                                       // 否则，先接收打印的信息，然后再打印
                        // 这一接收语句指定了源进程和消息标识，而不是像一开始的那样用任意源和任意标识
                        MPI_Recv(buf2,256,MPI_CHAR,i,MSG_PRINT_ORDERED,
                            MPI_COMM_WORLD,&status);
                        fputs(buf2,stdout);
                    }
                }
                break;
        }
    }
}

// 从进程执行的部分
void slave_io(){
    char buf[256];
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);                                         // 得到自己的标识
    sprintf(buf,"Hello from slave %d,ordered print\n",rank);

    MPI_Send(buf,strlen(buf)+1,MPI_CHAR,0,MSG_PRINT_ORDERED,MPI_COMM_WORLD);     // 先向主进程发送一个有序打印消息
    sprintf(buf,"Goodbye from slave %d,ordered pprint\n",rank);

    MPI_Send(buf,strlen(buf)+1,MPI_CHAR,0,MSG_PRINT_ORDERED,MPI_COMM_WORLD);   // 再向主进程发送一个有序打印消息
    sprintf(buf,"I'm exiting (%d),unordered print\n",rank);

    MPI_Send(buf,strlen(buf)+1,MPI_CHAR,0,MSG_PRINT_UNORDERED,MPI_COMM_WORLD);   // 最后，向主进程发送一个乱序打印消息
    MPI_Send(buf,0,MPI_CHAR,0,MSG_EXIT,MPI_COMM_WORLD);                          // 向主进程发送退出执行的消息
}