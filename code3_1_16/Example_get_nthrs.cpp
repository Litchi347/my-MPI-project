# include <omp.h>
# include <iostream>

using namespace std;

void work(int i){}

void incorrect(){
    int np = omp_get_num_threads();
    cout << "当前进程数是" << np << endl;

    #pragma omp parallel for schedule(static)       // 只有在并行内部才能显示当前并行运行的线程数
    {
        for(int i =0;i<np;i++){
            work(i);
        }
    }
}

int main(){
    incorrect();
    return 0;
}