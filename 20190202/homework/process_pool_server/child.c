#include "pro_pool_server.h"

void makeChild(proData* pManage,int proNum)
{
    int i;
    pid_t pid;
    int fds[2];
    for(i=0;i<proNum;i++)
    {
        socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
        //每次在fork之前都要初始化一管道，一定要放在for循环内
        pid=fork();
        if(!pid)
        {
            close(fds[1]);//关闭写端
            childHandle(fds[0]);//读端
        }
        //父进程，
        close(fds[0]);//关闭读端
        pManage[i].pid=pid;//进程
        pManage[i].fds=fds[1];//管道
    }
    
}

void childHandle(int fds)
{
    int new_fd;
    char flag=1;
    while(1)
    {
        recv_fd(fds,&new_fd);
        printf("I am child,get task,send file start\n");
        close(new_fd);
        write(fds,&flag,sizeof(char));//通知父进程我完成任务
    }
}

