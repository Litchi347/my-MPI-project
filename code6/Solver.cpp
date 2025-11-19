# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_solver(){
    for(int i = 1;i < ni;i ++){
        for(int j = 1;j < nj;j ++){
            fm1[i][j] = mu * dt * f[i][j] * (1 - f[i][j]) * (f[i][j] - 0.5)
             + U * dt / dx * (f[i - 1][j] - f[i][j])
             + f[i][j];
        }
    }

    for(int i = 1;i < ni;i ++){
        for(int j = 1;j < nj;j ++){
            fm2[i][j] = mu * dt * fm1[i][j] * (1 - fm1[i][j]) * (fm1[i][j] - 0.5)
             + V * dt / dy * (fm1[i][j - 1] - fm1[i][j])
             + fm1[i][j];
        }
    }

    for(int i = 1;i < ni; i++){
        for(int j = 1;j < nj;j ++){
            f[i][j] = fm2[i][j];
        }
    }
    return;
}