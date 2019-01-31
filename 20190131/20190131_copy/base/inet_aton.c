#include <func.h>

int main(int argc,char* argv[])
{
    args_check(argc,2);
    int ret;
    struct in_addr addr;
    ret=inet_aton(argv[1],&addr);
    printf("%x\n",addr.s_addr);
    return 0;
}

