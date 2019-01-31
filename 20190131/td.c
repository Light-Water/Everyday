#include <func.h>

int main()
{
    time_t t=time(NULL);
    char *ptime=ctime(&t);
    printf("%s 今日份 の Hello world\n",ptime);
    return 0;
}

