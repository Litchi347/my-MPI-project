// default(none) 禁止 OpenMP 自动推断变量的共享属性，要求程序员显式声明
# include <omp.h>
# include <iostream>
using namespace std;

int x,y,z[1000];

#pragma omp threadprivate(x)

void default_none(int a){
    const int c = 1;
    int i = 0;

    #pragma omp parallel default(none) private(a,i) shared(z,y)
    {
        int j = omp_get_num_threads() - 1;

        a = z[j];
        x = c;
        z[i] = y;

        #pragma omp for firstprivate(y)
        for(i = 0;i < 10;i ++){
            z[i] = y;
        }
        z[i] = y;
        #pragma omp critical
        {
            cout << "Thread" << j << ": x= " << x << ",a = " << a << ", y = " << y << endl;
        }
    }
}

int main(){
    y = 5;
    for(int i = 0;i < 1000;i ++){
        z[i] = 0;
    }
    default_none(10);
    return 0;
}