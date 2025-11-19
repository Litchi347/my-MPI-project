# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_initia(){
    time0 = 0;

    for(int i = 0;i < ni;i ++){
        f[i] = new double[nj];
        fm1[i] = new double[nj];
        fm2[i] = new double[nj];
    }

    for(int i = 0;i < ni;i ++){
        for(int j = 0;j < nj;j ++){
            f[i][j] = 0;
            fm1[i][j] = 0;
            fm2[i][j] = 0;
        }
    }

    for(int i = 0;i < ni;i ++){
        for(int j = 0;j < nj;j ++){
            if(y[j] <= 0.5 && y[j] >= 0.2 && x[i] <= 0.5 && x[i] >= 0.2){
                f[i][j] = 1;
            }
        }
    }
    return;
}