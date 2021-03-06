#include <func.h>

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
    char buf[128]={0};
    int epfd=epoll_create(1);
    struct epoll_event event,evs[3];
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
                printf("new_fd=%d client ip=%s,port=%d\n",new_fd,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
                event.events=EPOLLIN|EPOLLET;//边沿触发
                event.data.fd=new_fd;
                ret=epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);

            }
            if(EPOLLIN==evs[i].events&&STDIN_FILENO==evs[i].data.fd);

            {
                memset(buf,0,sizeof(buf));
                ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
                if(0==ret)
                {
                    printf("I will close\n");
                    break;
                }
                send(new_fd,buf,strlen(buf)-1,0);
            }
            if(FD_ISSET(new_fd,&rdset))
            {
                memset(buf,0,sizeof(buf));
                ret=recv(new_fd,buf,sizeof(buf)-1,0);
                if(0==ret)
                {
                    printf("client close,byebye\n");
                    FD_CLR(new_fd,&recordset);
                    close(new_fd);
                }
                printf("%s\n",buf);
            }
        }
    }
    close(new_fd);
    close(socketfd);
    return 0;
}

