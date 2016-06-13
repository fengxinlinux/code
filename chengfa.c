/*************************************************************************
	> File Name: 2.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年06月13日 星期一 19时22分00秒
 ************************************************************************/


#include<stdio.h>
#include<stdlib.h>
#include"string.h"

#define len 20

int main()
{
    int i,j,t=0,n,temp;
    char mult1[len];
    char mult2[len];
    printf("Input two big numbers:\n");
    scanf("%s %s",mult1,mult2);
    int len1 = strlen(mult1);
    int len2 = strlen(mult2);
    int *c1 = (int *)malloc(sizeof(int)*len1);
    int *c2 = (int *)malloc(sizeof(int)*len2);

    for(i=0; i<len1; i++)    //转化为整形数组
    c1[i]=(int)mult1[i]-48;
    for(i=0; i<len2; i++)
    c2[i]=(int)mult2[i]-48;

     
    int *result = (int *)malloc(sizeof(int)*(len1+len2));

    n=len1+len2-1;
    for(i=0;i<=n;i++)
    result[i]=0; 
    temp=0;
    for(i=len2-1; i>=0; i--)
    {
        t++;//记录下次移位个数
        for(j=len1-1;j>=0;j--)
        {
            if((temp=result[n]+c2[i]*c1[j])>=10)
            {
                result[n]=temp%10;
                result[n-1]+=temp/10;

            }
            else
            result[n]=temp;
            n--;

        }
        n=len1+len2-1-t;//移位

    }

    printf("Result: "); 
    if(result[0]!=0)
    printf("%d",result[0]); 
    for(i=1;i<=len1+len2-1;i++)
    printf("%d",result[i]); 
    printf("\n");
    return 0;

}
