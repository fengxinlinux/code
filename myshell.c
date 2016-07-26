/*************************************************************************
	> File Name: myshell.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年07月26日 星期二 09时08分30秒
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
#include<dirent.h>
#include<pwd.h>



#define NONE           "\e[0m"     //不加色，恢复终端颜色
#define L_CYAN         "\e[1;36m"  //深淡蓝色
#define L_PURPLE       "\e[1;35m"  //深紫色

#define normal         0           //一般的命令
#define out_redirect   1           //输出重定向
#define in_redirect    2          //输入重定向
#define have_pipe      3           //命令中有管道


void pd_home(char buf[],struct passwd *psd)  //判断当前路径是否含家目录，将家目录改为～
{
    int len1; //记录当前目录路径长度
    int len2; //记录用户主目录路径长度
    int i;
    int n=1; //记录是否路径含主目录
    char mainpath[PATH_MAX+1]; //记录主目录路径
    char temp[PATH_MAX+1];
    strcpy(mainpath,psd->pw_dir);
    len2=strlen(mainpath);
    len1=strlen(buf);
    for(i=0;i<len2;i++)
    {
        if(mainpath[i]!=buf[i])
        n=0;
    }
    if(n)
    {
        strcpy(temp,buf);
        buf[0]='~';
        buf[1]='\0';
        strcat(buf,(temp+len2));
    }
    
}

void print_shell() //打印shell提示符
{
    char cpname[32]; //记录主机名
    struct passwd *psd;  //记录用户信息
    char buf[PATH_MAX+1]; //记录当前目录路径
    psd=getpwuid(getuid());
    printf(L_CYAN"%s"NONE,psd->pw_name);  //输出用户名
    printf(L_CYAN"@"NONE);
    if(gethostname(cpname, sizeof(cpname))==-1)
    {
        perror("get computer name error\n");
        exit(1); 
    }
    printf(L_CYAN"%s"NONE,cpname);  //输出主机名
    printf(":");
    if(getcwd(buf,sizeof(buf))==NULL)
    {
        perror("get cwd error\n");
        exit(1);
    }
    pd_home(buf,psd);      //判断主目录
    printf(L_PURPLE"%s"NONE,buf);  //输出当前目录
    if(getuid()==0)
    printf("#");
    else
    printf("$ ");


}


void input_command(char* buf)    //命令输入
{
    int len=0;
    int ch;
    ch=getchar();
    while(len<256&&ch!='\n')
    {
        buf[len++]=ch;
        ch=getchar();
    }
    if(len==256)
    {
       printf("commod is too long \n");
       exit(1);
    }
    buf[len]='\0';


}

void explain_command(char* buf,int* sum,char arglist[100][256])   //解析命令，分开存储
{
    int i=0;
    int s=0;
    while(buf[i]!='\0')
    {
        if(buf[i]!=' ')
        s++;
        else
        {
            strncpy(arglist[*sum],buf+i-s,s);
            s=0;
            *sum=*sum+1;
        }
    }

}

int find_command(char* command)       //查找命令是否存在
{
    char*path[]={"./","/bin","/usr/bin"};
    DIR* dp;
    struct dirent * dir;
    int i=0;
    for(i=0;i<3;i++)
    {
        if(dp=opendir(path[i])==-1)
        {
            printf("opendir error\n");
            return 0;
        }
        while(dir=readdir(dp)!=NULL)
        {
            if(strcmp(command,dir->d_name)==0)
            return 1;
        }
        closedir(dp);
    }
    return 0;
}

void do_command(int sum,char arglist[100][256])    //执行命令
{
    int flag =0;      //若flag>1,说明命令中含多个>,<,|或格式不正确
    int how=0;    //记录命令中是否含有>,<,|
    int background;  //记录命令中是否有&
    pid_t pid;
    int fd;
    char* file;      //记录文件名
    char** arg;      //记录命令
    char** argnext;   // 记录管道命令
    arg=(char**)malloc(sizeof(char*)*(sum+1));
    argnext=(char**)malloc(sizeof(char*)*(sun+1));
    
    for(i=0;i<sum;i++)  //将命令取出
    arg[i]=arglist[i];
    arg[i]=NULL;

    for(i=0;i<sum;i++)  //判断是否含有&
    {
        if(strcmp(arg,"&")==0)
        {
            if(arg[i+1]!=NULL)
            {
                printf("wrong command\n");
                return;
            }
       
        }
        else
        {
            background=1;
            arg[i]=NULL;
            break;
        }
    }

    for(i=0;i<sum;i++)  //判断是否有>,<,|
    {
        if(strcmp(arg[i],">")==0)
        {
            flag++;
            how=out_redirect;
            if(i==0)
            flag++;
            if(i==sum-1)
            flag++;
        }
        if(strcmp(arg[i],"<")==0)
        {
            flag++;
            how=in_redirect;
            if(i==0)
            flag++;
            if(i==sum-1)
            flag++;
        }
        if(strcmp(arg[i],"|")==0)
        {
            flag++;
            how=have_pipe;
            if(i==0)
            flag++;
            if(i==sum-1)
            flag++;
        }
        
    }


    if(flag>1)
    {
        printf("wrong command\n");
        return;
    }

    if(how==out_redirect)
    {
        for(i=0;i<sum;i++)
        if(strcmp(arg[i],">")==0)
        break;
        file=arg[i+1];
        arg[i]=NULL;

    }

    if(how==in_redirect)
    {
        for(i=0;i<sum;i++)
        if(strcmp(arg[i],"<")==0)
        break;
        file=arg[i+1];
        arg[i]=NULL;
    }

    if(how==have_pipe)
    {
        for(i=0;i<sum;i++)
        if(strcmp(arg[i],"|")==0)
        break;
        int j=i+1;
        for(j;j<sum;j++)
        argnext[j-i-1]=arg[j];
        argnext[j-i-1]=arg[j];
    }




}





int main()
{
    print_shell();
}
