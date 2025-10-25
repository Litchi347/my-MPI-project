# include <stdio.h>
# define N 100

int main(){
    int a[N],b[N];
    int x = 0;

    // 初始化
    for(int k = 0; k < N;k ++){
        a[k] = k + 1;
    }

    #pragma omp parallel for simd reduction(inscan,+:x)
    for(int k = 0;k < N;k ++){
        x += a[k];
        #pragma omp scan inclusive(x)            // 对一个序列中的元素，从头到当前位置依次进行累加操作
        // #pragma omp scan enclusive(x)            // inclusive 包含自身；exclusive 不包含自身
        b[k] = x;
    }
    printf("x = %d,b[0:3] = %d %d %d\n",x,b[0],b[1],b[2]);
    return 0;

}