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
    int ret;
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    struct sockaddr_in client;
    memset(&client,0,sizeof(client));
    socklen_t addrlen=sizeof(struct sockaddr);
    char buf[7]={0};
    recvfrom(socketfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&client,&addrlen);
    printf("%s ip=%s,port=%d\n",buf,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    recvfrom(socketfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&client,&addrlen);
    printf("%s ip=%s,port=%d\n",buf,inet_ntoa(client.sin_addr),ntohs(client.sin_port));
    sendto(socketfd,"I am server",11,0,(struct sockaddr*)&client,sizeof(struct sockaddr));
    close(socketfd);
    return 0;
}
