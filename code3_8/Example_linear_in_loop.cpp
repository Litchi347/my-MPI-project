# include <stdio.h>
# define N 100
int main(){
    float a[N],b[N/2];

    for(int i = 0;i < N;i++){
        a[i] = i+1;
    }
    int j = 0;
    #pragma omp parallel
    // 在本层 for 循环中，j 也会随着迭代增长，每次增加 1
    #pragma omp for linear(j:1)
    for(int i = 0;i < N;i +=2){
        b[j] = a[i] * 2.0f;
        j ++;                                     // 实际操作：j 确实会就加 1
    }
    printf("%d %f %f\n",j,b[0],b[j - 1]);

    return 0;
}