/*************************************************************************
> File Name: myshell.c
> Author: FengXin
> Mail: fengxinlinux@gmail.com
> Created Time: 2016年07月26日 星期二 09时08分30秒
************************************************************************/
#define _GNU_SOURCE

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
#include<signal.h>



#define NONE           "\e[0m"     //不加色，恢复终端颜色
#define L_CYAN         "\e[1;36m"  //深淡蓝色
#define L_PURPLE       "\e[1;35m"  //深紫色

#define normal         0           //一般的命令
#define out_redirect   1           //输出重定向
#define in_redirect    2          //输入重定向
#define have_pipe      3           //命令中有管道
#define have_cd        4          //cd命令



typedef struct history
{
    int n;
    char command[200];
    struct history *next;

}history;
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
    while(buf[i]==' '&&buf[i]!='\0')
    i++;
    while(buf[i]!='\0')
    {
        if(buf[i]!=' ')
        s++;
        else
        {
            strncpy(arglist[*sum],buf+i-s,s);
            arglist[*sum][s]='\0';
            s=0;
            *sum=*sum+1;

        }
        i++;
    }

    if(s!=0)
    {
        strncpy(arglist[*sum],buf+i-s,s);
        arglist[*sum][s]='\0';
        *sum=*sum+1;
    }

}
void cd_command(char**arg)      //执行cd命令
{
    int i;
    char path[PATH_MAX];      //记录更改的路径
    struct passwd *psd;       //记录用户信息
    psd=getpwuid(getuid());
    if(arg[1]==NULL||(strcmp(arg[1],"~")==0))
    {
        strcpy(path,psd->pw_dir);
        if(chdir(path)==-1)
        {
            printf("路径错误\n");
            return;
        }

    }
    else
    {
        strcpy(path,arg[1]);
        if(chdir(path)==-1)
        {
            printf("路径错误\n");
            return;
        }
    }

}

history*  history_command()     //执行history命令
{
    char *argv[3];
    FILE *fp;
    history* new=(history*)malloc(sizeof(history));
    history* head=(history*)malloc(sizeof(history));
    history* temp;
    temp=head;
    int n=0;
    struct passwd*psd;
    char path[PATH_MAX+1];
    psd=getpwuid(getuid());
    new->next=NULL;

    strcpy(path,psd->pw_dir);
    strcat(path,"/.bash_history");
    fp=fopen(path,"rt");
    while(!feof(fp))
    {
       
        fgets(new->command,1000,fp);
        temp->next=new;
        n++;
        new->n=n;
        new=(history*)malloc(sizeof(history));
        new->next=NULL;
        temp=temp->next;
        
    }
    printf("2\n");

    return head;

}

