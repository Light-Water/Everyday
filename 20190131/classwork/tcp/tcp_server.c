#include <func.h>

int main(int argc,char** argv)
{
    args_check(argc,3);//传ip,端口
    //，如网络中某一台计算机的IP为10.92.20.160，
    //操作系统分配给计算机中某一应用程序进程的端口号为1500，
    //则此时 10.92.20.160 1500就构成了一个套接口
    //socket函数 生成一个套接口描述符
    int socketfd=socket(AF_INET,SOCK_STREAM,0);//ipv4,tcp
    if(-1==socketfd)
    //成功返回套接口描述符 
    {
        perror("socket");
        return -1;
    }
    printf("socketfd=%d\n",socketfd);
    struct sockaddr_in ser;//定义结构体变量，现在的版本
    memset(&ser,0,sizeof(ser));//结构体？为什么要取地址？ 
    //bind函数：用来绑定一个端口号和IP地址，
    //使套接口与指定的端口号和IP地址相关联
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    //htons 大小端转换
    //atoi将字符串转化为数字
    ser.sin_addr.s_addr=inet_addr(argv[1]);
    //为什么ip地址不用大小端转换呢？因为我们
    //直接将点分十进制,一个字符串的ip转化为了32位的整形数，网络字节序，
    //inet_addr成功的返回值就是32的网络字节序
    int ret;
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));//强转，兼顾旧版本
    //bind函数：用来绑定一个端口号和IP地址，
    //使套接口与指定的端口号和IP地址相关联。
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    listen(socketfd,10);//使端口处于监听状态
    int new_fd;//通过new_fd与客户端发送接收数据
    //之前的套接字socketfd,用于绑定监听等工作，
    //服务器跟客服机连接之后，调用accept产生
    //一个新的套接字new_fd,用与以后跟客户间
    //数据的传输和读取
    struct sockaddr_in client;//
    socklen_t addrlen=sizeof(client);
    memset(&client,0,sizeof(client));
    new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
   //client, 和bind中的ser类似，
   //系统把远程客户端的ip和端口号保存到这个结构体
    if(-1==new_fd)
    {
        perror("accept");
        return -1;
    }
    char buf[128]={0};
    ret=recv(new_fd,buf,sizeof(buf),0);
    //用新的套接字来接收远端主机传来的数据,存到buf
    //根read类似，成功返回接收到的字符数
    if(-1==ret)
    {
        perror("recv");
        return -1;
    }
    printf("I am server,I recv %s\n",buf);
    send(new_fd,"World",5,0);//发送数据给远方客户端，返回值为发送的字符数
    close(new_fd);
    close(socketfd);
    return 0;
}

