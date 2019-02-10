#include <func.h>
void changeNonblock(int fd)
{
    int status=fcntl(fd,F_GETFL);
    status=status|O_NONBLOCK;//改为非阻塞
    fcntl(fd,F_SETFL,status);
}
int main()
{
    char buf[128]={0};
    sleep(10);
    changeNonblock(STDIN_FILENO);
    int ret=read(STDIN_FILENO,buf,sizeof(buf));
    printf("%d %s %d\n",ret,buf,errno);
    return 0;
}

