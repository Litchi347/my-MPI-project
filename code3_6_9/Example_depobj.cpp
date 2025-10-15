// 依赖对象机制：
// 在复杂的场景下，依赖的对象可能会发生变化、或者依赖类型要动态修改
// 把一个可复用的依赖关系绑定到一个依赖对象上，以后只需要引用这个对象，就能自动继承对应的依赖属性
# include <stdio.h>
# include <omp.h>

#define N 100
#define TRUE 1
#define FALSE 0

void driver(int update,float a[],float b[],int n,omp_depend_t *obj);

void update_copy(int update,float a[],float b[],int n);
void checkpoint(float a[],int n);
void init(float a[],int n);

int main(){
    float a[N],b[N];
    omp_depend_t obj;

    // 初始化
    init(a,N);

    // 创建一个依赖对象 obj ，并把它初始化为"对数组 a 的 inout 依赖"
    #pragma omp depobj(obj) depend(inout: a)        //  初始化依赖对象，建立依赖关系

    driver(TRUE,a,b,N,&obj);                        // Task2 依赖于 Task2

    // 更新依赖对象
    #pragma omp depobj(obj) update(in)              // 修改依赖类型 inout 改为 in
    // 任何使用 depend(depobj: *obj) 的任务都只会对 a 进行只读依赖

    driver(FALSE,a,b,N,&obj);                       // 两个任务都是只读依赖，可以并行执行，没有写依赖冲突

    #pragma omp depobj(obj) destroy                 // 销毁依赖对象，释放资源
    return 0;
}

// Task1 -> Task2
// Task1 必须先执行完，Task2 才能开始
void driver(int update,float a[],float b[],int n,omp_depend_t *obj)
{
    #pragma omp parallel num_threads(2)
    #pragma omp single
    {
        #pragma omp task depend(depobj: *obj)        // Task1 对 a 有写依赖
            update_copy(update,a,b,n);

        #pragma omp task depend(in: a[:n])          // Task2 对 a 只读
            checkpoint(a,n);
    }
}

void update_copy(int update,float a[],float b[],int n){
    if(update){
        for(int i = 0;i < n;i ++){
            a[i] += 0.1f;                           // 对 a[] 进行写操作
        }
    }
    for(int i = 0;i < n;i ++){
        b[i] = a[i];                                // 对 a[] 进行读操作
    }
}

void checkpoint(float a[],int n){
    for(int i = 0; i < n;i ++){
        printf(" %f",a[i]);                         // 对 a[] 进行读操作
    }
    printf("\n");
}

void init(float a[],int n){
    for(int i = 0;i < n;i++){
        a[i] = i;
    }
}
