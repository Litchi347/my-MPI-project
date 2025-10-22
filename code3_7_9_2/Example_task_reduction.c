// 任务归约机制，展示了如何在并行任务(task)和工作共享循环(for)中进行任务级别的归约操作
# include <stdlib.h>
# include <stdio.h>
// # define N 10

// 遍历链表时执行的归约过程
// 链表
typedef struct node_tag{
    int val;
    struct node_tag *next;
} node_t;

int linked_list_sum(node_t *p){
    int res = 0;
    # pragma omp taskgroup task_reduction(+:res)
    {
        node_t* aux = p;
        while (aux != 0)
        {
            #pragma omp task in_reduction(+:res)
            res += aux->val;

            aux = aux->next;
        }
    }
    return res;
}

// int main(int argc,char *argv[]){
//     // 创建根节点
//     node_t* root = (node_t*) malloc (sizeof(node_t));
//    root->val = 1;

//    node_t* aux = root;                                         // 临时指针，指向当前节点

//     // 创建前 n-1 个节点
//     for(int i = 2;i <= N;++i){
//         aux->next = (node_t*) malloc(sizeof(node_t));
//         aux = aux->next;
//         aux->val = i;
//     }

//     aux -> next = 0;
//     #pragma omp parallel 
//     #pragma omp single
//     {
//         int result = linked_list_sum(root);
//         printf("Calculated :%d Analytic%d\n",result,(N*(N+1)/2));
//     }
//     return 0;
// }

// 在 OpenMP 5.0 之前，如果想在任务之间做归约，必须显式包一层 taskgroup 
// 但在 OpenMP 5.0 ，可以直接在 parallel 或 for 里写 
// #pragma omp parallel reduction(task, +:x) 来实现相同效果
int main(void){
    int N = 100,M = 10;

    // CASE 1：显式任务参与 parallel 区域的归约
    int x = 0;
    // 不仅做线程级归约，还要把任务也包含进来
    #pragma omp parallel num_threads(M) reduction(task,+:x)
    {
        x ++;                                                   // 隐式任务（OpenMP自动创建的任务，每一个 parallel 就是一个任务）中的归约操作

        // 线程会创建 N 个任务
        #pragma omp single
        for (int i = 0;i < N;i ++)
            #pragma omp task in_reduction(+:x)
            x++;
    }
    printf("x= %d = M+N\n",x);                                  // M 个线程贡献 1 + N 个任务贡献 1

    // CASE 2：任务出现在 parallel for 循环中
    x = 0;
    #pragma omp parallel for num_threads(M) reduction(task,+:x) // x 的加法归约结果需要合并所有任务的贡献
    for(int i = 0;i < N;i ++){
        x ++;
        if(i % 2 == 0){
            #pragma omp task in_reduction(+:x)
            x --;
        }
    }
    printf("x= %d = N-N/2\n",x);
    return 0;
}