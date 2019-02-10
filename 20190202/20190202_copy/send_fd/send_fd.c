#include <func.h>
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
    msg.msg_iov=iov;
    msg.msg_iovlen=2;
    struct cmsghdr *cmsg;
    int len=CMSG_LEN(sizeof(int));
    cmsg=(struct cmsghdr *)calloc(1,len);
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    *(int*)CMSG_DATA(cmsg)=fd;
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
void recv_fd(int socketpiper,int *fd)
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
    msg.msg_iov=iov;
    msg.msg_iovlen=2;
    struct cmsghdr *cmsg;
    int len=CMSG_LEN(sizeof(int));
    cmsg=(struct cmsghdr *)calloc(1,len);
    cmsg->cmsg_len=len;
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    msg.msg_control=cmsg;
    msg.msg_controllen=len;
    int ret;
    ret=recvmsg(socketpiper,&msg,0);
    if(-1==ret)
    {
        perror("sendmsg");
        return;
    }
    *fd=*(int*)CMSG_DATA(cmsg);
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
    }else{
        close(fds[0]);//关闭读端
        int fd=open("file",O_RDWR);
        printf("parent fd=%d\n",fd);
        send_fd(fds[1],fd); 
        wait(NULL);
        return 0;
    }
}

