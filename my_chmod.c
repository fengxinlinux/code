/************************************************************************
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

void my_err(char* sh,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(sh);
    exit(1);
}
void chmod1(int argc,char **argv) //数字形式改权限
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
    my_err("chmod",__LINE__);


}
void chmod3(char*sh,char a,char b,char c,int mode)
{
    switch(a)
    {
        case 'u':
        if(b=='+')
        {
            if(c=='w')
            {
                mode=mode|S_IWUSR;
                if(chmod(sh,mode)==-1)
                my_err("chmod",__LINE__);
            }
            else if(c=='r')
            {
                mode=mode|S_IREAD;
                if(chmod(sh,mode)==-1)
                  my_err("chmod",__LINE__);
            }
            else if(c=='x')
            {
                mode=mode|S_IEXEC;
                if(chmod(sh,mode)==-1)
               my_err("chmod",__LINE__);
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
      else  if(b=='-')
        {
            if(c=='w')
            {
                mode=mode&~S_IWUSR;
                if(chmod(sh,mode)==-1)
                  my_err("chmod",__LINE__);
            }
            else if(c=='r')
            {
                mode=mode&~S_IREAD;
                if(chmod(sh,mode)==-1)
                  my_err("chmod",__LINE__);
            }
            else if(c=='x')
            {
                mode=mode&~S_IEXEC;
                if(chmod(sh,mode)==-1)
                my_err("chmod",__LINE__);
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
       else  if(b=='=')
        {
            if(c=='w')
            {
                mode=mode&S_IWUSR;
                if(chmod(sh,mode)==-1)
                  my_err("chmod",__LINE__);
            }
            else if(c=='r')
            {
                mode=mode&S_IREAD;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode&S_IEXEC;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
        else{
            printf("格式错误\n");
            exit(0);
        }
        break;
       case 'g':
        if(b=='+')
        {
            if(c=='w')
            {
                mode=mode|S_IWGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode|S_IRGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode|S_IXGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
      else  if(b=='-')
        {
            if(c=='w')
            {
                mode=mode&~S_IWGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode&~S_IRGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode&~S_IXGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
       else  if(b=='=')
        {
            if(c=='w')
            {
                mode=mode&S_IWGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode&S_IRGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode&S_IXGRP;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
        else{
            printf("格式错误\n");
            exit(0);
        }
        break;
      case 'o':
        if(b=='+')
        {
            if(c=='w')
            {
                mode=mode|S_IWOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode|S_IROTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode|S_IXOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
      else  if(b=='-')
        {
            if(c=='w')
            {
                mode=mode&~S_IWOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode&~S_IROTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode&~S_IXOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
       else  if(b=='=')
        {
            if(c=='w')
            {
                mode=mode&S_IWOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='r')
            {
                mode=mode&S_IROTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else if(c=='x')
            {
                mode=mode&S_IXOTH;
                if(chmod(sh,mode)==-1)
                {
                      my_err("chmod",__LINE__);
                }
            }
            else {
                printf("格式错误\n");
                exit(0);}

        }
        else{
            printf("格式错误\n");
            exit(0);
        }
        break;
    default:
            printf("格式错误\n");
            exit(0);
            break;

    }
}
void chmod2(int argc,char **argv)  //字母形式改权限
{
    char a;//判断u,g,r
    char b;//判断+，-，=
    char c[3]={0};
    char *sh;
    int i=0;
    int j=0;
    struct stat buf;
    int mode;
    char *ch;
    ch=argv[2];
    sh=argv[1];
    if(strlen(sh)<3||strlen(sh)>5)
    {
        printf("格式错误\n");
        exit(0);
    }
    a=sh[i++];
    b=sh[i++];
    while(sh[i]!='\0')
    {
        c[j++]=sh[i];
        i++;
        
    }
    j=0;
    while(c[j]!=0)
    {
      stat(ch,&buf);
      mode=buf.st_mode;
      chmod3(ch,a,b,c[j],mode);
     j++;   

    }
}
int main(int argc,char **argv)
{
    char* sh;
    int i=0;
    sh=argv[1];
    if(sh[0]>='0'&&sh[0]<'9')
    chmod1(argc,argv);
    else
    chmod2(argc,argv);
    return 0;
}
