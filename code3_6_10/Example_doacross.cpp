// 跨迭代依赖示例
// 虽然使用了并行区域，实际执行仍是顺序的
# include <stdio.h>
# include <omp.h>

float foo(int i){
    return (float)i * 1.0f;
}

double foo(int i,int j){
    return (double)(i * 0.1 + j * 0.2);
}

float bar(float a,float b){
    return a + b * 0.5f;
}

float bar(float a,float b,float c){
    return a + b + c;
}

float baz(float b){
    return b * 2.0f;
}

// 单维依赖关系
// 去掉 depend 之后，OpenMP会认为所有的迭代之间都没有依赖关系，导致使用没有计算的数据二产生错误结果
void work(int N,float *A,float *B,float *C)
{
    // 循环中存在一级依赖网络，单变量依赖
    #pragma omp for ordered(1)
    for(int i = 1;i < N;i ++){
        A[i] = foo(i);

        // sink 依赖：等待前一迭代的数据准备好
        #pragma omp ordered depend(sink: i-1)
        {
            B[i] = bar(A[i],B[i-1]);
            printf("Thread %d -> B[%d] depends on B[%d] done -> B[%d] = %.2f\n",
                    omp_get_thread_num(),i,i-1,i,B[i]);
        }
            
        // source 依赖：表示当前迭代结果已经准备好，可以被后面的迭代使用
        #pragma omp ordered depend(source)
        {
            C[i] = baz(B[i]);
            printf("Thread %d -> C[%d] ready = %.2f\n",
                    omp_get_thread_num(),i,C[i]);
        }
    }
}

// 二维依赖版本
void work(int N,int M,float **A,float **B,float **C){
    #pragma omp for ordered(2)
    {
        for(int i = 1;i < N;i ++)
        {
            for(int j = 1;j < M;j ++)
            {
                A[i][j] = foo(i,j);

                // i 和 j 都依赖于前一项
                #pragma omp ordered depend(sink:i-1,j) depend(sink:i,j-1)
                {
                    B[i][j] = bar(A[i][j],B[i-1][j],B[i][j-1]);
                    printf("Thread %d -> B[%d][%d] depends on B[%d][%d] & B[%d][%d] -> B=%.2f\n",
                            omp_get_thread_num(),i,j,i-1,j,i,j-1,B[i][j]);
                }

                // 当前(i,j)结果准备好
                #pragma omp ordered depend(source)
                {
                    C[i][j] = baz(B[i][j]);
                    printf("Thread %d -> C[%d][%d] ready = %.2f\n",
                            omp_get_thread_num(),i,j,C[i][j]);
                }
            }
        }
    }
}

// 错误示例
void work_wrong(double p[][100][100])
{
    int i,j,k;
    #pragma omp parallel for ordered(2) private(i,j,k)
    for(i = 1;i < 100-1;i++){
        for(j = 1;j < 100 -1;j++){
            #pragma omp ordered depend(sink:i-1,j) depend(sink:i+1,j)\
            depend(sink:i,j-1) depend(sink:i,j+1)
            for(k = 1;k< 100-1;k ++){
                double tmp1 = p[i-1][j][k] + p[i+1][j][k];
                double tmp2 = p[i][j-1][k] + p[i][j+1][k];
                double tmp3 = p[i][j][k-1] + p[i][j][k+1];
                p[i][j][k] = (tmp1 + tmp2 + tmp3) / 6.0;

            }
            // 缺少"#pramp omp oedered depend(source)，依赖链会断裂
        }
    }
}

// 二维 doacross 示例
void work(int N,int M,double **A,double **B,double **C){
    double alpha = 1.2;

    // 将两个循环(i,j)合并成一个单一的2D迭代空间
    // 迭代之间可能存在二维依赖关系
    #pragma omp for collapse(2) ordered(2)
    for(int i = 1;i < N-1;i ++){
        for(int j = 1;j < M-1;j ++){

            A[i][j] = foo(i,j);

            // 当前(i,j)迭代完成了，计算结果可以被后续依赖的迭代使用了
            #pragma omp ordered depend(source)
            {
                B[i][j] = alpha * A[i][j];
                printf("Thread %d -> B[%d][%d] ready = %.2f\n",omp_get_thread_num(),i,j,B[i][j]);
            }

            #pragma omp ordered depend(sink:i-1,j) depend(sink:i,j-1)
            {
                C[i][j] = 0.2 * (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1] + A[i][j]);
                printf("Thread %d -> C[%d][%d] depends on (%d,%d)&(%d,%d) done -> C=%.2f\n",omp_get_thread_num(),i,j,i-1,j,i,j-1,C[i][j]);
            }
        }
    }
}

int main(){

    omp_set_num_threads(4);
    int N = 10;
    float A[10],B[10],C[10];

    B[0] = 1.0f;
    work(N,A,B,C);

    printf("\nFinal result:\n");
    for(int i = 1;i < N;i ++){
        printf("C[%d] = %.2f\n",i,C[i]);
    }

    // 二维测试
    float a2[10][5],b2[10][5],c2[10][5];
    float *pa[10],*pb[10],*pc[10];
    for(int i = 0;i < 10;i ++){
        pa[i] = a2[i];
        pb[i] = b2[i];
        pc[i] = c2[i];
    }
    work(5,5,pa,pb,pc);

    return 0;
}

/*
结果几乎全部由 Thread0 打印，看起来像串行：
依赖将执行序列化了：将每个格子的执行和它上方、左方的格子的完成挂钩，形成“波前依赖”
有些OpenMP实际会把按序依赖的连续可运行任务分配给同一线程以减少开销
*/