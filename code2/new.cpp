#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double t_start, t_init, t_compute, t_gather;
    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

    // 二维全局网格大小
    const int N = 10000;
    const int MAX_ITERS = 10;

    // 按行划分
    int local_rows = N / size;      
    int remainder = N % size;
    if (rank < remainder) {
        local_rows++;
    }
    int offset = (N / size) * rank + min(rank, remainder);

    // 本地子网格(含ghost边界)
    vector<vector<double>> x(local_rows + 2, vector<double>(N + 2, 0.0));
    vector<vector<double>> new_x(local_rows + 2, vector<double>(N + 2, 0.0));

    // 初始化边界条件
    // 顶边界
    if (offset == 0) {
        for (int j = 0; j <= N+1; j++) {
            x[1][j] = 1.0;
        }
    }
    // 底边界
    if (offset + local_rows == N) {
        for (int j = 0; j <= N+1; j++) {
            x[local_rows][j] = 1.0;
        }
    }
    // 左右边界（所有进程都要设置）
    for (int i = 0; i <= local_rows+1; i++) {
        x[i][0] = 1.0;
        x[i][N+1] = 1.0;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t_init = MPI_Wtime();

    // Jacobi迭代（二维公式+上下通信）
    for (int iter = 0; iter < MAX_ITERS; iter++) {
        // 上边界通信
        if (rank > 0) {
            MPI_Send(x[1].data(), N+2, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
            MPI_Recv(x[0].data(), N+2, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        // 下边界通信
        if (rank < size-1) {
            MPI_Send(x[local_rows].data(), N+2, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
            MPI_Recv(x[local_rows+1].data(), N+2, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Jacobi更新 (二维: 上下左右四邻居平均)
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j <= N; j++) {
                new_x[i][j] = 0.25 * (x[i-1][j] + x[i+1][j] + x[i][j-1] + x[i][j+1]);
            }
        }
        swap(x, new_x);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t_compute = MPI_Wtime();

    // 收集结果
    vector<int> recvcounts(size);
    for (int i = 0; i < size; i++) {
        recvcounts[i] = (N / size) * N;
        if (i < remainder) {
            recvcounts[i] += N;
        }
    }

    vector<int> displs(size);
    displs[0] = 0;
    for (int i = 1; i < size; i++) {
        displs[i] = displs[i-1] + recvcounts[i-1];
    }

    vector<double> local_flat(local_rows * N);
    for (int i = 1; i <= local_rows; i++) {
        for (int j = 1; j <= N; j++) {
            local_flat[(i-1)*N + (j-1)] = x[i][j];
        }
    }

    vector<double> global_x;
    if (rank == 0) {
        global_x.resize(N * N);
    }

    MPI_Gatherv(local_flat.data(), local_rows * N, MPI_DOUBLE,
                global_x.data(), recvcounts.data(), displs.data(), MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    t_gather = MPI_Wtime();

    // if (rank == 0) {
    //     ofstream fout("jacobi2D_result.txt");
    //     fout << "Index\tValue\n";
    //     cout << "Matrix " << N << "x" << N << " after Jacobi:\n";
    //     for (int i = 0; i < N; i++) {
    //         for (int j = 0; j < N; j++) {
    //             fout << fixed << setprecision(6) << global_x[i*N + j] << " ";
    //             cout << fixed << setprecision(6) << global_x[i*N + j] << " ";
    //         }
    //         fout << "\n";
    //         cout << "\n";
    //     }
    //     fout.close();
    // }

    // MPI_Barrier(MPI_COMM_WORLD);
    // t_output = MPI_Wtime();

    MPI_Finalize();

    if (rank == 0) {
        // cout << "==== Timing Breakdown (seconds) ====\n";
        // cout << "Initialization: " << (t_init - t_start) << "\n";
        cout << "Computation: " << (t_compute - t_init) << "\n";
        // cout << "Gather: " << (t_gather - t_compute) << "\n";
        // cout << "Total: " << (t_init - t_start) << "\n";
    }
    return 0;
}
