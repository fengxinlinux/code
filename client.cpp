/*************************************************************************
	> File Name: client.cpp
	> Author:fengxin 
	> Mail:903087053@qq.com 
	> Created Time: 2017年05月30日 星期二 20时47分08秒
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
#include<time.h>

#define PORT 6666   //服务器端口
#define NET_PACKET_DATA_SIZE 5000  
using namespace std;



struct File_message    //文件信息类
{
    char filename[100]; //文件名
    long filesize;  //文件大小

};

struct File_data   //文件内容类
{
    char filename[100];  //文件名
    unsigned char buffer[1024];  //文件内容
};

void my_err(const char *err_string,int line)  //自定义错误函数
{
    cerr<<"line:"<<line<<endl; //输出错误发生在第几行
    perror(err_string);       //输出错误信息提示
    exit(1);
}

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
    NET_TEST1 = 1, //发送文件信息
    NET_TEST2 = 2, //发送文件内容
};  


bool send_to_serv(int conn_fd,unsigned char *data_buffer,int datasize,uint16_t wOpcode) //向服务器发送数据
{
    NetPacket send_packet;
    send_packet.Header.wDataSize=datasize+sizeof(NetPacketHeader);  //数据包大小
    send_packet.Header.wOpcode=wOpcode;
    memcpy(send_packet.Data,data_buffer,datasize);

    


    if(send(conn_fd,&send_packet,send_packet.Header.wDataSize,0))
       return true;
    else
       return false;

}


bool send_file(int conn_fd)   //向服务器发送文件
{
    unsigned char send_buffer[1024]; //发送数据buffer
    string filename;  //文件路径名
    int fd; //文件描述符
    struct stat file_buffer;  //文件属性buffer
    File_message file_message;   //文件信息
    File_data file_data;
    int nsize=0;
    int sum_size=0;
    


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



    if(send_to_serv(conn_fd,(unsigned char*)&file_message,sizeof(file_data),NET_TEST1)<0)
    {
        cout<<"向服务器发送文件信息失败"<<endl;
    }


    close(fd);
    if((fd=open(filename.c_str(),O_RDONLY))<0)
       {
           my_err("打开文件失败",__LINE__);
       }

    while(nsize=read(fd,send_buffer,sizeof(send_buffer)))
    {
        

        memset(&file_data,0,sizeof(file_data));
        strcpy(file_data.filename,filename.c_str());

        memcpy(file_data.buffer,send_buffer,nsize);

        send_to_serv(conn_fd,(unsigned char*)&file_data,nsize+sizeof(file_data.filename),NET_TEST2);

        sum_size+=nsize;
    }



    if(sum_size==file_buffer.st_size)
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

int main(int argc ,char **argv)
{
    int conn_fd; //创建连接套接字
    struct sockaddr_in serv_addr; //储存服务器地址

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
    

    send_file(conn_fd);  //向服务器发送文件

    sleep(10);
    

}
