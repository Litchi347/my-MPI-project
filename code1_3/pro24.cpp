#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

const int MAX_ROWS = 1000;
const int MAX_COLS = 1000;

int main(int argc,char* argv[]){
    int myid,master,numprocs;
    MPI_Status status;

    const int rows = 100;
    const int cols = 100;

    // 矩阵和向量
    static double a[MAX_ROWS][MAX_COLS];
    static double b[MAX_COLS];
    static double c[MAX_ROWS];              // 结果向量
    static double buffer[MAX_COLS];

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    master = 0;
     
    if(myid == master){
        // 初始化矩阵A和向量B
        for(int i = 0;i < cols;i++){
            b[i] = 1.0;
            for(int j = 0;j < rows;j++){
                a[i][j] = i + 1;
            }
        }
        int numsent = 0;

        // 广播向量B
        for(int i = 0;i < min(numprocs - 1, rows );i++){
            for(int j = 0;j < cols;j++){
                buffer[j] = a[i][j];
            }
            MPI_Send(buffer,cols,MPI_DOUBLE,i+1,i+1,MPI_COMM_WORLD);
            numsent ++;
        }

        // 接收结果并继续分发
        for(int i = 0;i < rows; i++){
            double ans;
            MPI_Recv(&ans,1,MPI_DOUBLE,MPI_ANY_SOURCE,
                MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            int sender = status.MPI_SOURCE;
            int anstype = status.MPI_TAG;

            c[anstype - 1] = ans;

            if(numsent < rows){
                for(int j = 0; j < cols; j++){
                    buffer[j] = a[numsent][j];
                }
                MPI_Send(buffer,cols,MPI_DOUBLE,sender,
                    numsent + 1,MPI_COMM_WORLD);
                    numsent ++;
            }else{
                // 发送终止信号
                MPI_Send(nullptr,0,MPI_DOUBLE,sender,
                    0,MPI_COMM_WORLD);
            }
        }
        cout << "Result vector C:" << endl;
    }else{
        MPI_Bcast(b,cols,MPI_DOUBLE,master,MPI_COMM_WORLD);
        while(true){
            MPI_Recv(buffer,cols,MPI_DOUBLE,master,
                    MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if(status.MPI_TAG == 0){
                break;
            }
            int row = status.MPI_TAG;
            double ans = 0.0;
            for(int i = 0; i<cols;i++){
                ans += buffer[i] * b[i];
            }
            MPI_Send(&ans,1,MPI_DOUBLE,master,row,MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}