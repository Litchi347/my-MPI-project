# include <math.h>

void lastpriv(int n,float *a,float *b){
    int i;

    #pragma omp parallel
    {
        // 主线程期望该变量的值是和顺序执行时一样的“最后一次更新值”
        #pragma omp for lastprivate(i)
        for(i = 0;i < n-1;i++){
            a[i] = b[i] + b[i+1];
        }
    }
    a[i] = b[i];
}

float condlastprivate(float *a,int n){
    float x = 0.0f;

    // 取最后一次执行了相关语句的 x 的值
    #pragma omp parallel for simd lastprivate(conditional:x)
    for(int k = 0;k < n;k++){
        if(a[k] < 108.5 || a[k] > 208.5){
            // x 返回的是最后一次迭代的满足a[k] < 108.5 || a[k] > 208.5的a[k]的正弦值
            x = sinf(a[k]);
        }
    }
    return x;
}