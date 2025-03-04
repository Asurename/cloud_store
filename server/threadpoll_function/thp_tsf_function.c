//服务端
#include "../lib.h"
#include <sys/sendfile.h>

void handl_upload(int socketfd){
    //接受上传文件
    printf("正在接受来自客户端xxx的文件...\n");//可加客户ip

    FILE *fp;
    char buffer[4096] = {0};
    fp = fopen("../fileshouse_server/500mb_file", "wb");
    if (fp == NULL) {
        perror("File open failed");
    }
    int bytes_received;
    while ((bytes_received = recv(socketfd, buffer, 4096, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, fp);
    }
    printf("File received and saved as 'received_file.txt'\n");
    close(socketfd);
    printf("客户端xxx上传文件完成，断开数据TCP连接");
}

void handl_download(int socketfd){
         //发送下载文件
        printf("下载处理中...\n");

        // 打开要发送的文件
        const char *file_path = "../fileshouse_server/1000mb_file";
        int file_fd = open(file_path, O_RDONLY);
        if (file_fd < 0) {
            perror("File open failed");
        }

        // 获取文件大小
        struct stat file_stat;
        if (fstat(file_fd, &file_stat) < 0) {
            perror("fstat failed");
            exit(EXIT_FAILURE);
        }
        off_t file_size = file_stat.st_size;
        // 发送文件大小给客户端
        send(socketfd, &file_size, sizeof(file_size), 0);

        // 接收客户端请求的起始偏移量
        off_t offset;
        recv(socketfd, &offset, sizeof(offset), 0);
        printf("Client requested to resume from offset: %ld\n", offset);

        // 使用 sendfile 发送文件内容
        ssize_t sent_bytes = sendfile(socketfd, file_fd, &offset, file_size - offset);
        if (sent_bytes < 0) {
            perror("Sendfile failed");
        } else {
            printf("File sent successfully (%ld bytes).\n", sent_bytes);
        }

        // 关闭文件和连接
        close(file_fd);
        close(socketfd);



        printf("客户端xx下载任务完成,断开数据TCP连接\n");

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
        handl_upload(socketfd);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        handl_download(socketfd);
    }

    return NULL;
}


