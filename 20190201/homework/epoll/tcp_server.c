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
   
    

    //立即重连,要放在accept之前，不然不生效
    int reuse=1;
    int ret;
    ret=setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    if(-1==ret)
    {
        printf("line=%d\n",__LINE__);
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    ser.sin_addr.s_addr=inet_addr(argv[1]);

    
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    listen(socketfd,10);
    int new_fd;
    struct sockaddr_in client;
    socklen_t addrlen=sizeof(client);
    memset(&client,0,sizeof(client));
    new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
   //accept 与connect 完成3次握手
   
   if(-1==new_fd)
    {
        perror("accept");
        return -1;
    }
    printf("client ip=%s,port=%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    
    //epoll的准备工作 
    char buf[128]={0};
    int epfd=epoll_create(1);
    struct epoll_event event,evs[2];//结构数组监控几个描述符就搞多大
    event.events=EPOLLIN;//事件，表示对应的文件描述符可读
    event.data.fd=STDIN_FILENO;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&event);
    if(-1==ret)
    {
        perror("epoll_ctl");
        return -1;
    }
    event.data.fd=new_fd;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);
    if(-1==ret)
    {
        printf("line num=%d\n",__LINE__);//打印行号
        perror("epoll_ctl");
        return 0;
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
                send(new_fd,buf,strlen(buf)-1,0);

            }
            if(new_fd==evs[i].data.fd)
            {
                memset(buf,0,sizeof(buf));
                ret=recv(new_fd,buf,sizeof(buf)-1,0);
                if(0==ret)
                {
                    printf("client close,byebye\n");
                    goto end;
                }
                printf("%s\n",buf);
            }
        }
    }
end:
    close(new_fd);
    close(socketfd);
    return 0;
}

