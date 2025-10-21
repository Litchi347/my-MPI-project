// 展示了 firstprivate 子句在处理数组与指针类型变量时的行为
// 指针在作为私有副本的时候初始化方式为浅拷贝，各线程指向相同地址，可能会出现数据竞争问题
// 数组为各线程提供独立副本
# include <assert.h>

int A[2][2] = {1,2,3,4};

void f(int n,int B[n][n],int C[]){
    int D[2][2] = {1,2,3,4};
    int E[n][n];

    assert(n >= 2);
    E[1][1] = 4;

    #pragma omp parallel firstprivate(B,C,D,E)                // 为每个线程创建自己的副本，初值与主线程中的初值相同
    {
        // 验证
        assert(sizeof(B) == sizeof(int (*)[n]));              // B 是指向数组的指针
        assert(sizeof(C) == sizeof(int*));                    // C 是指针
        assert(sizeof(D) == 4 * sizeof(int));                 // D 是一个 2 X 2 的整型数组
        assert(sizeof(E) == n * n * sizeof(int));             // E 是 n x n 的整型数组

        // B 和 C 是指针类型而不是数组本身，它们的私有副本被浅拷贝了
        // 每个线程的 B 和 C 都仍然指向与原始 A 相同的内存区域
        assert(&B[1][1] == &A[1][1]);
        assert(&C[3] == &A[1][1]);
        assert(D[1][1] == 4);
        assert(E[1][1] == 4);

    }
}

int main(){
    // B = {1,2,3,4},C = {1,2}
    // D[2][2] = {1,2,3,4},E[1][1] = 4
    f(2,A,A[0]);
    return 0;
}