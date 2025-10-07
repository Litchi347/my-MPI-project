// firstprivate 子句
# include <omp.h>
# include <stdio.h>
#define NT 4

int main(){
    int section_count = 0;
    omp_set_dynamic(0);
    omp_set_num_threads(NT);

    #pragma omp parallel
    #pragma omp sections firstprivate(section_count)          // firstprivate 也是将括号内的变量设置为每个线程私有，但是会将初值设置成外层同名变量的值
    {
        #pragma omp section
        {
            section_count ++;
            printf("section_count %d\n",section_count);
        }
        #pragma omp section
        {
            section_count ++;
            printf("section_count %d\n",section_count);
        }
    }
    return 0;
}