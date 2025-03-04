//客户端
#include "thp_tsf_function.h"

void handl_upload(cmd_tast* t, int socketfd){
/* 
    //1 组装结构体
        //要解析出文件名
        //要解析出当前路径
        //组合出虚拟绝对文件路径 = 当前路径+文件名
        //计算这个文件哈希值
        //获取文件大小
        //以上数据存在一个结构体中

    //2 发送文件哈希值（发送结构体）

    //4 接收服务器返回的哈希值标志
    if(哈希值不存在){//即文件内容不存在，需要发送文件内容
        //6 发送文件内容

        //发送完毕就结束线程
    }

    if(哈希值存在){//即文件内容已存在
        //6 秒传了，什么都不做，结束线程
    }
*/
    
}



/*
    介绍结构体t:
    t->cmdType: 命令类型
    t->content: 命令类型+文件名
    t->path: 虚拟绝对当前工作目录路径
    例子：
    t->cmdType = CMD_TYPE_UPLOAD;
    t->content = "upload test.txt";
    t->path = "/james";
    逻辑上，表示上传/james/test.txt文件到服务器，服务器会将文件保存到/james/test.txt
    实际上，服务器会将文件保存到服务器的当前工作目录下的一个fileshouse文件夹里，且文件名为哈希值
*/

//文件传输线程要执行的函数
void* thp_tsf_function(void* arg){
    tast_queue* q = (tast_queue*)arg;

    //从队列中取出任务
    void* t1 = tast_queue_pop(q);//线程一般会在这阻塞和唤醒
    cmd_tast* t = (cmd_tast*)t1;
    sleep(1);
    //创建新socket，并connect到服务器
    int socketfd = tcp_connect("127.0.0.1", 12222);
    printf("已和服务端子线程函数建立了TCP连接\n");
    //----------------------已和服务端子线程函数建立了TCP连接------------------------------

    if(t->cmdType == CMD_TYPE_UPLOAD){
        //上传文件
        handl_upload(t, socketfd);
        printf("上传/下载任务处理中...\n");
        sleep(1);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        //下载文件
        printf("上传/下载任务处理中...\n");
        sleep(1);
    }

    close(socketfd);
    printf("任务完成,断开TCP数据连接\n");
    return 0;
}

