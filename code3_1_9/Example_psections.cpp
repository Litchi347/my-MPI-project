// 把一组互相独立的任务（非循环迭代）分配给多个线程去执行
void XAXIS();
void YAXIS();
void ZAXIS();
void sect_example(){
    #pragma omp parallel sections
    {
        #pragma omp section
        XAXIS();

        #pragma omp section
        YAXIS();

        #pragma omp section
        ZAXIS();

    }
}