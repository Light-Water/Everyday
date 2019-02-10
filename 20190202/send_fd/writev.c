#include <func.h>

int main()
{
    int fd=open("file1",O_RDWR);
    char buf1[10]="xiong";
    char buf2[10]="da";
    struct iovec iov[2];
    iov[0].iov_base=buf1;
    iov[0].iov_len=5;
    iov[1].iov_base=buf2;
    iov[1].iov_len=2;
    writev(fd,iov,2);
    //writev将多块分散的内存一并写入文件描述符中，即集中写
    close(fd);
    return 0;
}

