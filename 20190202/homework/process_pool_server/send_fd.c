#include <func.h>
//进程间传递文件描述符
void send_fd(int socketpipew,int fd)
{
    struct msghdr msg;//先定义这个结构体
    memset(&msg,0,sizeof(msg));//一定要初始化清空
    char buf1[10]="xiong";
    char buf2[10]="da";
    struct iovec iov[2];//没啥用，必须写，退出机制可能有用
    iov[0].iov_base=buf1;
    iov[0].iov_len=5;
    iov[1].iov_base=buf2;
    iov[1].iov_len=2;
    msg.msg_iov=iov;//iovec 一定要写
    msg.msg_iovlen=2;//一定要写
    struct cmsghdr *cmsg;//要定义一个结构体指针，变长结构体
    int len=CMSG_LEN(sizeof(int));//申请空间的大小
    cmsg=(struct cmsghdr*)calloc(1,len);//申请空间
    cmsg->cmsg_len=len;//第一个，存着本结构体有多长，否者内核不知道
    cmsg->cmsg_level=SOL_SOCKET;//与man帮助保持一致
    cmsg->cmsg_type=SCM_RIGHTS;//与man cmsg帮助保持一致
    *(int *)CMSG_DATA(cmsg)=fd;//偏移，拿到变长结构体的最后成员的起始地址
    msg.msg_control=cmsg;//关键，真正传递内核控制信息的
    msg.msg_controllen=len;//关键，真正传递内核控制信息的
    //
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
    struct cmsghdr *cmsg;//要定义一个结构体指针，变长结构体
    int len=CMSG_LEN(sizeof(int));//申请空间的大小
    cmsg=(struct cmsghdr*)calloc(1,len);//申请空间
    cmsg->cmsg_len=len;//第一个，存着本结构体有多长
    cmsg->cmsg_level=SOL_SOCKET;//与man帮助保持一致
    cmsg->cmsg_type=SCM_RIGHTS;//与man帮助保持一致
    msg.msg_control=cmsg;//关键
    msg.msg_controllen=len;//关键
    int ret;
    ret=recvmsg(socketpiper,&msg,0);
    if(-1==ret)
    {
        perror("sendmsg");
        return;
    }
   *fd= *(int *)CMSG_DATA(cmsg);
    
 }

/*
  int main()//测试用代码
{
    int fds[2];
    socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
    if(!fork())
    {
    //    close(fds[1]);//关闭写端
        int fd;
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
        send_fd(fds[1],fd);
        wait(NULL);
        return 0;
    }
    
}
*/
