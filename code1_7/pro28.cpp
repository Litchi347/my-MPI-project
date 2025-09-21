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

    // 就绪发送者
    if(rank == 0){
        if (rank == 0){
            cout << "Rsend Test " << endl;
        }
        // 等待进程1发送的“就绪信号”，只用于同步，不传输实际数据（等待确认准备好）
        MPI_Recv(MPI_BOTTOM,0,MPI_INT,next,tag,MPI_COMM_WORLD,&status);      // MPI_BOTTOM是MPI预定义的一个内存地址
        cout << "Processs " << rank << " post Ready send" << endl;
        
        //执行就绪发送,发送实际的数据给进程1，必须确保进程1的接受操作已经发布（通过前面的同步保证）
        MPI_Rsend(send_buf.data(),count,MPI_FLOAT,next,tag,MPI_COMM_WORLD);

    // 接收者
    }else{
        cout << "Process " << rank << " post a receive call" << endl;

        // 先执行一个非阻塞的接收调用，调用后立即返回，不会等待接收操作完成（可以接收）
        MPI_Irecv(recv_buf.data(),TEST_SIZE,MPI_FLOAT,
                    MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&request);
        
        // 发送空信息的就绪信号，确保进程0的就绪发送在接收操作发布之后才执行
        MPI_Send(MPI_BOTTOM,0,MPI_INT,next,tag,MPI_COMM_WORLD);

        // 等待非阻塞接收调用的完成：等待实际数据的到达
        MPI_Wait(&request,&status);

        cout << "Process " << rank << " Receive Rsend message from " << status.MPI_SOURCE << endl;
    }
    MPI_Finalize();
    return 0;
}