int find_command(char* command)       //查找命令是否存在
{
    char*path[]={"./","/bin","/usr/bin"};
    DIR* dp;
    struct dirent * dir;
    int i=0;
    if(strncmp(command,"./",2)==0)
    command=command+2;
    for(i=0;i<3;i++)
    {
        if((dp=opendir(path[i]))==NULL)
        {
            printf("opendir error\n");
            return 0;
        }
        while((dir=readdir(dp))!=NULL)
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
    int background=0;  //记录命令中是否有&
    pid_t pid;
    int fd;
    int i;
    char* file;      //记录文件名
    char** arg;      //记录命令
    char** argnext;   // 记录管道命令
    history* head=(history*)malloc(sizeof(history));
    history* temp;
    arg=(char**)malloc(sizeof(char*)*(sum+1));
    argnext=(char**)malloc(sizeof(char*)*(sum+1));
    for(i=0;i<sum;i++)  //将命令取出
    arg[i]=arglist[i];
    arg[i]=NULL;


    if(strcmp(arg[0],"cd")==0)   //判断是否cd命令
    {
        cd_command(arg);
        return;
    }

    if(strcmp(arg[0],"history")==0)  //判断是否为history命令
    {
        
        head=history_command();
        temp=head->next;
        while(temp!=NULL)
        {
            printf("%s",temp->command);
            temp=temp->next;
        }
        return;
    }

    if(strcmp(arg[0],"ls")==0)  //给ls命令默认设置为--color=auto
    {
        i=0;
        while(arg[i]!=NULL)
        i++;
        if(strcmp(arg[i-1],"&")==0)
        {
            arg[i-1]="--color=auto";
            arg[i]="&";
            arg[i+1]=NULL;
        }
        else
        {
            arg[i]="--color=auto";
            arg[++i]=NULL;

        }
    }

    for(i=0;arg[i]!=NULL;i++)  //判断是否含有&
    {

        if(strcmp(arg[i],"&")==0)
        {
            if(arg[i+1]!=NULL&&strcmp(arg[0],"ls")!=0)
            {
                printf("wrong command\n");
                return;
            }


            else
            {
                background=1;
                arg[i]=NULL;
                break;
            }
        }
    }

    for(i=0;arg[i]!=NULL;i++)  //判断是否有>,<,|
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
            if(arg[i+1]==NULL)
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
        for(i=0;arg[i]!=NULL;i++)
        if(strcmp(arg[i],">")==0)
        break;
        file=arg[i+1];
        arg[i]=NULL;

    }

    if(how==in_redirect)
    {
        for(i=0;arg[i]!=NULL;i++)
        if(strcmp(arg[i],"<")==0)
        break;
        file=arg[i+1];
        arg[i]=NULL;
    }

    if(how==have_pipe)
    {
        for(i=0;arg[i]!=NULL;i++)
        if(strcmp(arg[i],"|")==0)
        break;
        arg[i]=NULL;
        int j=i+1;
        for(j;arg[j]!=NULL;j++)
        argnext[j-i-1]=arg[j];
        argnext[j-i-1]=arg[j];
    }


    if((pid=fork())<0)
    {
        printf("creat process error \n");
        return;

    }

        switch(how)
        {
            case normal:
            if(pid==0)
            {
                if(!find_command(arg[0]))
                {
                    printf("未找到'%s'命令\n",arg[0]);
                    exit(1);
                }
                execvp(arg[0],arg);
                exit(0);
            } 
            break;
            case out_redirect:
            if(pid==0)
            {
                if(!find_command(arg[0]))
                {
                    printf("未找到'%s'命令\n",arg[0]);
                    exit(1);
                }
                if((fd=open(file,O_RDWR|O_CREAT|O_TRUNC,0644))==-1)
                {
                    printf("creat file error\n");
                    exit(1);
                }
                if (dup2(fd,1) < 0) {
                    perror("dup2");
                }
                if(execvp(arg[0],arg)==-1)
                {
                    printf("do command error\n");
                    exit(1);

                }
                exit(0);
            }
            break;
            case in_redirect:
            if(pid==0)
            {
                if(!find_command(arg[0]))
                {
                    printf("未找到'%s'命令\n",arg[0]);
                    exit(1);
                }
                if((fd=open(file,O_RDWR))==-1)
                {
                    printf("read file error\n");
                    exit(1);
                }
                dup2(fd,0);
                execvp(arg[0],arg);
                exit(0);
            } 
        break;
    case have_pipe:
        if(pid==0)
        {
            int pid2;
            int fd2;
            if((pid2=fork())<0)
            {
                printf("fork2 error\n");
                exit(1);
            }
            if(pid2==0)
            {
                if(!find_command(arg[0]))
                {
                    printf("未找到'%s'命令\n",arg[0]);
                    return;
                }

                if((fd2=open("/tmp/temp.txt",O_RDWR|O_CREAT|O_TRUNC,0644))==-1)
                {
                    printf("open dir error\n");
                    exit(0);
                }
                dup2(fd2,1);
                execvp(arg[0],arg);
                exit(0);
            }

            if(waitpid(pid2,NULL,0)==-1)
            printf("wait for child process error\n");
            if(!find_command(argnext[0]))
            {
                printf("未找到命令'%s'\n",argnext[0]);
                return;
            }
            if((fd=open("/tmp/temp.txt",O_RDONLY))==-1)
            printf("open file error\n");
            dup2(fd,0);
            execvp(argnext[0],argnext);
            exit(0);
        }
        default:
            break;
        }
        if(background)
        {
            printf("[process id %d]\n",pid);
            return;
        }
    else
    if(waitpid(pid,NULL,0)==-1)
    {
        printf("wait for child process error\n");
        return;
    }


}





int main()
{
    int i;
    int sum=0;
    char arglist[100][256];
    char* buf=NULL;
    history* head;

    buf=(char*)malloc(sizeof(char)*256);
    while(1)
    {
        memset(buf,0,256);
        print_shell();
        input_command(buf);
        if(strcmp(buf,"exit")==0||strcmp(buf,"logout")==0)
        break;
        for(i=0;i<100;i++)
        arglist[i][0]='\0';
        sum=0;
        explain_command(buf,&sum,arglist);
        do_command(sum,arglist);


    }

    if(buf!=NULL)
    free(buf);
    exit(0);

}
