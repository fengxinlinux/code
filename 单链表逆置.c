/*************************************************************************mZ	> File Name: 单链表逆置.c
	> Author: FengXin
	> Mail: fengxinlinux@gmail.com
	> Created Time: 2016年05月24日 星期二 19时35分21秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>

/* 将头结点摘下，然后从第一结点开始，依次前插入到头结点的后面（头插法建立单链表)，直到最后一个结点为止，则实现了链表的逆置 */
typedef struct node
{
    int i;
    struct node* next;
}node;

node* reverse(node* head)
{
   //L是带头结点的单链表，本算法将L就地逆置
    node *r,*p;
    p=head->next;
    head->next=NULL;
    while(p!=NULL)
    {
        r=p->next;
        p->next=head->next;
        head->next=p;
        p=r;
    }
    return head;

}
