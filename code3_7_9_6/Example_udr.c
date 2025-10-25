// // 用户自定义归约
// # include <stdio.h>
// # include <limits.h>
// # include <omp.h>

// // 定义一个结构体 point
// struct point{
//     int x;
//     int y;
// };

// // // 先进行函数定义
// // void minproc(struct point *out,struct point *in)
// // {
// //     if(in->x < out->x){
// //         out->x = in->x;
// //     }
// //     if(in->y < out->y){
// //         out->y = in->y;
// //     }
// // };

// // void maxproc(struct point *out,struct point *in)
// // {
// //     if(in->x > out->x){
// //         out->x = in->x;
// //     }
// //     if(in->y > out->y){
// //         out->y = in->y;
// //     }
// // };

// // // omp_in 是被归约进来的另一个线程的私有副本
// // // omp_out 是当前线程的私有副本
// // // omp_priv 初始化局部变量
// // #pragma omp declare reduction(min:struct point:minproc(&omp_out,&omp_in))initializer(omp_priv = {INT_MAX,INT_MAX})
// // #pragma omp declare reduction(max:struct point:maxproc(&omp_out,&omp_in))initializer(omp_priv = {0,0})

// #pragma omp declare reduction(min:struct point:\
//     omp_out.x = omp_in.x > omp_out.x ? omp_out.x : omp_in.x, \
//     omp_out.y = omp_in.y > omp_out.y ? omp_out.y : omp_in.y) \
//     initializer(omp_priv = {INT_MAX,INT_MAX}))

// #pragma omp declare reduction(max:struct point:\
//     omp_out.x = omp_in.x < omp_out.x ? omp_out.x : omp_in.x, \
//     omp_out.y = omp_in.y < omp_out.y ? omp_out.y : omp_in.y) \
//     initializer(omp_priv = {0,0}))

// void find_enclosing_rectangle(int n,struct point points[]){
//     struct point minp = {INT_MAX,INT_MAX},maxp = {0,0};

//     #pragma omp parallel for reduction(min:minp) reduction(max:maxp)
//     for(int i = 0;i < n;i ++){
//         // 对线程内部的数据进行最大值和最小值的搜索
//         if(points[i].x < minp.x){
//             minp.x = points[i].x;
//         }
//         if(points[i].y < minp.y){
//             minp.y = points[i].y;
//         }
//         if(points[i].x > maxp.x){
//             maxp.x = points[i].x;
//         }
//         if(points[i].y > maxp.y){
//             maxp.y = points[i].y;
//         }
//         // minproc(&minp,&points[i]);
//         // maxproc(&maxp,&points[i]);
//     }
//     printf("min = (%d,%d)\n",minp.x,minp.y);
//     printf("max = (%d,%d)\n",maxp.x,maxp.y);
// }

// int main(){
//     struct point pts[] = {
//         {1,2},{3,5},{0,4},{2,1},{5,3}
//     };
//     int n = sizeof(pts) / sizeof(pts[0]);

//     find_enclosing_rectangle(n,pts);
//     return 0;
// }



// 另一个例子：找出一个数组中最大的元素值及它的索引
# include <stdio.h>
# define N 100

struct mx_s{
    float value;
    int index;
};

void mx_combine(struct mx_s *out,struct mx_s *in);
void mx_init(struct mx_s *priv,struct mx_s *orig);

// 使用init函数来对各线程副本初始化
#pragma omp declare reduction(maxloc:struct mx_s: \
    mx_combine(&omp_out,&omp_in)) \
    initializer(mx_init(&omp_priv,&omp_orig))

void mx_combine(struct mx_s *out,struct mx_s *in)
{
    if(out->value < in->value){
        out->value = in->value;
        out->index = in->index;
    }
}

void mx_init(struct mx_s *priv,struct mx_s *orig){
    priv->value = orig->value;
    priv->index = orig->index;
}

int main(void){
    struct mx_s mx;
    float val[N],d;
    int count = N;

    for(int i = 0; i < count ;i ++){
        d = (N * 0.8f - i);
        val[i] = N * N - d * d;
    }
    mx.value = val[0];
    mx.index = 0;

    #pragma omp parallel for reduction(maxloc:mx)
    for(int i = 1;i <count ;i++){
        if(mx.value < val[i]){
            mx.value = val[i];
            mx.index = i;
        }
    }
    printf("max value = %g,index = %d\n",mx.value,mx.index);
    return 0;
}