# include <omp.h>
# include <stdio.h>

void subdomain(float *x,int istart,int ipoints){
    int i;

    for(i = 0;i < ipoints;i++){
        x[istart + i] = 123.456;
    }
}

// 把数组x的npoints个元素平均分给多个线程，每个线程只处理自己负责的一段
void sub(float *x,int npoints){
    int iam,nt,ipoints,istart;

#pragma omp parallel default(shared) private(iam,nt,ipoints,istart)
// #pragma omp parallel 从此处开始创建一个并行区域，程序生成多个线程，线程数默认等于机器的逻辑CPU核数，也可以通过环境变量设置
// default(shared) 默认情况下，变量在并行区里是共享的，所有线程访问同一份内存，一个线程写入会影响其他线程读到的值
// private(iam,nt,ipoints,istart) 这几个变量在并行区是私有的，每个线程有自己独立的副本，互不干扰
    {
        iam = omp_get_thread_num();               // 线程编号
        nt = omp_get_num_threads();               // 线程总数
        ipoints = npoints / nt;                   // 分配的任务数
        istart = iam * ipoints;                   // 子区起始位置
        if(iam == nt - 1){
            ipoints = npoints - istart;
        }
        printf("线程 %d / %d: 负责下标 [%d, %d]\n", iam, nt, istart, istart + ipoints - 1);

        subdomain(x,istart,ipoints);
    }
}

int main(){
    float array[10000];
    sub(array,10000);
    return 0;
}
