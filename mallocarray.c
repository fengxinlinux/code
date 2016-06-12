/*************************************************************************
	> File Name: 2.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年05月29日 星期日 21时53分56秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>


int main()
{
      int i,j;
  //  申请一个3行2列的整型数组
    int (*a)[2]=(int(*)[2])malloc(sizeof(int)*3*2);
    for(i=0;i<3;i++)
    for(j=0;j<2;j++)
    {
       
       printf("%p\n",&a[i][j]);

    } //输出数组每个元素地址，每个元素的地址是连续的
    free(a);
    return 0;
}
