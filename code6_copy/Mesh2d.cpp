# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_mesh2d(){
    x = new double[ni_global];
    y = new double[nj];

    dx = (xb - xa) / (ni_global - 1);
    dy = (yb - ya) / (nj - 1);

    for(int i = 0;i < ni_global;i ++){
        x[i] = xa + dx * i;
    }

    for(int j = 0;j < nj;j ++){
        y[j] = ya + dy * j;
    }    
    return;
}