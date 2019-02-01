#include <func.h>

int main(int argc,char *argv[])
{
    args_check(argc,3);
    int socketfd=socket(AF_INET,SOCK_DGRAM,0);
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
    sendto(socketfd,"I am client",11,0,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    socklen_t addrlen=sizeof(struct sockaddr);
    char buf[128]={0};
    recvfrom(socketfd,buf,sizeof(buf),0,(struct sockaddr*)&ser,&addrlen);
    printf("%s\n",buf);
    close(socketfd);
    return 0;
}
