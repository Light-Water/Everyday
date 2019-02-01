#include <func.h>

int main(int argc,char** argv)
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
    char buf[128]={0};
    sendto(socketfd,"1",1,0,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    fd_set rdset;
    socklen_t addrlen;
    while(1)
    {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        FD_SET(socketfd,&rdset);
        ret=select(socketfd+1,&rdset,NULL,NULL,NULL);
        if(ret>0)
        {
            if(FD_ISSET(STDIN_FILENO,&rdset))
            {
                memset(buf,0,sizeof(buf));
                ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
                if(0==ret)
                {
                    printf("I will close\n");
                    break;
                }
                sendto(socketfd,buf,strlen(buf)-1,0,(struct sockaddr*)&ser,sizeof(struct sockaddr));
                                
            }
            if(FD_ISSET(socketfd,&rdset))
            {
                memset(buf,0,sizeof(buf));
                addrlen=sizeof(struct sockaddr);
                ret=recvfrom(socketfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&ser,&addrlen);
               if(-1==ret)
               {
                   perror("recvfrom");
                   return -1;
               }
               printf("%s\n",buf);

            }
        }
    }
    close(socketfd);
    return 0;
}

