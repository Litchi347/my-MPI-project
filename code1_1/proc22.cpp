#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

inline int idx(int i0,int j0,int mysize){
    return i0 * (mysize + 2) + j0;
}

int main(int argc,char* argv[]){
    const int totalsize = 16;
    const int steps = 10;

    MPI_Init(&argc,&argv);

    int myid,numprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    if(totalsize % numprocs != 0){
        if(myid == 0){
            cerr << "Error:totalsize (" << totalsize << ")must be divisible by numprocs(" << numprocs << ")." << endl;
        }
        MPI_Abort(MPI_COMM_WORLD,1);
        return 1;
    }
    const int mysize = totalsize / numprocs;
    cout << "Process " << myid << " of " << numprocs << " is alive" << endl;

    vector<double> a(totalsize * (mysize + 2),0.0);
    vector<double> b(totalsize * (mysize + 2),0.0);

    if(myid == 0){
        int j0 = 0;
        for(int i0 = 0;i0 < totalsize;i0++){
            a[idx(i0,j0,mysize)] = 8.0;
        }
    }

    if(myid == numprocs - 1){
        int j0 = mysize;
        for(int i0 = 0;i0 < totalsize;i0++){
            a[idx(i0,j0,mysize)] = 8.0;
        }
    }

    for(int j0 = 0; j0 < mysize + 2; j0++){
        a[idx(0,j0,mysize)] = 8.0;
        a[idx(totalsize - 1,j0,mysize)] = 8.0;

    }

    MPI_Datatype coltype;
    MPI_Type_vector(totalsize,1,mysize+2,MPI_DOUBLE,&coltype);
    MPI_Type_commit(&coltype);
    
    MPI_Status status;

    for(int n=1;n<=steps;n++){
        // 从左向右传递
        if(myid == 0){
            MPI_Send(&a[idx(0,mysize,mysize)],1,coltype,myid + 1,10,MPI_COMM_WORLD);
        }else if(myid == numprocs - 1){
            MPI_Recv(&a[idx(0,0,mysize)],1,coltype,myid - 1,10,MPI_COMM_WORLD,&status);
        }else{
            MPI_Sendrecv(&a[idx(0,mysize,mysize)],1,coltype,myid + 1,10,
                            &a[idx(0,0,mysize)],1,coltype,myid - 1,10,
                        MPI_COMM_WORLD,&status);
        }

        if(myid == 0){
            MPI_Recv(&a[idx(0,mysize + 1,mysize)],1,coltype,myid + 1,10,MPI_COMM_WORLD,&status);
        }else if(myid == numprocs - 1){
            MPI_Send(&a[idx(0,1,mysize)],1,coltype,myid - 1,10,MPI_COMM_WORLD);
        }else{
            MPI_Sendrecv(&a[idx(0,1,mysize)],1,coltype,myid - 1,10,
                            &a[idx(0,mysize + 1,mysize)],1,coltype,myid + 1,10,
                            MPI_COMM_WORLD,&status);
        }

        int begin_col = 1;
        int end_col = mysize;

        if(myid == 0){
            begin_col = 2;
        }
        if(myid == numprocs - 1){
            end_col = mysize - 1;
        }

        for(int j0 = begin_col;j0<=end_col;j0++){
            for(int i0=1;i0<=totalsize;i0++){
                b[idx(i0, j0, mysize)] = 0.25 * (
                    a[idx(i0, j0 + 1, mysize)] + a[idx(i0, j0 - 1, mysize)] +
                    a[idx(i0 + 1, j0, mysize)] + a[idx(i0 - 1, j0, mysize)]
                );
            }
        }

        for(int j0 = begin_col;j0<=end_col;j0++){
            for(int i0=1;i0<=totalsize;i0++){
                a[idx(i0, j0, mysize)] = b[idx(i0, j0, mysize)];
            }
        }
    }
    int begin_col = 1;
        int end_col = mysize;

        if(myid == 0){
            begin_col = 2;
        }
        if(myid == numprocs - 1){
            end_col = mysize - 1;
        }

    for(int i0 = 1;i0 <= totalsize - 2;i0++){
        cout << "Process " << myid << ":" ;
        for(int j0 = begin_col;j0<=end_col;j0++){
            cout << a[idx(i0,j0,mysize)] << " ";
        }  
        cout << endl;
    }
    MPI_Type_free(&coltype);
    MPI_Finalize();


    return 0;
}