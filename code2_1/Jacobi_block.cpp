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
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    const int N = 16;
    const int MAX_ITERS = 10;

    // 每个进程分到的局部区间大小
    int local_n = N / size;
    int remainder = N % size;
    if(rank < remainder){
        local_n ++;
    }
    int offset = (N / size) * rank + min(rank,remainder);

    vector<double> x(local_n + 2,0.0);
    vector<double> new_x(local_n + 2,0.0);

    if(offset == 0){
        x[1] = 1.0;
    }
    if(offset + local_n == N){
        x[local_n] = 1.0;
    }

    for(int iter = 0;iter < MAX_ITERS;iter++){
        if(rank > 0){
            MPI_Send(&x[1],1,MPI_DOUBLE,rank - 1,0,MPI_COMM_WORLD);
            MPI_Recv(&x[0],1,MPI_DOUBLE,rank - 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        if(rank < size -1 ){
            MPI_Send(&x[local_n],1,MPI_DOUBLE,rank + 1,0,MPI_COMM_WORLD);
            MPI_Recv(&x[local_n + 1],1,MPI_DOUBLE,rank + 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        for(int i=1;i<=local_n;i++){
            new_x[i] = 0.5 * (x[i-1]+x[i+1]);
        }
        x.swap(new_x);
    }
    // 收集结果
    vector<int> recvcounts(size),displs(size);
    for(int i = 0;i < size;i++){
        recvcounts[i] = N / size;
        if(i < remainder){
            recvcounts[i] ++;
        }
    }
    displs[0] = 0;
    for(int i = 1;i<size;i++){
        displs[i] = displs[i-1]+recvcounts[i-1];
    }
    vector<double> global_x;
    if(rank == 0){
        global_x.resize(N);
    }
    MPI_Gatherv(&x[1],local_n,MPI_DOUBLE,
                global_x.data(),recvcounts.data(),displs.data(),MPI_DOUBLE,
                0,MPI_COMM_WORLD);
    
    if(rank == 0){
        ofstream fout("jacobi_result.txt");
        fout << "Index\tValue\n";
        cout << "Index\tValue\n";
        for(int i = 0;i < N ;i++){
            fout << i << "\t" << fixed << setprecision(6) << global_x[i] << "\n";
            cout << i << "\t" << fixed << setprecision(6) << global_x[i] << "\n";
        }
        fout.close();

        ofstream fcsv("Jacobi_results.csv");
        fcsv << "Index,Value\n";
        for(int i = 0;i < N;i++){
            fcsv << i << "," << global_x[i] << "\n";
        }
        fcsv.close();
    }
    MPI_Finalize();
    return 0;
}