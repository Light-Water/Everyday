#include <func.h>
//进程间传递文件描述符
void send_fd(int socketpipew,int fd)
{
    struct msghdr msg;
    memset(&msg,0,sizeof(msg));
    
    char buf1[10]="xiong";
    char buf2[10]="da";
    struct iovec iov[2];
    iov[0].iov_base=buf1;
    iov[0].iov_len=5;
    iov[1].iov_base=buf2;
    iov[1].iov_len=2;
    struct cmsghdr *cmsg;
    int len=CMSG_LEN(sizeof(int));
    cmsg=(struct cmsghdr*)calloc(1,len);//申请空间强转
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    *(int *)CMSG_DATA(cmsg)=fd;
    msg.msg_control=cmsg;
    msg.msg_controllen=len;
    int ret;
    ret=sendmsg(socketpipew,&msg,0);
    if(-1==ret)
    {
        perror("sendmsg");
        return;
    }

}

void recv_fd(int socketpiper,int *fd)//接收要取地址
{
    struct msghdr msg;
    memset(&msg,0,sizeof(msg));
    
    char buf1[10]={0};
    char buf2[10]={0};
    struct iovec iov[2];
    iov[0].iov_base=buf1;
    iov[0].iov_len=5;
    iov[1].iov_base=buf2;
    iov[1].iov_len=2;
    struct cmsghdr *cmsg;
    int len=CMSG_LEN(sizeof(int));
    cmsg=(struct cmsghdr*)calloc(1,len);//申请空间强转
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    msg.msg_control=cmsg;
    msg.msg_controllen=len;
    int ret;
    ret=recvmsg(socketpiper,&msg,0);
    if(-1==ret)
    {
        perror("recvmsg");
        return;
    }
    *fd=*(int*)CMSG_DATA(cmsg);//传过来，

}
int main()
{//
    
    int fds[2];
    socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
    //不用pipe无名管道，用特殊的socketpair管道
    if(!fork())
    {
        close(fds[1]);//关闭写端
        int fd;
        //read(fds[0],&fd,sizeof(fd));//read返回值是读到的字节数
        recv_fd(fds[0],&fd);
        printf("child fds[0]=%d,fd=%d\n",fds[0],fd);
        char buf[128]={0};
        read(fd,buf,sizeof(buf));
        printf("buf=%s\n",buf);
        exit(0);//子进程结束
    }else{
        close(fds[0]);//关闭读端
        int fd=open("file",O_RDWR);
        printf("parent fd=%d\n",fd);
       // write(fds[1],&fd,sizeof(fd));
       send_fd(fds[1],fd);//把write改为send_fd发送描述符信息，
      // 因为是发送内核信息，所以发不用取地址
        wait(NULL);
        return 0;
    }
    
}

