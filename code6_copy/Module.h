#pragma once
# include <mpi.h>
# include <omp.h>
# include <algorithm>

// 最多四个非阻塞通信：左发/左收/右发/右收
extern MPI_Request request[4];
extern MPI_Status status[4];

extern int ni;
extern int nj;
extern int ni_global;
extern int my_rank;
extern int num_procs;
extern int ni_local;
extern int ni_start;

extern double xa;
extern double xb;
extern double ya;
extern double yb;
extern double dx;
extern double dy;

extern double mu;

extern double U;
extern double V;
extern double tout;
extern double dtx;
extern double dty;
extern double dt;
extern double time0;
extern double cfl;

extern double* x;
extern double* y;
extern double** f;
extern double** fm1;
extern double** fm2;

void call_non_blockking_comm();
void call_mesh2d();
void call_initia();
void call_bounda();
void call_cflcon();
void call_solver();
void call_output(double num);
void call_releasespace();

