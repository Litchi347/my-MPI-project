# include "Module.h"

MPI_Request request[4];
MPI_Status status[4];

int my_rank = 0;
int num_procs = 1;
int ni_local = 0;
int ni_start = 0;

int ni = 201;
int nj = 201;
int ni_global = ni;

double xa = 0.0;
double xb = 1.0;
double ya = 0.0;
double yb = 1.0;
double cfl = 0.8;
double U = 1.0;
double V = 2.0;
double tout = 0.2;
double mu = 100;

double dx = 0.0;
double dy = 0.0;
double dtx = 0.0;
double dty = 0.0;
double dt = 0.0;
double time0 = 0.0;
double* x = nullptr;
double* y = nullptr;

// 网格数组的维度更改为局部维度 + 2个 Ghost Cells(Ghost Cells用于边界通信)
double* f = nullptr;
double* fm1 = nullptr;
double* fm2 = nullptr;
