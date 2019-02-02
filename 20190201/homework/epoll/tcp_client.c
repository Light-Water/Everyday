#include <func.h>

int main(int argc,char** argv)
{
    args_check(argc,3);
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==socketfd)
    {
        perror("socket");
        return -1;
    }
    printf("socketfd=%d\n",socketfd);
    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    ser.sin_addr.s_addr=inet_addr(argv[1]);

    int ret;
    ret=connect(socketfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    if(-1==ret)
    {
        perror("connect");
        return -1;
    }
    char buf[128]={0};
    int epfd=epoll_create(1);
    struct epoll_event event,evs[2];
    event.events=EPOLLIN;
    event.data.fd=STDIN_FILENO;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&event);
    if(-1==ret)
    {
        perror("epoll_ctl");
        return -1;
    }
    event.data.fd=socketfd;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,socketfd,&event);
    if(-1==ret)
    {
        perror("epoll_ctl");
        return -1;
    }
    int i,fdreadynum;
    while(1)
    {
        memset(evs,0,sizeof(evs));
        fdreadynum=epoll_wait(epfd,evs,2,-1);
        for(i=0;i<fdreadynum;i++)
        {
            if(EPOLLIN==evs[i].events&&STDIN_FILENO==evs[i].data.fd)
            {
                memset(buf,0,sizeof(buf));
                ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
                if(0==ret)
                {
                    printf("I will close\n");
                    break;
                }
                //send(socketfd,&ser,strlen(buf)-1,0);错误示例
                send(socketfd,buf,strlen(buf)-1,0);
            }
            if(socketfd==evs[i].data.fd)
            {
                memset(buf,0,sizeof(buf));
                ret=recv(socketfd,buf,sizeof(buf)-1,0);
                if(0==ret)
                {
                    printf("server close\n");
                    goto end;
                }
                printf("%s\n",buf);
            }
        }
    }
end:
    close(socketfd);
    return 0;
}

