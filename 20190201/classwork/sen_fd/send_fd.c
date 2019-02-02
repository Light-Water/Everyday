#include <func.h>

void send_fd(int socketpipew,int fd)
{

}
void recv_fd(int socketpiper,int *fd)
{

}
int main()
{
    int fds[2];
    socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
    if(!fork())
    {
        //close(fds[1]);//关闭写端
        int fd;
        recv_fd(fds[0],&fd);
        printf("child fds[0]=%d, fd=%d\n",fds[0],fd);
        char buf[128]={0};
        read(fd,buf,sizeof(buf));
        printf("buf=%s\n",buf);//打印读取内容
        exit(0);

    }
    else{
        close(fds[0]);//关闭读端
        int fd=open("file",O_RDWR);
        printf("parent fd=%d\n",fd);
        send_fd(fds[1],fd);
        wait(NULL);
        return 0;
    }
}

