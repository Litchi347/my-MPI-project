# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;
void call_bounda(){
    for(int i = 0;i <= ni_local + 1;i ++){
        f[IDX(i,0)] = 0.0;
        f[IDX(i,nj - 1)] = 0.0;

        fm1[IDX(i,0)] = 0.0;
        fm1[IDX(i,nj - 1)] = 0.0;
    }
}