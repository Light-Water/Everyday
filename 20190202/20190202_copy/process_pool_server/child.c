#include "pro_pool_server.h"

void makeChild(proData* pManage,int proNum)
{
    int i;
    pid_t pid;
    int fds[2];
    for(i=0;i<proNum;i++)
    {
        socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
        pid=fork();
        if(!pid)
        {
            close(fds[1]);
            childHandle(fds[0]);
        }
        close(fds[0]);
        pManage[i].pid=pid;
        pManage[i].fds=fds[1];
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
        write(fds,&flag,sizeof(char));//通知父进程我完成任务了
    }
}
