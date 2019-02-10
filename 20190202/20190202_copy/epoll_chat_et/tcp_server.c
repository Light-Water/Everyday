#include <func.h>
void changeNonblock(int fd)
{
    int status=fcntl(fd,F_GETFL);
    status=status|O_NONBLOCK;//改为非阻塞
    fcntl(fd,F_SETFL,status);
}

int main(int argc,char *argv[])
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
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    listen(socketfd,10);
    int new_fd=-1;//通过new_fd与客户端发送与接收数据
    struct sockaddr_in client;
    char buf[10]={0};
    int epfd=epoll_create(1);
    struct epoll_event event,evs[3];
    event.events=EPOLLIN;//读事件
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
        perror("epoll_ctl1");
        return -1;
    }
    printf("line num =%d\n",__LINE__);
    int i,fdreadynum;
    while(1)
    {
next:
        memset(evs,0,sizeof(evs));
        fdreadynum=epoll_wait(epfd,evs,3,-1);
        for(i=0;i<fdreadynum;i++)
        {
            if(evs[i].data.fd==socketfd)
            {
                socklen_t addrlen=sizeof(client);
                memset(&client,0,sizeof(client));
                new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
                if(-1==new_fd)
                {
                    perror("accept");
                    return -1;
                }
                printf("client ip=%s,port=%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
                changeNonblock(new_fd);
                event.events=EPOLLIN|EPOLLET;//边沿触发
                event.data.fd=new_fd;
                ret=epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);
            }
            if(EPOLLIN==evs[i].events&&STDIN_FILENO==evs[i].data.fd)
            {
                memset(buf,0,sizeof(buf));
                ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
                if(0==ret)
                {
                    printf("I will close\n");
                    goto end;
                }
                send(new_fd,buf,strlen(buf)-1,0);
            }
            if(new_fd==evs[i].data.fd)
            {
                while(1)
                {
                    memset(buf,0,sizeof(buf));
                    ret=recv(new_fd,buf,sizeof(buf)-1,0);
                    if(0==ret)
                    {
                        printf("client close,byebye\n");
                        event.data.fd=new_fd;
                        event.events=EPOLLIN|EPOLLET;
                        ret=epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd,&event);
                        if(-1==ret)
                        {
                            perror("epoll_ctl");
                            return -1;
                        }
                        goto next;
                    }else if(-1==ret&&errno==EAGAIN)
                    {
                        break;//读取完毕
                    }
                    printf("%s",buf);
                }
                printf("\n");
            }
        }
    }
end:
        close(new_fd);
        close(socketfd);
        return 0;
    }

