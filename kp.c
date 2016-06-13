/*************************************************************************
	> File Name: 1.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年06月13日 星期一 20时05分58秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>
int kp(int a[],int low,int high)
{
    int i,j,temp;
    i=low;
    j=high;
    temp=a[i];
    if(i<j)
    {
        while(i<j)
        {
            while(a[j]>=temp&&i<j)
            j--;
            a[i]=a[j];
            while(a[i]<=temp&&i<j)
            i++;
            a[j]=a[i];

        }
        a[i]=temp;
        kp(a,low,i-1);
        kp(a,i+1,high);
    }
    else
    return 0;
}
int main()
{
    int a[100];
    int n,i;
    printf("请输入n的个数\n");
    scanf("%d",&n);
    for(i=0;i<n;i++)
    {
        printf("请输入一个整数\n");
        scanf("%d",&a[i]);
    }
    kp(a,0,n-1);
    printf("排序后为\n");
    for(i=0;i<n;i++)
    printf("%3d ",a[i]);
    printf("\n");
    return 0;
}
