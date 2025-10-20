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
int increment_counter_2(){
    static int counter = 0;
    #pragma omp parallel
    {
        #pragma omp threadprivate(counter)       // 局部静态变量的存储位置属于函数作用域内，无法在编译期为所有线程分配独立副本
        #pragma omp single
        {
            counter ++;
        }
        counter ++;
        cout << "Thread" << omp_get_thread_num() << " :" << counter << endl;
    }
    return(counter);
}

// 初始化的不确定性
class T {                                        // 类定义
    public:
    int val;
    T (int);                                     // 构造函数
    T (const T&);                                // 拷贝构造函数
};

// 构造函数的实现
T :: T (int v){                                  // 普通构造函数
    val = v;
}

T :: T (const T& t){                             // 拷贝构造函数
    val = t.val;
}

void g(T a,T b){
    a.val += b.val;                              // 按值传递：会调用拷贝函数
}

int x = 1;
T a(x);                                          // 调用 T(int) 构造函数

// 创建一个类型为 const T ,初始化值为 x=1 的变量 b_aux
const T b_aux(x);                                // 常量对象

T b(b_aux);                                      // 调用拷贝构造函数
#pragma omp threadprivate(a,b);                  // 声明 a 和 b 为线程私有

// 并行函数
void f(int n){
    x ++;                                        // 修改全局变量 x
    #pragma omp parallel for
    {
        for(int i = 0; i < n;i++){
            g(a,b);
        }
    }
}


int main(){
    // increment_counter();
    increment_counter_2();

    return 0;
}