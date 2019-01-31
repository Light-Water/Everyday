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
    memset(&client,0,sizeof(client));
    new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
    if(-1==new_fd)
    {
        perror("accept");
        return -1;
    }
    char buf[128]={0};
    ret=recv(new_fd,buf,sizeof(buf),0);
    if(-1==ret)
    {
        perror("recv");
        return -1;
    }
    printf("I am server,I recv %s\n",buf);
    send(new_fd,"world",5,0);
    close(new_fd);
    close(socketfd);
    return 0;
}

