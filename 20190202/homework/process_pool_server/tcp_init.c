#include <func.h>

int tcpInit(int *sfd,char *ip,char *port)//一会儿把socketfd赋值给sfd
{//网络编程，进程间通信封装起来
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==socketfd)
    {
        perror("socket");
        return -1;
    }
    printf("socketfd=%d",socketfd);
    int ret;
    int reuse=1;
     //允许重用本地地址和端口
     ret=setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
     if(-1==ret)
     {
        perror("setsockopt0");
        return -1;
      }

    
    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(port));
    ser.sin_addr.s_addr=inet_addr(ip);
    
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    listen(socketfd,10);
    *sfd=socketfd;
    return 0;
}

