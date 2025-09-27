# include <stdio.h>
# include <omp.h>

int main(){
    int i,n=10;
    float a[10],b[10],c[10];
    for(int i=0;i<n;i++){
        a[i] = i * 1.0;
        b[i] = i * 2.0;
    }

    #pragma omp parallel for
    for(int i= 0;i<n;i++){
        c[i] = a[i] + b[i];
    }

    for(i=0;i<n;i++){
        printf("%f\n",c[i]);
    }
    return 0;
}
// void simple(int n,float *a,float *b){
//     int i;
//     for(int i=1;i<n;i++){
//         b[i] = (a[i] + a[i-1]) / 2.0;      // i默认情况下是私有的
//     }
// }