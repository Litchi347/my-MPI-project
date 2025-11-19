# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_initia(){
    time0 = 0;

    // 重新分配f,fm1,fm2 ，使用ni_local + 2(左ghost，内部，右ghost)
    f = new double* [ni_local + 2];
    fm1 = new double* [ni_local + 2];
    fm2 = new double* [ni_local + 2];

    // 循环到 ni_local + 2
    for(int i = 0;i < ni_local + 2;i ++){
        f[i] = new double[nj];
        fm1[i] = new double[nj];
        fm2[i] = new double[nj];
    }

    // 初始化和赋值循环需要使用局部索引和全局坐标
    for(int i = 0;i < ni_local;i ++){                  // 局部索引从1到ni_local
        int i_global = ni_start + (i - 1);             // 映射到全局 I 坐标

        for(int j = 0;j < nj;j ++){
            f[i][j] = 0;
            fm1[i][j] = 0;
            fm2[i][j] = 0;

            if(y[j] <= 0.5 && y[j] >= 0.2 && x[i_global] <= 0.5 && x[i_global] >= 0.2){
                f[i][j] = 1;                           // 使用局部索引
            }
        }
    }
    return;
}