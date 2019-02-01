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
    int new_fd;//通过new_fd与客户端发送与接收数据
    struct sockaddr_in client;
    socklen_t addrlen=sizeof(client);
    char buf[128]={0};
    fd_set rdset,recordset;//recordset 始终记录要监控的描述符的情况
    FD_SET(socketfd,&recordset);
    FD_SET(STDIN_FILENO,&recordset);

    while(1)
    {
        FD_ZERO(&rdset);
        memcpy(&rdset,&recordset,sizeof(fd_set));
        ret=select(13,&rdset,NULL,NULL,NULL);
        if(ret>0)
        {
            if(FD_ISSET(socketfd,&rdset))
            {
                memset(&client,0,sizeof(client));
                new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
                if(-1==new_fd)
                {
                    perror("accept");
                    return -1;
                }
                printf("new_fd=%d client ip=%s,port=%d\n",new_fd,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
                FD_SET(new_fd,&recordset);
            }
            if(FD_ISSET(STDIN_FILENO,&rdset))
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

