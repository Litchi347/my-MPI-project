# include "Module.h"
# include <fstream>
# include <vector>
# include <cstdio>

using namespace std;

void call_output(double num){
    double* f_global = nullptr;
    int* recvcounts =  nullptr;
    int* displs =  nullptr;

    // 只有主进程负责输出
    if(my_rank == 0){
        f_global = new double[ni_global * nj];
        recvcounts = new int[num_procs];
        displs = new int[num_procs];
    }
    int send_count = ni_local * nj;

    MPI_Gather(&send_count,1,MPI_INT,recvcounts,1,MPI_INT,0,MPI_COMM_WORLD);

    if(my_rank == 0){
        displs[0] = 0;
        for(int p = 1;p < num_procs;p++){
            displs[p] = displs[p - 1] + recvcounts[p - 1];
        }
    }

    MPI_Gatherv(&f[IDX(1,0)],send_count,MPI_DOUBLE,f_global,recvcounts,displs,MPI_DOUBLE,0,MPI_COMM_WORLD);

    if(my_rank == 0){
        char filename[50];
        snprintf(filename, sizeof(filename), "flow_%05.0f.dat", num);
        ofstream outfile(filename);
        outfile << "VARIABLEs = X,Y,fi" << "\n";
        outfile << "ZONE I=" << ni_global << "\t" << "J=" << nj << "\n";

        double* f_reordered = new double[ni_global * nj];
        int current_idx = 0;

        outfile << "DATAPACKING = BLOCK" << "\n";
        for(int j = 0;j <nj;j ++){
            for(int i = 0;i < ni_global; i ++){
                outfile << x[i] << "\n";
            }
        }

        for(int j = 0;j <nj;j ++){
            for(int i = 0;i < ni_global; i ++){
                outfile << y[j] << "\n";
            }
        }

        vector<double> f_matrix(ni_global * nj);
        int offset = 0;
        int current_i_start = 0;

        for(int p = 0;p < num_procs; p ++){
            int w = recvcounts[p] / nj;
            for(int local_i = 0;local_i < w; local_i++){
                for(int local_j = 0;local_j < nj;local_j ++){
                    int global_i = current_i_start + local_i;
                    int global_j = local_j;
                    f_matrix[global_j * ni_global + global_i] = f_global[offset++];
                }
            }
            current_i_start += w;
        }
        for(size_t k = 0;k < f_matrix.size();k ++){
            outfile << f_matrix[k] << "\n";
        }

        outfile.close();
        delete[] f_reordered;
        delete[] recvcounts;
        delete[] displs;
        delete[] f_global;

        return;
    }
}