/*************************************************************************
	> File Name: 1.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年05月29日 星期日 21时32分03秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>


int main()
{
    int **a;  //用二级指针动态申请二维数组
    int i,j;
    int m,n;
    printf("请输入行数\n");
    scanf("%d",&m);
    printf("请输入列数\n");
    scanf("%d",&n);
    a=(int**)malloc(sizeof(int*)*m);
    for(i=0;i<m;i++)
    a[i]=(int*)malloc(sizeof(int)*n);
    for(i=0;i<m;i++)
    for(j=0;j<n;j++)
    printf("%p\n",&a[i][j]);     //输出每个元素地址，每行的列与列之间的地址时连续的，行与行之间的地址不连续
    for(i=0;i<m;i++)
    free(a[i]);
    free(a);
    return 0;
}
