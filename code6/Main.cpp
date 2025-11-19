# include "Module.h"
# include <iostream>
# include <iomanip>
# include <fstream>
# include <sstream>

using namespace std;

void call_mesh2d();
void call_initia();
void call_bounda();
void call_cflcon();
void call_solver();
void call_output(double num);
void call_releasespace();

int main(){
    call_mesh2d();
    call_initia();
    call_cflcon();
    call_bounda();

    do{
        call_solver();
        time0 = time0 + dt;
        cout << "Time=" << "\t" << time0 << endl;
        call_output(time0/dt);
    } while(time0 <= tout);
    call_releasespace();
    return 0;
}