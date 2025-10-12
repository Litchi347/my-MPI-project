// 使用 atomic 保证操作原子性，避免竞态问题
// atomic 在一个简单自旋锁上的应用
# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <unistd.h>

float work1(int i)
{
    return 1.0 * i;
}

float work2(int i)
{
    return 2.0 * i;
}

void atomic_example(float *x,float *y,int *index,int n)
{
    #pragma omp parallel for shared(x,y,index,n)
    {
        for(int i = 0;i < n;i ++){
            #pragma omp atomic update
            x[index[i]] += work1(i);                 // x[] 的值会经历多次修改，存在潜在的竞态问题。使用 atomic 保证操作的原子性
            y[i] += work2(i);
            if(i % 100 == 0){
                printf("x[%d]: %.2f,y[%d]: %.2f\n",index[i],x[index[i]],i,y[i]);
            }
        }
    }
}

// int main(){
//     // 初始化
//     float x[1000];
//     float y[10000];
//     int index[10000];

//     for(int i = 0;i < 10000;i ++){
//         index[i] = i % 1000;                         // 1000 个数一组
//         y[i] = 0.0;
//     }
//     for(int i = 0;i < 1000;i ++){
//         x[i] = 0.0;
//     }

//     atomic_example(x,y,index,10000);
//     return 0;
// }

// 原子获取函数
int atomic_read(const int *p){
    int value;
    #pragma omp atomic read                          // 确保原子性地读取 *p 的整个值。在读取操作期间，*p的任何部分都不得改变
        value = *p;
    return value;
}

int atomic_write(int *p,int value){
    #pragma omp atomic write                         // 确保值以原子方式存储到 *p 中, *p 不能S更改直至整个写入操作全部完成
        *p = value;
}

// 原子获取并自增函数
int fetch_and_add(int *p){
    int old;
    #pragma omp atomic capture
    {
        old = *p;
        (*p) ++;
    }
    return old;
}

// 定义锁类型
struct locktype{
    int ticketnumber;
    int turn;
};

void work(int tid){
    printf("Threead %d is doing locked work\n",tid);
    usleep(100000);                                  // 模拟耗时操作
}

// 使用票据锁实现的互斥访问
void do_locked_work(struct locktype *lock)
{
    // 每个线程都唯一地取一个票号
    int myturn = fetch_and_add(&lock -> ticketnumber);
    
    // 轮询等待：只有到了属于自己的 turn 才能进入临界区
    while (atomic_read(&lock->turn) != myturn)
    {
        #pragma omp flush
    }

    // 临界区（获得锁）
    #pragma omp flush
    work(omp_get_thread_num());

    // 解锁
    #pragma omp atomic update
    lock->turn++;

    #pragma omp flush
};

int main(){
    struct locktype mylock = {0,0};
    int nthreads = 4;

    #pragma omp parallel num_threads(nthreads) shared(mylock)
    {
        do_locked_work(&mylock);
    }
    return 0;
}