# include <omp.h>
void subdomain(float *x,int istart,int ipoints){
    for(int i=0;i<ipoints;i++){
        x[istart] = 123.456;
    }
}
void sub(float *x,int npoints){
    int iam,nt,ipoints,istart;

}
