// 矩阵向量乘法 c = a * b
#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

const int MAX_ROWS = 1000;                       // 最大行数
const int MAX_COLS = 1000;                       // 最大列数

int main(int argc,char* argv[]){
    int myid,master,numprocs;
    MPI_Status status;

    const int rows = 100;
    const int cols = 100;

    // 矩阵和向量
    static double a[MAX_ROWS][MAX_COLS];
    static double b[MAX_COLS];
    static double c[MAX_ROWS];                   // 结果向量
    static double buffer[MAX_COLS];

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    master = 0;                                  // 主进程是第一个进程
     
    if(myid == master){
        // 主进程对矩阵A和B赋初值
        for(int i = 0;i < cols;i++){
            b[i] = 1.0;
            for(int j = 0;j < rows;j++){
                a[i][j] = i + 1;
            }
        }

        int numsent = 0;

        // 将矩阵B发送给所有其他的从进程，通过广播实现
        MPI_Bcast(b,cols,MPI_DOUBLE,master,MPI_COMM_WORLD);

        // 依次将矩阵A的各行发送给其他的numprocs-1个从进程
        for(int i = 0;i < min(numprocs - 1, rows );i++){
            for(int j = 0;j < cols;j++){
                // 将一行的数据取出来依次放到缓冲区
                buffer[j] = a[i][j];
            }
            // 将准备好的一行数据发送出去
            MPI_Send(buffer,cols,MPI_DOUBLE,i+1,i+1,MPI_COMM_WORLD);
            numsent ++;
        }

        // 对所有的行，依次接收从进程对一行数据的计算结果
        for(int i = 0;i < rows; i++){
            double ans;
            MPI_Recv(&ans,1,MPI_DOUBLE,MPI_ANY_SOURCE,
                MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            int sender = status.MPI_SOURCE;
            int anstype = status.MPI_TAG;

            c[anstype - 1] = ans;                // 将该行数据赋值给结果数组C的相应单元

            if(numsent < rows){                  // 如果还有其他的行没有被计算，则继续发送
                for(int j = 0; j < cols; j++){
                    buffer[j] = a[numsent][j];   // 准备好新一行的数据，并将该行数据发送出去
                }
                MPI_Send(buffer,cols,MPI_DOUBLE,sender,
                    numsent + 1,MPI_COMM_WORLD);
                    numsent ++;
            }else{                               // 若所有行都已发送出去，则每接收一个消息则向相应的从进程发送一个标识为0的空消息，终止该从进程的执行
                MPI_Send(nullptr,0,MPI_DOUBLE,sender,
                    0,MPI_COMM_WORLD);
            }
        }
        cout << "Result vector C:" << endl;
    }else{                                       // 从进程的执行步骤，首先是接收数组B
        MPI_Bcast(b,cols,MPI_DOUBLE,master,MPI_COMM_WORLD);
        while(true){
            MPI_Recv(buffer,cols,MPI_DOUBLE,master,
                    MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if(status.MPI_TAG == 0){             // 若接收到标识为0的消息，则退出执行
                break;
            }
            // 计算一行的结果，并将结果发送给主进程
            int row = status.MPI_TAG;
            double ans = 0.0;
            for(int i = 0; i < cols;i++){
                ans += buffer[i] * b[i];
            }
            MPI_Send(&ans,1,MPI_DOUBLE,master,row,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}