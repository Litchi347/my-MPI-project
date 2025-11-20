# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_cflcon(){
    dx = (xb - xa) / (ni_global - 1);
    dy = (yb - ya) / (nj - 1);
    dt = cfl / (U/dx + V/dy);

    return;
}