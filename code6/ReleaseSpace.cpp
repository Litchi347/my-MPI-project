# include "Module.h"
# include <iostream>
# include <iomanip>

using namespace std;

void call_releasespace(){
    for(int i = 0;i < ni;i ++){
        for(int j = 0;j < nj;j ++){
            delete[] f[i];
            f[i] = nullptr;

            delete[] fm1[i];
            fm1[i] = nullptr;

            delete[] fm2[i];
            fm2[i] = nullptr;
        }
        // delete[] f;
        // f = nullptr;

        // delete[] fm1;
        // fm1 = nullptr;

        // delete[] fm2;
        // fm2 = nullptr;

        delete[] x;
        x = nullptr;

        delete[] y;
        y = nullptr;
    }
    return;
}