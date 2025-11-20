# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_releasespace(){
    if(f){
        delete[] f;
        f = nullptr;
    }

    if(fm1){
        delete[] fm1;
        fm1 = nullptr;
    }

    if(fm2){
        delete[] fm2;
        fm2 = nullptr;
    }

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