# include <stdlib.h>
# include <stdio.h>
# include <mpi.h>
# define SIZE 6                                                                // 总的数据大小
static int src = 0;
static int dest = 1;

void Generate_Data(double* ,int);                                              // 产生发送的数据
void Normal_Test_Recv(double* ,int);                                           // 标准接收
void Buffered_Test_Send(double* ,int);                                         // 缓存发送
void Generate_Data(double* buffer,int buffer_size){

    for(int i = 0;i<buffer_size;i++){
        buffer[i] = (double)i+1;
    }

}

void Normal_Test_Recv(double* buffer,int buffer_size){
    MPI_Status status;
    double* b = buffer;                                                        // 先接收buffer_size-1个连续的双精度数
    MPI_Recv(b,(buffer_size - 1),MPI_DOUBLE,src,2000,MPI_COMM_WORLD,&status);
    fprintf(stderr,"standard receive a message of %d data\n",buffer_size - 1);
    for(int j = 0;j<buffer_size - 1;j++){
        fprintf(stderr," buf[%d] = %f\n",j,b[j]);
    }
    b += buffer_size - 1;                                                      // 再接收一个双精度数
    MPI_Recv(b,1,MPI_DOUBLE,src,2000,MPI_COMM_WORLD,&status);
    fprintf(stderr,"standard receive a message of one data\n");
    fprintf(stderr,"buf[0]=%f\n",*b);
}

void Buffered_Test_Send(double* buffer,int buffer_size){
    void *bbuffer;
    int size;
    fprintf(stderr,"buffered send message of %d data\n",buffer_size - 1);
    for(int j=0;j<buffer_size - 1;j++){  
        // 先用缓存方式发送buffer_size-1个双精度的数
        MPI_Bsend(buffer,(buffer_size -1),MPI_DOUBLE,dest,2000,MPI_COMM_WORLD);
        buffer += buffer_size - 1;
        fprintf(stderr,"buffered send message of one data\n");
        fprintf(stderr,"buf[0]=%f\n",*buffer);

        // 再用缓存方式发送1个双精度数
        MPI_Bsend(buffer,1,MPI_DOUBLE,dest,2000,MPI_COMM_WORLD);
        MPI_Buffer_detach(&bbuffer,&size);                                     // 强制收回发送缓冲，这样也保证了该操作返回后消息已经送出
        MPI_Buffer_attach(bbuffer,size);                                       // 再重新递交发送缓存
    }
}

int main(int argc,char* argv[]){
    int rank;
    double buffer[SIZE],*tmpbuffer,*tmpbuf;
    int tsize,bsize;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    if(rank == src){                                                       // 若当前进程为发送进程
        Generate_Data(buffer,SIZE);                                        // 产生发送数据
        MPI_Pack_size(SIZE,MPI_DOUBLE,MPI_COMM_WORLD,&bsize);              // 计算为发送SIZE个MPI_DOUBLE类型的数据所需的空间
        tmpbuffer = (double *)malloc(bsize + 2 * MPI_BSEND_OVERHEAD);      // 申请缓存发送所需要的空间
        if(!tmpbuffer){
            fprintf(stderr,"Could not allocate bsend buffer of size %d\n",bsize);
            MPI_Abort(MPI_COMM_WORLD,1);
        }
        MPI_Buffer_attach(tmpbuffer,bsize + MPI_BSEND_OVERHEAD);           // 将申请到的空间递交给MPI，从而MPI可以利用该空间进行消息缓存
        Buffered_Test_Send(buffer,SIZE);                                   // 执行缓存消息发送
        MPI_Buffer_detach(&tmpbuf,&tsize);                                 // 发送完成后收回递交的缓冲区
    }else if(rank == dest){                                                // 若当前进程为接收进程
        Normal_Test_Recv(buffer,SIZE);                                     // 执行标准的接收操作
    }else{
        fprintf(stderr,"*** This program uses exactly 2 progresses!***\n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }
    MPI_Finalize();
}
