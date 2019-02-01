#include <func.h>

int main()
{
    time_t t=time(NULL);
    struct tm *ptime;
    ptime=gmtime(&t);
    printf("%04d-%02d-%02d %02d:%02d:%02d",\
           1900+ptime->tm_year,1+ptime->tm_mon,ptime->tm_mday,\
           8+ptime->tm_hour,ptime->tm_min,ptime->tm_sec);
    printf(" 今日份 の Hello world\n");
    return 0;
}

