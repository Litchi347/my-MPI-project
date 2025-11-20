# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_initia(){
    time0 = 0;

    // 重新分配f,fm1,fm2 ，使用ni_local + 2(左ghost，内部，右ghost)
    long long totalsize = (long long)(ni_local + 2) * nj;
    f = new double [totalsize];
    fm1 = new double [totalsize];
    fm2 = new double [totalsize];

    // 初始化为0
    fill(f, f + totalsize ,0.0);
    fill(fm1, fm1 + totalsize ,0.0);
    fill(fm2, fm2 + totalsize ,0.0);

    // 内部计算域是从 i = 1 到 ni_local
    // i=0 是左Ghost, i=ni_local+1 是右Ghost
    for(int i = 1;i <= ni_local;i ++){
        int i_global = ni_start + (i - 1);
        for(int j = 0; j < nj;j ++){
            // 方波初始条件
            if(y[j] <= 0.5 && y[j] >= 0.2 && x[i_global] <= 0.5 && x[i_global] >= 0.2){
                f[IDX(i,j)] = 1.0;
            }
            else{
                f[IDX(i,j)] = 0.0;
            }
        }
    }
    return;
}