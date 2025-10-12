// 不正确的写法：同一个内存空间不能通过不同类型进行原子访问
// 规范：所有对同一内存位置执行的原子操作，必须是相同类型、相同语义的操作
void atomic_wrong()
{
    union{int n;float x;} u;         // union 的所有成员变量共享一块存储空间
    #pragma omp parallel
    {
        #pragma omp atomic update    // 第一次：对 u.n (int) 做原子操作
            u.n++;
        #pragma omp atomic update    // 第一次：对 u.x (float) 做原子操作
            u.x += 1.0;
    }
}