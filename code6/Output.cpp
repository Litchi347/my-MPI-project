# include "Module.h"
# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string.h>

using namespace std;

void call_output(double num){
    char filename[30] = { 0 };

    snprintf(filename, sizeof(filename), "%f.dat", num);
    ofstream outfile(filename);

    outfile << "VARIABLEs = X,Y,fi" << "\n";
    outfile << "ZONE I=" << ni << "\t" << "J=" << nj - 1 << "\n";
    outfile << "datapacking = block" << "\n";

    for(int j = 0;j < nj;j ++){
        for(int i = 0; i < ni;i ++){
            outfile << x[i] << "\n";
        }
    }

    for(int j = 0;j < nj;j ++){
        for(int i = 0; i < ni;i ++){
            outfile << y[i] << "\n";
        }
    }

    for(int j = 0;j < nj;j ++){
        for(int i = 0;i < ni;i ++){
            outfile << f[i][j] << "\n";
        }
    }
    outfile.close();
    return;
}