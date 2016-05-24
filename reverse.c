/*************************************************************************
	> File Name: reverse.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年05月24日 星期二 20时08分45秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>

//就地逆置带头结点的链表
/*typedef struct node{
    int i;
    node* next;
}node;   */

node* reverse(node* head)
{
    node *r,*pre,*p;
    p=head->next;
    r=p->next;
    p->next=NULL;
    while(r!=NULL)
    {
        pre=p;
        p=r;
        r=r->next;
        p->next=pre;

    }
    head->next=p;
    return head;
}


