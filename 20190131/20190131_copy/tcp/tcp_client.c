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
    ret=connect(socketfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    if(-1==ret)
    {
        perror("connect");
        return -1;
    }
    send(socketfd,"hello",5,0);
    char buf[128]={0};
    recv(socketfd,buf,sizeof(buf),0);
    printf("I am client %s\n",buf);
    close(socketfd);
    return 0;
}
