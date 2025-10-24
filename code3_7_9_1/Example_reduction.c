// 规约子句 reduction 的各种用法
# include <math.h>
# include <limits.h>
# include <stdio.h>

# define N 100

void init(int n,float (*b)[N]){
    for(int i =0;i<n;i++){
        for(int j = 0;j < N;j ++){
            b[i][j] = (float)(i + j);
        }
    }
}

// 自动 reduction
void reduction1(float *x,int *y,int n)
{
    int i,b,c;
    float a,d;
    a = 0.0;
    b = 0;
    c = y[0];
    d = x[0];
    /*
    reduction(+:a)：每个线程维护一个局部副本 a_local，循环结束后所有线程的 a_local 会被加和到全局 a；
    reduction(^:b)：对每个线程的局部 b 执行按位异或 (^) 归约；
    reduction(min:c)：对各线程的局部 c 取最小值；
    reduction(max:d)：对各线程的局部 d 取最大值
    */
    #pragma omp parallel for private(i) shared(x,y,n) reduction(+:a) reduction(^:b) reduction(min:c) reduction(max:d)
    for(i = 0;i < n;i ++){
        a += x[i];
        b ^= y[i];
        if(c > y[i]){
            c = y[i];
        }
        d = fmaxf(d,x[i]);              // 计算两个浮点数的最大值
    }
}

// 手动 reduction
void reduction2(float *x,int *y,int n){
    int i,b,b_p,c,c_p;
    float a,a_p,d,d_p;
    a = 0.0f;
    b = 0;
    c = y[0];
    d = x[0];

    #pragma omp parallel shared(a,b,c,d,x,y,n) private(a_p,b_p,c_p,d_p)
    {
        a_p = 0.0f;
        b_p = 0;
        c_p = INT_MAX;
        d_p = -HUGE_VALF;
        #pragma omp for private(i)
        for(i = 0;i < n;i ++){
            a_p += x[i];
            b_p ^= y[i];
            if(c_p > y[i])
            {
                c_p = y[i];
            }
            d_p = fmaxf(d_p,x[i]);
        }
        #pragma omp critical
        {
            a += a_p;
            b ^= b_p;
            if(c > c_p){
                c = c_p;
            }
            d = fmaxf(d,d_p);
        }
    }
}

// // 不符合规范
// int main(void){
//     int a,i;
//     #pragma omp parallel shared(a) private(i)
//     {
//         #pragma omp master
//         a = 0;
//         // 变量 a 的初始化与并行区中对它的使用之间没有同步机制，可能主线程中的 a 还没有被初始化，其他的线程的 a 已经开始迭代了
//         #pragma omp for reduction(+:a)
//         for(i = 0;i < 10;i ++){
//             a += i;
//         }
//         #pragma omp single
//         printf("Sum is %d\n",a);
//     }
//     return 0;
// }

// 如何在 OpenMP 中安全地对整个数组进行并行加法归约
int main(){
    int i,j;
    float a[N],b[N][N];

    init(N,b);
    for(i = 0;i < N;i ++){
        a[i] = 0.0e0;
    }

    // OpenMP 会自动为每个线程创建一个私有副本的数组，并在循环解释后将这些副本主元素相加，合并到共享的 a 中
    #pragma omp parallel for reduction(+:a[0:N]) private(j)
    for(i = 0;i < N;i ++){
        for(j = 0;j < N;j ++){
            a[j] += b[i][j];
        }
    }        
    printf("a[0] a[N-1]:%f %f\n",a[0],a[N-1]);
    return 0;
}