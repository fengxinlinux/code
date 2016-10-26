/*************************************************************************
	> File Name: my_who.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年10月26日 星期三 15时59分29秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<utmp.h>
#include<time.h>

void showtime(long time);
void show_info(struct utmp utbuf);

int main()
{
    struct utmp utbuf;
    int utmpfd;

    if((utmpfd=open(UTMP_FILE,O_RDONLY))==-1)
    {
        perror(UTMP_FILE);
        exit(1);
    }
    while(read(utmpfd,&utbuf,sizeof(struct utmp))==sizeof(struct utmp))
    {
        show_info(utbuf);
    }
    close(utmpfd);
    return 0;
}

void show_info(struct utmp utbuf)
{
    if(utbuf.ut_type!=USER_PROCESS)  //若结构体里的信息不是已登陆的用户信息，则不输出
    return;

    printf("%-8.8s",utbuf.ut_name);    //用户名
    printf(" ");
    printf("%-8.8s",utbuf.ut_line);   //终端
    printf(" ");
    showtime(utbuf.ut_time);   //输出时间

    #ifdef SHOWHOST 
    if(utbuf.ut_host[0]!='\0')
    printf("(%s)",utbuf.ut_host);
    #endif
    printf("\n");
}

void showtime(long time)
{
    char *cp;
    struct tm *p_tm;
    p_tm=gmtime(&time);
    cp=ctime(&time);
    printf("    %d-%d-%d",(p_tm->tm_year+1900), (p_tm->tm_mon+1), p_tm->tm_mday);  
   // printf("  %d:%d",p_tm->tm_hour, p_tm->tm_min);
     printf("%6.6s",cp+10);
}
