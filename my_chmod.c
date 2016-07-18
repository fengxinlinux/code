/*************************************************************************
	> File Name: my_chmod.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年07月18日 星期一 14时03分34秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
int main(int argc,char **argv)
{
    int mode;
    int mode_u;
    int mode_g;
    int mode_o;
    char *sh;
    if(argc<3)
    {
        printf("输入参数格式错误\n");
        exit(0);
    }
    mode=atoi(*(argv+1));
    if(mode>777||mode<0)
    {
        printf("格式错误\n");
        exit(0);
    }
    mode_u=mode/100;
    mode_g=mode%100/10;
    mode_o=mode%10;
    mode=mode_u*8*8+mode_g*8+mode_o;
    sh=argv[2];
    if(chmod(sh,mode)==-1)
    {
        printf("error\n");
        exit(0);
    }
    return 0;

}
