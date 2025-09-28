// 在 OpenMP 的 parallel for 中，循环迭代变量 i 默认是私有的，每个线程都会有自己独立的 i
// 不需要private(i)
void simple(int n,float *a,float *b){
    int i;
#pragma omp parallel for                   // 把接下来的 for 循环并行化，自动把迭代分配给多个线程执行，循环变量默认私有，末尾有隐式同步
    for(int i=1;i<n;i++){
        b[i] = (a[i] + a[i-1]) / 2.0;
    }
}