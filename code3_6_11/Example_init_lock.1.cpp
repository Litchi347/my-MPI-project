# include <stdlib.h>
# include <stdio.h>
# include <omp.h>

// // // 使用 omp_init_lock 初始化一个锁数组
// // omp_lock_t *new_locks(){
// //     int i;
// //     omp_lock_t *lock = new omp_lock_t[1000];

// //     #pragma omp parallel for private(i)
// //     {
// //         for (i = 0;i < 1000;i ++){
// //             omp_init_lock(&lock[i]);
// //         }
// //     }
// //     return lock;
// // }

// // 初始化提示
// omp_lock_t *new_locks(){
//     int i;
//     omp_lock_t *lock = new omp_lock_t[1000];
//     #pragma omp parallel for private(i)
//     {
//         for(i = 0; i < 1000;i ++){
//             // 带有性能优化提示，告诉OpenMP运行时这个锁可能会被频繁竞争
//             omp_init_lock_with_hint(&lock[i],
//             static_cast<omp_lock_hint_t>(omp_lock_hint_contended | omp_lock_hint_speculative));
//             // omp_lock_hint_contended 表示该锁可能会被多个线程频繁竞争，运行时可使用更高效的自旋锁策略或缓存行分离来降低冲突
//             // omp_lock_hint_speculative 表示该锁可能适合使用推测锁技术，以减少锁等待时间
//         }
//     }
//     return lock;
// }

// // 解释锁所有权变化
// int main(){
//     int x;
//     omp_lock_t lck;

//     omp_init_lock(&lck);
//     omp_set_lock(&lck);                // 主线程加锁

//     x = 0;

//     #pragma omp parallel shared(x)
//     {
//         #pragma omp master             // 仅主线程执行
//         {
//             x = x + 1;
//             omp_unset_lock(&lck);      // 在 master 区域中释放锁
//         }
//     }
//     omp_destroy_lock(&lck);
//     return 0;
// }


// 以下例子演示了 OpenMP 锁机制的使用方式，展示了互斥访问、尝试锁定和锁竞争时的行为
void skip(int i){}
void work(int i){}

int main(){
    omp_lock_t lck;
    int id;
    omp_init_lock(&lck);               // 初始化锁
    #pragma omp parallel shared(lck) private(id)
    {
        id = omp_get_thread_num();

        // 互斥访问
        omp_set_lock(&lck);            // 线程尝试获取锁如果锁被其他线程占用，它就会阻塞等待

        // 位于加锁区内，保证同一时刻只有一个线程在打印
        printf("My thread id is %d.\n",id);
        omp_unset_lock(&lck);          // 石方锁，让其他线程有机会进入

        // 尝试锁定 + 执行逻辑
        while (!omp_test_lock(&lck)){  // 非阻塞尝试上锁。能获得锁返回true，锁被占用返回false
        // 与 omp_set_lock 的不同：不会阻塞等待，可以立即得到一个结果
            skip(id);                  // 如果暂时拿不到锁，调用 skip 执行一些非关键任务
        }
        work(id);                      // 拿到锁后执行一些关键任务然后释放锁
        omp_unset_lock(&lck);
    }
    omp_destroy_lock(&lck);
    return 0;
}

// 可嵌套锁
typedef struct{
    int a,b;
    omp_nest_lock_t lck;
}pair;

int work1();
int work2();
int work3();

// 只修改 a
void incr_a(pair *p,int a)
{
    p->a += a;
}

// 只修改 b
void incr_b(pair *p,int b)
{
    omp_set_nest_lock(&p ->lck);
    p->b += b;
    omp_unset_nest_lock(&p ->lck);
}

// 同时修改 a 和 b
void incr_pair(pair *p,int a,int b){
    omp_set_nest_lock(&p ->lck);
    incr_a(p,a);
    incr_b(p,b);
    omp_unset_nest_lock(&p->lck);
}

// 创建两个并行section：
void nestlock(pair *p){
    #pragma omp parallel sections
    {
        // 这两个线程可以同时修改 b ，锁的存在保证了安全访问
        #pragma omp section
        {
            incr_pair(p,work1(),work2());
        }
        #pragma omp section
        {
            incr_b(p,work3());
        }
    }
}