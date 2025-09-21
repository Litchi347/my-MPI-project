// 阻塞通信模式实例
# include <mpi.h>
# include <iostream>
# include <vector>

using namespace std;

int main(int argc,char* argv[]){
    MPI_Init(&argc,&argv);
    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    const int TEST_SIZE = 2000;
    vector<float> send_buf(TEST_SIZE,0.0f);
    vector<float> recv_buf(TEST_SIZE,0.0f);

    int prev,next;                                                           // prev:当前进程的“后继”进程后；next:当前进程的“前驱”进程号
    next = rank + 1;

    // 设置进程标识间的关系
    if(next >= size){
        next = 0;
    }
    prev = rank - 1;
    if(prev < 0){
        prev = size - 1;
    }

    int tag = 1456;
    int count = TEST_SIZE / 3;
    MPI_Status status;
    MPI_Request request;
    if(size != 2){
        if(rank == 0){
            cout << "This test requires exactly 2 processes" << endl;
        }
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    if(rank == 0){
        if (rank == 0){
            cout << "Rsend Test " << endl;
        }
        // 进程0在执行就绪发送之前首先执行一个阻塞接收
        // 保证就绪发送操作一定在改阻塞接收完成后才能进行，该阻塞接收操作没有接收任何数据
        MPI_Recv(MPI_BOTTOM,0,MPI_INT,next,tag,MPI_COMM_WORLD,&status);      // MPI_BOTTOM是MPI预定义的一个内存地址
        cout << "Processs " << rank << " post Ready send" << endl;
        //执行就绪发送
        MPI_Rsend(send_buf.data(),count,MPI_FLOAT,next,tag,MPI_COMM_WORLD);

    }else{
        cout << "Process " << rank << " post a receive call" << endl;

        // 进程1先执行一个非阻塞的接收调用，该调用可以立即返回
        MPI_Irecv(recv_buf.data(),TEST_SIZE,MPI_FLOAT,
                    MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&request);
        
        // 然后再执行一个发送空信息的阻塞调用，将这一阻塞发送放在和就绪发送对应的MPI_Irecv之后，可以保证就绪发送一定在相应的接收调用之后才调用
        MPI_Send(MPI_BOTTOM,0,MPI_INT,next,tag,MPI_COMM_WORLD);

        // 完成非阻塞接收调用
        MPI_Wait(&request,&status);

        cout << "Process " << rank << " Receive Rsend message from " << status.MPI_SOURCE << endl;
    }
    MPI_Finalize();
    return 0;
}
