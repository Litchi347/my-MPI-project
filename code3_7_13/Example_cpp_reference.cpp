// 引用类型数据共享

// 任务要执行的主体函数：接收一个整型引用参数，即对某个变量的引用
void task_body(int &);

// 用来创建一个任务
void gen_task(int &x){
    #pragma omp task
    task_body(x);
}

void test(int &y,int &z){
    // 将引用型变量 y 设置成私有，其指向的对象也是每个线程私有的
    #pragma omp parallel private(y)
    {
        y = z + 2;
        gen_task(y);
        gen_task(z);

        y ++;
        gen_task(y);
    }
}