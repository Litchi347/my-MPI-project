# include <iostream>
# include <omp.h>
using namespace std;

int counter = 0;

#pragma omp threadprivate(counter)               // 将全局变量 counter 声明为线程私有变量，使得每个线程都有自己独立的副本

// threadprivate 的基本用法
int increment_counter()
{
    #pragma omp parallel
    {
        #pragma omp single
        {
            counter ++;
        }
        cout << "Thread" << omp_get_thread_num() << " :" << counter << endl;
    }
    return(counter);
}

// 错误例子！
// int increment_counter_2(){
//     static int counter = 0;
//     #pragma omp parallel
//     {
//         #pragma omp threadprivate(counter)       // 局部静态变量的存储位置属于函数作用域内，无法在编译期为所有线程分配独立副本
//         #pragma omp single
//         {
//             counter ++;
//         }
//         counter ++;
//         cout << "Thread" << omp_get_thread_num() << " :" << counter << endl;
//     }
//     return(counter);
// }

// 初始化的不确定性：threadprivate 变量的初始化在其第一次被访问前的某个时间点发生，确切时间不确定。若初始化依赖于其他变量的变化，会造成不确定结果
class T {                                        // 类定义
    public:
    int val;                                     // 成员变量
    T (int);                                     // 构造函数：使用 int 参数初始化 val
    T (const T&);                                // 拷贝构造函数：用另一个 T 对象初始化当前对象
};

T :: T (int v){                                  // 普通构造函数
    val = v;                                     // 将传入参数 v 赋值给成员变量 val（主线程）
}

T :: T (const T& t){                             // 拷贝构造函数
    val = t.val;                                 // 使用另一个对象的 val 初始化当前对象（主线程）
}

void g(T a,T b){
    a.val += b.val;                              // 修改副本的值，不影响原对象（其他线程）
}

int x = 1;
T a(x);                                          // 调用 T(int) 构造函数，a.val = x = 1

const T b_aux(x);                                // 常量对象，b_aux.val = x = 1

T b(b_aux);                                      // 调用拷贝构造函数，使用 b_aux 初始化 b ，b.val = 1（主线程）
#pragma omp threadprivate(a,b)                   // 声明 a 和 b 为线程私有

// 并行函数
void f(int n){
    x ++;                                        // 修改全局变量 x
    #pragma omp parallel for
    for(int i = 0; i < n;i++){
        g(a,b);                                  // 非主线程的其他线程的值，即副本的值可能在主线程值被定义的时候同步了副本的值，或者在创建并行区域的时候再同步，初始的值不确定
    }
}

// 类和静态成员的 threadprivate 用法
// 静态对象可以声明为 threadprivate ；每个线程都是执行相应的构造函数，对象销毁时也会调用析构函数
struct S{
    S();
    S(int);
    ~S();
    int t;
};

int f();

static S t1;
#pragma omp threadprivate(t1)

static S t2(23);
#pragma omp threadprivate(t2)

static S t3 = f();
#pragma omp threadprivate(t3)

// threadprivate 用于静态类成员的用法。必须将 threadprivate 指令放在类定义内。
// 静态类成员默认是所有对象共享的，加上 threadprivate 后，每个线程都有自己的静态成员副本
class T1{
    public:
    static int i;
    #pragma omp threadprivate(i);
};

int main(){
    // increment_counter();
    // increment_counter_2();
    f(5);
    return 0;
}