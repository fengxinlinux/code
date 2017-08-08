/*************************************************************************
	> File Name: client.cpp
	> Author:fengxin 
	> Mail:903087053@qq.com 
	> Created Time: 2017年07月22日 星期六 10时38分30秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<math.h>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"protocol.h"

#define PORT 6666   //服务器端口

using namespace std;

class TCPClient
{
    public:

    TCPClient(int argc ,char** argv);
    ~TCPClient();


    //向服务器发送数据
    bool send_to_serv(unsigned char *data_buffer,int datasize,uint16_t wOpcode);
    //从服务器接收数据
    bool recv_from_serv(unsigned char *data_buffer);

    bool send_file();   //向服务器发送文件
    void run(); //主运行函数


    private:

    int conn_fd; //创建连接套接字
    struct sockaddr_in serv_addr; //储存服务器地址

};




TCPClient::TCPClient(int argc,char **argv)  //构造函数
{
    if(argc!=3)    //检测输入参数个数是否正确
    {
        cout<<"Usage: [-a] [serv_address]"<<endl;
        exit(1);
    }


    //初始化服务器地址结构
    memset(&serv_addr,0,sizeof(struct sockaddr_in));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);

    //从命令行服务器地址
    for(int i=0;i<argc;i++)
    {
        if(strcmp("-a",argv[i])==0)
        {

            if(inet_aton(argv[i+1],&serv_addr.sin_addr)==0)
            {
                cout<<"invaild server ip address"<<endl;
                exit(1);
            }
            break;
        }
    }

    //检查是否少输入了某项参数
    if(serv_addr.sin_addr.s_addr==0)
    {
        cout<<"Usage: [-a] [serv_address]"<<endl;
        exit(1);
    }

    //创建一个TCP套接字
    conn_fd=socket(AF_INET,SOCK_STREAM,0);


    if(conn_fd<0)
    {
        my_err("connect",__LINE__);
    }

    //向服务器发送连接请求
    if(connect(conn_fd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr))<0)
    {
        my_err("connect",__LINE__);
    }

}

TCPClient::~TCPClient()  //析构函数
{
    close(conn_fd);

}

bool TCPClient::send_to_serv(unsigned char *data_buffer,int datasize,uint16_t wOpcode) //向服务器发送数据
{
    NetPacket send_packet;   //数据包
    send_packet.Header.wDataSize=datasize+sizeof(NetPacketHeader);  //数据包大小
    send_packet.Header.wOpcode=wOpcode;

    memcpy(send_packet.Data,data_buffer,datasize);  //数据拷贝


    if(send(conn_fd,&send_packet,send_packet.Header.wDataSize,0))
       return true;
    else
       return false;

}


bool TCPClient::recv_from_serv(unsigned char* data_buffer)   //从服务器接收数据
{
    int nrecvsize=0; //一次接收到的数据大小
    int sum_recvsize=0; //总共收到的数据大小
    int packersize;   //数据包总大小
    int datasize;     //数据总大小


    memset(data_buffer,0,sizeof(data_buffer));   ///初始化buffer

      while(sum_recvsize!=sizeof(NetPacketHeader))
    {
        nrecvsize=recv(conn_fd,data_buffer+sum_recvsize,sizeof(NetPacketHeader)-sum_recvsize,0);
        if(nrecvsize==0)
        {
            //服务器退出;
            return false;
        }
        if(nrecvsize<0)
        {
            cout<<"从客户端接收数据失败"<<endl;
            return false;
        }
        sum_recvsize+=nrecvsize;

    }



    NetPacketHeader *phead=(NetPacketHeader*)data_buffer;
    packersize=phead->wDataSize;  //数据包大小
    datasize=packersize-sizeof(NetPacketHeader);     //数据总大小



    while(sum_recvsize!=packersize)
    {
        nrecvsize=recv(conn_fd,data_buffer+sum_recvsize,packersize-sum_recvsize,0);
        if(nrecvsize==0)
        {
            cout<<"从客户端接收数据失败"<<endl;
            return false;
        }
        sum_recvsize+=nrecvsize;
    }

    return true;

}

bool TCPClient::send_file()   //向服务器发送文件
{
    unsigned char send_buffer[1024]; //发送数据buffer
    unsigned char data_buffer[5000];   //接收数据buffer
    string filename;  //文件路径名
    int fd; //文件描述符
    struct stat file_buffer;  //文件属性buffer
    File_message file_message;   //文件信息
    File_data file_data;    //存储文件的内容和大小
    int nsize=0;     //一次读取的数据大小
    int sum_size=0;     //总共读取的数据大小



    memset(send_buffer,0,sizeof(send_buffer));

    cout<<"请输入要发送的文件路径及文件名"<<endl;
    getline(cin,filename);

    if(fd=open(filename.c_str(),O_RDONLY)<0)
    {
        my_err("open file error",__LINE__);
    }
    if(stat(filename.c_str(),&file_buffer)<0)
    {
        my_err("stat file error",__LINE__);
    }

    strcpy(file_message.filename,filename.c_str());
    file_message.filesize=file_buffer.st_size;



    if(send_to_serv((unsigned char*)&file_message,sizeof(file_data),NET_TEST1)<0)
    {
        cout<<"向服务器发送文件信息失败"<<endl;
    }


    close(fd);

    if(recv_from_serv(data_buffer)<0)
    {
        cout<<"从服务器接收数据失败"<<endl;
    }
    file_message=*(struct File_message*)(data_buffer+sizeof(NetPacketHeader));
    if((fd=open(filename.c_str(),O_RDONLY))<0)
       {
           my_err("打开文件失败",__LINE__);
       }


    if(file_message.fseek)
    {
        cout<<"继续上次传送文件，已传输"<<file_message.fseek<<"bytes,还需传输"<<file_message.filesize-file_message.fseek<<"bytes"<<endl;
        lseek(fd,file_message.fseek,0);
    }

    while(nsize=read(fd,send_buffer,sizeof(send_buffer)))
    {


        memset(&file_data,0,sizeof(file_data));
        strcpy(file_data.filename,filename.c_str());

        memcpy(file_data.buffer,send_buffer,nsize);

        send_to_serv((unsigned char*)&file_data,nsize+sizeof(file_data.filename),NET_TEST2);
        sum_size+=nsize;
    }



    if(sum_size+file_message.fseek==file_buffer.st_size)
    {
        cout<<"发送文件成功"<<endl;
        close(fd);
        return true;
    }
    else
    {
        cout<<"发送文件出错"<<endl;
        close(fd);
        return false;
    }

}

void TCPClient::run()
{
    send_file();  //向服务器发送文件

}

int main(int argc ,char **argv)
{

    TCPClient client(argc,argv);
    client.run();
    sleep(10);

}

