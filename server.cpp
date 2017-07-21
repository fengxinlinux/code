/*************************************************************************
    > File Name: server.cpp
    > Author: fengxin
    > Mail: 903087053@qq.com 
    > Created Time: 2017年05月29日 星期一 21时24分50秒
 ************************************************************************/

#include<iostream>
#include<cstring>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>

using namespace std;

#define NET_PACKET_DATA_SIZE 5000 
#define PORT 6666   //服务器端口
#define LISTEN_SIZE 1023   //连接请求队列的最大长度
#define EPOLL_SIZE  1023   //epoll监听客户端的最大数目
  

/// 网络数据包包头  
struct NetPacketHeader  
{  
    unsigned short      wDataSize;  ///< 数据包大小，包含封包头和封包数据大小  
    unsigned short      wOpcode;    ///< 操作码  
};  
  
/// 网络数据包  
struct NetPacket  
{  
    NetPacketHeader     Header;                         ///< 包头  
    unsigned char       Data[NET_PACKET_DATA_SIZE];     ///< 数据  
};  
  
  
  
/// 网络操作码  
enum eNetOpcode  
{  
    NET_TEST1  = 1,  
};  

struct File_message
{
    char filename[100];  //文件名
    long filesize;  //文件大小
};

struct File_data
{
    char filename[100];  //文件名
    unsigned char buffer[1024]; //文件内容
};

void my_err(const char *err_string,int line)  //自定义错误函数
{
    cerr<<"line:"<<line<<endl; //输出错误发生在第几行
    perror(err_string);       //输出错误信息提示
    exit(1);
}

bool dealwithpacket(int conn_fd,unsigned char *recv_data,uint16_t wOpcode,int datasize)  //处理接收到的数据
{
    

    int fd;
    if(wOpcode==1)  //接收文件信息
    { 
        File_message *file_message=(File_message*)recv_data;
        strcat(file_message->filename,".down");
        

        if((fd=open(file_message->filename,O_RDWR|O_APPEND|O_CREAT,0777))<0)
        {
            cout<<"创建文件失败"<<endl;
            return false;
        }
    }
    else if(wOpcode==2)  //接收文件内容
    {
        
        File_data * file_data=(File_data*)recv_data;

        strcat(file_data->filename,".down");
        if((fd=open(file_data->filename,O_RDWR|O_APPEND))<0)
        {
            cout<<"打开文件失败"<<endl;
            return false;
        }
        if(write(fd,file_data->buffer,datasize-sizeof(file_data->filename))<0)
        {
            cout<<"写入文件失败"<<endl;
            return false;
        }
        close(fd);
    }
   return true;
    
}

bool server_recv(int conn_fd)  //接收数据函数
{
    int nrecvsize=0; //一次接收到的数据大小
    int sum_recvsize=0; //总共收到的数据大小
    int packersize;   //数据包总大小
    int datasize;     //数据总大小
    unsigned char recv_buffer[10000];  //接收数据的buffer


    memset(recv_buffer,0,sizeof(recv_buffer));  //初始化接收buffer
    

    while(sum_recvsize!=sizeof(NetPacketHeader))
    {
        nrecvsize=recv(conn_fd,recv_buffer+sum_recvsize,sizeof(NetPacketHeader)-sum_recvsize,0);
        if(nrecvsize==0)
        {
            cout<<"从客户端接收数据失败"<<endl;
            return false;
        }
        sum_recvsize+=nrecvsize;

    }
    


    NetPacketHeader *phead=(NetPacketHeader*)recv_buffer;
    packersize=phead->wDataSize;  //数据包大小
    datasize=packersize-sizeof(NetPacketHeader);     //数据总大小

    


    while(sum_recvsize!=packersize)
    {
        nrecvsize=recv(conn_fd,recv_buffer+sum_recvsize,packersize-sum_recvsize,0);
        if(nrecvsize==0)
        {
            cout<<"从客户端接收数据失败"<<endl;
            return false;
        }
        sum_recvsize+=nrecvsize;
    }

    
    dealwithpacket(conn_fd,(unsigned char*)(phead+1),phead->wOpcode,datasize);



}
int main()
{

    int sock_fd;  //监听套接字
    int conn_fd;    //连接套接字
    int epollfd;  //epoll监听描述符
    socklen_t cli_len;  //记录连接套接字地址的大小
    struct epoll_event  event;   //epoll监听事件
    struct epoll_event*  events;  //epoll监听事件集合
    struct sockaddr_in cli_addr;  //客户端地址
    struct sockaddr_in serv_addr;   //服务器地址



	//创建一个套接字
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd<0)
    {
        my_err("socket",__LINE__);
    }
    //设置该套接字使之可以重新绑定端口
    int optval=1;
    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&optval,sizeof(int))<0)
    {
        my_err("setsock",__LINE__);
    }
    //初始化服务器端地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(sock_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr_in))<0)
    {
        my_err("bind",__LINE__);
    }
    //将套接字转化为监听套接字
    if(listen(sock_fd,LISTEN_SIZE)<0)
    {
        my_err("listen",__LINE__);
    }


    cli_len=sizeof(struct sockaddr_in);
    events=(struct epoll_event*)malloc(sizeof(struct epoll_event)*EPOLL_SIZE); //分配内存空间
    
    //创建一个监听描述符epoll,并将监听套接字加入监听列表
    epollfd=epoll_create(EPOLL_SIZE);
    if(epollfd==-1)
    {
        my_err("epollfd",__LINE__);
    }
    event.events = EPOLLIN;
    event.data.fd = sock_fd;
    if(epoll_ctl(epollfd,EPOLL_CTL_ADD,sock_fd,&event)<0)
    {
        my_err("epoll_ctl",__LINE__);
    }

     
    while(1)   //循环监听事件
    {
        int sum=0,i;
        sum=epoll_wait(epollfd,events,EPOLL_SIZE,-1);
        for(i=0;i<sum;i++)
        {
            if(events[i].data.fd==sock_fd)    //客户端请求连接
            {
                conn_fd=accept(sock_fd,(struct sockaddr*)&cli_addr,&cli_len);
                if(conn_fd<0)
                {
                    my_err("accept",__LINE__);
                }
                event.events = EPOLLIN | EPOLLRDHUP; //监听连接套接字的可读和退出
                event.data.fd = conn_fd;
                if(epoll_ctl(epollfd,EPOLL_CTL_ADD,conn_fd,&event)<0) //将新连接的套接字加入监听
                {
                    my_err("epoll",__LINE__);
                }
                cout<<"a connet is connected,ip is "<<inet_ntoa(cli_addr.sin_addr)<<endl;

            }
            else if(events[i].events&EPOLLIN)    //客户端发来数据
            {
                
                server_recv(events[i].data.fd);  //接收数据包并做处理
                
            }
            if(events[i].events&EPOLLRDHUP) //客户端退出
            {
                cout<<"a connet is quit,ip is "<<inet_ntoa(cli_addr.sin_addr)<<endl;
                epoll_ctl(epollfd,EPOLL_CTL_DEL,events[i].data.fd,&event);
                close(events[i].data.fd);
            }
            
        }
    }

}
