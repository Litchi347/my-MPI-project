# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_cflcon(){
    dtx = dx * cfl / U;
    dty = dy * cfl / V;
    dt = min(dtx,dty);

    return;
}