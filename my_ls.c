/*************************************************************************
	> File Name: my_ls.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年07月20日 星期三 16时04分51秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>
#include<linux/limits.h>
#include<time.h>

#define flag_none 0  //没有参数
#define flag_a  1   //对应参数为为a
#define flag_l 2   //对应参数为l
#define flag_R 4    //对应参数为R
#define max 200  //每行最多输出200个字符

int len_max=max;
int len_leave;



void my_err(const char*err_string,int line)    //错误输出函数
{
    fprintf(stderr,"line:%d",line);
    perror(err_string);
    exit(1);
}

int  panduan(char* filename)  //判断是否为..或.目录,防止R参数无限递归.或..目录 
{
    
    int x=1;
    int i=0,j=0;
    char name[NAME_MAX+1];
    for(i=0;filename[i]!='\0';i++)
    {
        if(filename[i]=='/')
        j=0;
        else
        name[j++]=filename[i];
    }
    name[j]='\0';
    i=0;
    if(name[0]=='.')
    {
        if((name[1]=='\0')||(name[1]=='.'))
        x=0;
    }
    return x;

}
void printf_file1(struct stat buf,char *name) //l形式输出文件信息
{

    struct passwd* user;
    struct group* grp;
    char time[32];
    if(S_ISLNK(buf.st_mode))     //判断文件属性
    printf("l");
    if(S_ISREG(buf.st_mode))
    printf("-");
    if(S_ISDIR(buf.st_mode))
    printf("d");
    if(S_ISCHR(buf.st_mode))
    printf("c");
    if(S_ISBLK(buf.st_mode))
    printf("b");
    if(S_ISFIFO(buf.st_mode))
    printf("f");
    if(S_ISSOCK(buf.st_mode))
    printf("s");                

    if(buf.st_mode&S_IRUSR)   //判断文件所有者权限
    printf("r");
    else
    printf("-");
    if(buf.st_mode&S_IWUSR)
    printf("w");
    else
    printf("-");
    if(buf.st_mode&S_IXUSR)
    printf("x");
    else
    printf("-");

    if(buf.st_mode&S_IRGRP)   //判断文件所属组权限
    printf("r");
    else
    printf("-");
    if(buf.st_mode&S_IWGRP)
    printf("w");
    else
    printf("-");
    if(buf.st_mode&S_IXGRP)
    printf("x");
    else
    printf("-");


    if(buf.st_mode&S_IROTH)   //判断文件其他者权限
    printf("r");
    else
    printf("-");
    if(buf.st_mode&S_IWOTH)
    printf("w");
    else
    printf("-");
    if(buf.st_mode&S_IXOTH)
    printf("x");
    else
    printf("-");
    printf(" ");

    printf("%ld",buf.st_nlink);//文件链接数
    printf(" ");

    user=getpwuid(buf.st_uid);
    grp=getgrgid(buf.st_gid);
    printf("%-8s ",user->pw_name);
    printf("%-8s  ",grp->gr_name);
    printf("%-6ld ",buf.st_size);
    strcpy(time,ctime(&buf.st_mtime));
    time[strlen(time)-1]='\0';
    printf("%-10s ",time);
    printf("%s\n",name);
    
   
}

void printf_file2(char *name)  //只对齐输出文件名
{

    int i;
    int len;
    if(len_leave<len_max)  //如果该行不够输出文件名，则换行
    {
      printf("\n");
      len_leave=max;

    }
    printf("%-s",name);
    len=strlen(name);
    for(i=0;i<len_max-len;i++)
    printf(" ");
    printf("  ");
    len_leave-=(len_max+2);


}

void printf_file(int flag,char* path) //读取参数和路径输出文件信息
{

    struct stat buf;
    int i=0,j=0;
    char name[NAME_MAX+1];
    for(i=0;path[i]!='\0';i++)
    {
        if(path[i]=='/')
        j=0;
        else
        name[j++]=path[i];
    }
    name[j]='\0';
    if(lstat(path,&buf)==-1) 
    my_err("lstat",__LINE__);
    switch(flag)
    {
        case flag_none:
        if(name[0]!='.')
        printf_file2(name);
        break;
        case flag_R:
        if(name[0]!='.')
        printf_file2(name);
        break;
        case flag_a+flag_R:
        printf_file2(name);
        break;
        case flag_l+flag_R:
        if(name[0]!='.')
        printf_file1(buf,name);
        break;
        case flag_l+flag_a+flag_R:
        printf_file1(buf,name);
        break;
        case flag_a:
        printf_file2(name);
        break;
        case flag_l:
        if(name[0]!='.')
        printf_file1(buf,name);
        break;
        case flag_l+flag_a:
        printf_file1(buf,name);
        break;

    }
 
}

void printf_dir2(int flag,char* path) //输出目录内的文件信息
{
     
    char filename[256][PATH_MAX+1];
    DIR* dir;
    struct dirent *ptr;
    int i=0,j=0;
    int s=0;//计数
    char temp[PATH_MAX];
    struct stat buf;
    int len; //记录路径名长度
    if((dir=opendir(path))==NULL)
    my_err("opendir",__LINE__);
    while((ptr=readdir(dir))!=NULL) //将目录里的文件名存到数组中
    {
        if((flag&flag_a)==0)
        if(ptr->d_name[0]=='.')
        continue;
        strcpy(filename[i],path);
       strcat(filename[i],ptr->d_name);
        i++;
        s++;
     
    }
    len_max=strlen(filename[1]);
    for(i=0;i<s-1;i++)      //冒泡排序将文件名进行字典排序
    for(j=0;j<s-1-i;j++)
    {
       
        if(strlen(filename[j])>strlen(filename[j+1]))
        {
           strcpy(temp,filename[j]);
           strcpy(filename[j],filename[j+1]);
           strcpy(filename[j+1],temp);
            len_max=strlen(filename[j]);
        }
    }
    closedir(dir);
    printf("\n%s:\n",path);
    for(i=0;i<s;i++)
    printf_file(flag,filename[i]);
    printf("\n");
    for(i=0;i<s;i++)
    {
    
       if(lstat(filename[i],&buf)==-1)
        my_err("lstat",__LINE__);
       len=strlen(filename[i]);
        if(S_ISDIR(buf.st_mode)&&panduan(filename[i]))
        {
          if(filename[i][len-1]!='/')
        {
            filename[i][len]='/';
            filename[i][len+1]='\0';
        }
        
      
            printf_dir2(flag,filename[i]);

        }

    }
}

void printf_dir1(int flag,char* path) //输出目录内的文件信息
{
     
    char filename[256][PATH_MAX+1];
    DIR* dir;
    struct dirent *ptr;
    int i=0,j=0;
    int s=0;//计数
    char temp[PATH_MAX];

    if((dir=opendir(path))==NULL)
    my_err("opendir",__LINE__);
    while((ptr=readdir(dir))!=NULL) //将目录里的文件名存到数组中
    {
        if((flag&flag_a)==0)
        if(ptr->d_name[0]=='.')
        continue;
        strcpy(filename[i],path);
       strcat(filename[i],ptr->d_name);
        i++;
        s++;
       if(s>256)
        my_err("too many files under this dir",__LINE__);
    }
    len_max=strlen(filename[1]);
    for(i=0;i<s-1;i++)      //冒泡排序将文件名进行字典排序
    for(j=0;j<s-1-i;j++)
    {
       
        if(strlen(filename[j])>strlen(filename[j+1]))
        {
           strcpy(temp,filename[j]);
           strcpy(filename[j],filename[j+1]);
           strcpy(filename[j+1],temp);
            len_max=strlen(filename[j]);
        }
    }
    closedir(dir);
 
    for(i=0;i<s;i++)
    {
       printf_file(flag,filename[i]);
    }
      if((flag&flag_l)==0)
      printf("\n");
      closedir(dir);
    

}

int main(int argc,char** argv)
{
    struct stat buf;
    char param[32];
    int flag=0;
    int i,j,s=0,k=0;
    char path[PATH_MAX+1];
    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')
        {
            j=1;
            while(argv[i][j]!='\0')
            {
              param[k++]=argv[i][j];
              j++;

            }
            s++;
        }
    }
    for(i=0;i<k;i++)
    {
        switch(param[i])
        {
            case 'a':
            flag=flag|flag_a;break;
            case 'l':
            flag=flag|flag_l;break;
            case 'R':
            flag=flag|flag_R;break;
            default:
            printf("error\n");
            exit(1);
            break;

        }
    }
      if(s+1==argc)
        {

        strcpy(path,"./");
            if((flag&flag_R)==0)
        printf_dir1(flag,path);
            else
            printf_dir2(flag,path);
        }
    for(i=s+1;i<argc;i++)
    {
      
         strcpy(path,argv[i]);
        if(stat(path,&buf)==-1)
        my_err("stat",__LINE__);
        if(S_ISDIR(buf.st_mode))
        {
            if((flag&flag_R)==0)
            {
                
            if(path[(strlen(path)-1)]!='/')
            {
            path[strlen(path)]='/';
            path[strlen(path)+1]='\0';
            }
           printf_dir1(flag,path);

            }
            else
            {
               if(path[(strlen(path)-1)]!='/')
             {
              path[strlen(path)]='/';
              path[strlen(path)+1]='\0';
             }
              printf_dir2(flag,path);
            }        
        }
        else
        printf_file(flag,path);
        
    }
    return 0;
}

