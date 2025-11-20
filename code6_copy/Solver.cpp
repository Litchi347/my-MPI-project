# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_solver(){
    // 边界数据交换
    call_non_blockking_comm();

    call_bounda();

    // 局部计算
    #pragma omp parallel for default(shared)
    for(int i = 1;i <= ni_local;i ++){
        for(int j = 1;j < nj - 1; j ++){
            fm1[IDX(i,j)] = mu * dt * f[IDX(i,j)] * (1 - f[IDX(i,j)]) * (f[IDX(i,j)] - 0.5)
             - U * dt / dx * (f[IDX(i,j)] - f[IDX(i - 1,j)])
             + f[IDX(i,j)];
        }
    }

    #pragma omp parallel for default(shared)
    for(int i = 1;i <= ni_local;i ++){
        for(int j = 1;j < nj - 1;j ++){
            fm2[IDX(i,j)] = mu * dt * fm1[IDX(i,j)] * (1 - fm1[IDX(i,j)]) * (fm1[IDX(i,j)] - 0.5)
             - V * dt / dy * (fm1[IDX(i,j)] - fm1[IDX(i,j - 1)])
             + fm1[IDX(i,j)];
        }
    }

    #pragma omp parallel for default(shared)
    for(int i = 1;i <= ni_local; i++){
        for(int j = 1;j < nj - 1;j ++){
            f[IDX(i,j)] = fm2[IDX(i,j)];
        }
    }
    return;
}