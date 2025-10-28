# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

void print_running_cpu(){
    char qry_cmd[1024] = {0};
    sprintf(qry_cmd,"ps -o pid,psr -p %d | tail -n 1 | awk {'print $2'}",getpid());
    FILE *fp = popen(qry_cmd,"r");
    if (fp == NULL){
        return;
    }
    char cpu_id_str[200] = {0};
    fgets(cpu_id_str,80,fp);
    fclose(fp);

    printf("current process %d is running on cpu(%d)\n",getpid(),atoi(cpu_id_str));
}

int main(){
    print_running_cpu();

    while (1)
    {
        long loop = 4000000000;
        while(loop--)
            ;                               // 循环执行空语句：CPU占用率达 100% ，方便观察程序在哪个核上运行
        sleep(0);                           // 每隔一段时间执行一次 sleep ,主动放弃 cpu 占用，让 Linux 重新调度，这样进程就有机会被切换到其他核心上执行
        printf("----------------\n");
        printf("switched !\n");
        print_running_cpu();
    }
    return 0;
    
}