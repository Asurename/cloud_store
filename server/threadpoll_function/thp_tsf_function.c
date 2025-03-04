//服务端
#include "../lib.h"

void handl_upload(cmd_tast* t, int socketfd){
/*
    //3 接受并解析结构体
        //提取出文件名
        //提取出当前路径
        //提取出虚拟绝对文件路径
        //提取这个文件哈希值
        //提取文件大小

    //4 遍历hash值表
    if(哈希值不存在){//即文件内容不存在，需要接收文件内容，存储路径是pwd/fileshouse/哈希值
        //5 发送哈希值不存在的标志。
            //7 接受文件内容
        //8 在数据库 表插入一行
            //插入函数
        //执行到这里，就说明文件已经接收完毕，并且已经存储到服务器的fileshouse文件夹中和数据库中，结束线程
    }
    
    if(哈希值存在){//即文件内容已存在，不需要接收文件内容，只需要在数据库 表插入一行。秒传！
        //5 发送哈希值存在的标志。
        if(虚拟绝对路径存在){
            //什么都不做，秒传，结束线程
        }else{
            //表插入一行
        }
    }

*/  


}

void handl_download(cmd_tast* t, int socketfd){
    
}



//文件传输线程要执行的函数
void* thp_tsf_function(void * arg){
    queue_and_mysql_t* qam = (queue_and_mysql_t*)arg;

    //从队列中取出任务
    cmd_tast* t = (cmd_tast*)tast_queue_pop(qam->q);//线程一般会在这阻塞和唤醒

    //从已完成监听套接字中连接队列中取出一个连接请求
    int listenfd = t->peerfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int socketfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    printf("已和客户端子线程函数建立了TCP连接\n");
    //----------------------已和客户端子线程函数建立了TCP连接------------------------------
    if(t->cmdType == CMD_TYPE_UPLOAD){
        //上传文件
        handl_upload(t, socketfd);
        printf("上传/下载任务处理中...\n");
        sleep(1);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        //下载文件
        handl_download(t, socketfd);
        printf("上传/下载任务处理中...\n");
        sleep(1);
    }

    close(socketfd);
    printf("任务完成,断开TCP数据连接\n");
    return NULL;
}


