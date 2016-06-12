/*************************************************************************
	> File Name: 1.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年06月12日 星期日 19时06分52秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>

void change(char*a,char*b)
{
    char t[101];
    strcpy(t,a);
    strcpy(a,b);
    strcpy(b,t);
}
int main()
{ 
    char a[101],b[101];
    int i,j=0;
    int s1,s2;
    printf("请输入第一个整数\n");
    scanf("%s",a);
    printf("请输入第二个整数\n");
    scanf("%s",b);
    s1=strlen(a);
    s2=strlen(b);
    if(s1<s2)
    {
        change(a,b);
        s1^=s2;
        s2^=s1;
        s1^=s2;
    }
    for(i=s1;i>=0;i--)
    {
        a[i+1]=a[i];
        
    }
    a[0]='0';
    for(i=0;i<=s2;i++)
    {
        b[s1+1-i]=b[s2-i];

    }
    for(i=0;i<s1-s2+1;i++)
    b[i]='0';
    for(i=s1;i>=0;i--)
    {
        char t;
        t=a[i];
        a[i]=(a[i]+b[i]-96+j)%10+48;
        j=(t+b[i]-96)/10;
    
        
    }
    if(a[0]=='0')
    printf("%s\n",a+1);
    else
    printf("%s\n",a);

    return 0;


}
