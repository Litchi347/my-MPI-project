# include <stdio.h>

void work(int k){
    #pragma omp ordered
        printf(" %d\n",k);
}

void ordered_example(int lb,int ub,int stride){

    // order 指定代码必须按顺序执行
    // schedule (dynamic) 某个线程完成当前迭代之后，在领取下一个未完成的任务1
    #pragma omp parallel for ordered schedule(dynamic)
    for (int i = lb;i < ub;i += stride){
        work(i);
    }
}

void ordered_wrong(int n){
    int i;
    #pragma omp for ordered
    for(int i = 0;i < n;i ++){
        // 错误示例：
        // 对于包含 ordered 子句的循环体，循环体内部必须且只能有一个 #pragma omp ordered 指令
        #pragma omp ordered
        {
            work(i);
        }
        #pragma omp ordered
        {
            work(i+1);
        }
    }
}

void ordered_good(int n){

    #pragma omp for ordered
    for(int i = 0;i < n;i ++){
        if(i <= 10){
            #pragma omp ordered
            work(i);
        }
        if(i > 10){
            #pragma omp ordered
            work(i + 1);
        }
    }
}

int main()
{
    // ordered_example(0,100,5);
    // ordered_wrong(10);
    ordered_good(10);
    return 0;
}