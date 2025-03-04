//客户端
#include "thp_tsf_function.h"

void print_progress(double percentage) {
    int bar_width = 50;
    printf("[");
    int pos = bar_width * percentage;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %.2f%%\r", percentage * 100);
    fflush(stdout);
}

void handl_upload(int socketfd) {
         //上传文件
        printf("即将上传...\n");
        FILE *fp;
        char buffer[4096] = {0};
        fp = fopen("../fileshouse_client/500mb_file", "rb");
        if (fp == NULL) {
            perror("File open failed");
            
        }

        // 获取文件大小
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        int bytes_read;
        long total_sent = 0;
        while ((bytes_read = fread(buffer, 1, 4096, fp)) > 0) {
            send(socketfd, buffer, bytes_read, 0);
            total_sent += bytes_read;
            print_progress((double)total_sent / file_size);
        }
        printf("\nFile sent successfully.\n");
        close(socketfd);
        printf("上传成功.\n");

}

void handl_download(int socketfd) {
        //下载文件
        printf("即将下载...\n");

        FILE *fp;
        char buffer[4096] = {0};
        // 接收文件大小
        long file_size;
        recv(socketfd, &file_size, sizeof(file_size), 0);
        // 打开文件以写入
        fp = fopen("../fileshouse_client/1000mb_file", "wb");
        if (fp == NULL) {
            perror("File open failed");
        }
        // 接收文件内容
        long total_received = 0;
        int bytes_received;
        while ((bytes_received = recv(socketfd, buffer,4096, 0)) > 0) {
            fwrite(buffer, 1, bytes_received, fp);
            total_received += bytes_received;

            // 更新进度条
            print_progress((double)total_received / file_size);

            if (total_received >= file_size) break;
        }
        printf("\nFile received and saved as '1000mb_file'\n");
        close(socketfd);
        printf("下载任务完成，断开数据TCP连接。\n");
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
    
    //创建新socket，并connect到服务器
    int socketfd = tcp_connect("127.0.0.1", 12222);
    printf("已和服务端子线程函数建立了TCP连接\n");
    //----------------------已和服务端子线程函数建立了TCP连接------------------------------

    if(t->cmdType == CMD_TYPE_UPLOAD){
        handl_upload(socketfd);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        handl_download(socketfd);

    }

    return 0;
}

