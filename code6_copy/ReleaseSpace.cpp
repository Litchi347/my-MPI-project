# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_releasespace(){
    // 释放二维数组的内层指针
    for(int i = 0;i < ni_local + 2;i ++){
        delete[] f[i];
        delete[] fm1[i];
        delete[] fm2[i];
    }

    // 释放二维数组的外层指针
    delete[] f;
    delete[] fm1;
    delete[] fm2;

    f =  fm1 = fm2 = nullptr;

    if(x != nullptr){
        delete[] x;
        x = nullptr;
    }
    if(y != nullptr){
        delete[] y;
        y = nullptr;
    }
    return;
}