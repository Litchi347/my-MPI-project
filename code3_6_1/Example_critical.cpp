// critical 临界区：同时只允许一个线程执行该段代码
# include <stdio.h>
# include <stdio.h>
# include <omp.h>
# include <unistd.h>

# define N 10

int x_queue[N],y_queue[N];                                                // 任务队列
int x_front = 0,y_front = 0;                                              // 索引

// 从队列中取任务
int dequeue(float *a){
    int val = (int)(*a);
    if(val == 1){
        if(x_front < N){
            return x_queue[x_front++];
        }else{
            return -1;
        }
    }else{
        if(y_front < N){
            return y_queue[y_front++];
        }else{
            return -1;
        }
    }
}

void work(int i,float *a){
    if(i < 0){
        return;
    }
    char queue_name = (*a == 1.0f) ? 'X' : 'Y';
    printf("Thread %2d working on %c[%d]\n",omp_get_thread_num(),queue_name,i );
    sleep(1);
}

void critical_example(float *x,float *y){
    int ix_next,iy_next;

    // 使得对同一队列的操作互斥，但不同队列可以并行
    #pragma omp parallel shared(x,y) private(ix_next,iy_next)
    {
        while(1){
            // 从 x 队列取任务
            #pragma omp critical (xaxis) hint(omp_lock_hint_contended)    // 性能提示：该临界区竞争激烈：避免忙等待，忙时挂起
            {
                ix_next = dequeue(x);
            }
            if(ix_next == -1){
                break;
            }
            work(ix_next,x);

            // 从 y 队列取任务
            #pragma omp critical (yaxis) hint(omp_lock_hint_contended)
            {
                iy_next = dequeue(y);
            }
            if(iy_next == -1){
                break;
            }
            work(iy_next,y);
        }
    }
}

int main(){
    // 初始化任务队列
    for(int i = 0;i < N;i ++){
        x_queue[i] = i;
        y_queue[i] = i + 100;
    }

    float x_flag = 1.0f;
    float y_flag = 2.0f;

    double start = omp_get_wtime();
    critical_example(&x_flag,&y_flag);
    double end = omp_get_wtime();

    printf("Total time:%.2f seconds\n",end - start);
    return 0;
}