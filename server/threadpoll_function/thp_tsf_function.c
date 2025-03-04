//服务端
#include "../lib.h"
#include <sys/sendfile.h>

void update_progress_bar(long current, long total, clock_t* last_time);

void handl_upload(int socketfd, char* filename){
    //接受上传文件
    printf("正在接受来自客户端xxx的文件...\n");//可加客户ip

    // 接收文件大小
    long file_size;
    if (recv(socketfd, &file_size, sizeof(file_size), 0) <= 0) {
        perror("Failed to receive file size");
        close(socketfd);
    }
    printf("File size: %ld bytes\n", file_size);

    // 打开目标文件
    char output_path[512];
    snprintf(output_path, sizeof(output_path), "../fileshouse_server/%s", filename);

    // 文件名冲突检测(可去掉，只要能接受就可以)
    int counter = 1;
    while (access(output_path, F_OK) != -1) {
        char ext[32] = {0};
        char name[224] = {0};
        // 分离文件名和扩展名
        char *dot = strrchr(filename, '.');
        if (dot) {
            strncpy(ext, dot, sizeof(ext)-1);  // 保留扩展名
            strncpy(name, filename, dot - filename);  // 主文件名
        } else {
            strncpy(name, filename, sizeof(name)-1);
        }
        // 生成新文件名
        snprintf(output_path, sizeof(output_path), "../fileshouse_server/%s_%d%s", name, counter++, ext);
    }

    int output_fd = open(output_path, O_RDWR | O_CREAT, 0644);
    if (output_fd < 0) {
        perror("文件打开失败");
        close(socketfd);
    }

    // 获取文件当前大小（用于断点续传）
    off_t offset = lseek(output_fd, 0, SEEK_END);
    printf("Resuming upload from offset: %ld\n", offset);

    // 发送起始偏移量给客户端
    send(socketfd, &offset, sizeof(offset), 0);

    // 接收文件内容
    long total_received = offset;
    char buffer[4096];
    clock_t last_time = clock();  // 初始化计时器
    while (total_received < file_size) {
        int bytes_to_read = (file_size - total_received > 4096) ? 4096 : (file_size - total_received);
        int bytes_received = recv(socketfd, buffer, bytes_to_read, 0);
        if (bytes_received <= 0) {
            perror("recv failed");
            break;
        }

        // 写入文件
        lseek(output_fd, total_received, SEEK_SET);
        write(output_fd, buffer, bytes_received);

        total_received += bytes_received;

        // 更新进度条
        update_progress_bar(total_received, file_size, &last_time);
        fflush(stdout);
    }
    printf("\nFile received and saved as '%s'\n", output_path);

    // 关闭文件和连接
    close(output_fd);
    close(socketfd);





    printf("客户端xxx上传文件完成，断开数据TCP连接");
}

void handl_download(int socketfd, char* filename){
         //发送下载文件
        printf("下载处理中...\n");

        // 打开要发送的文件
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "../fileshouse_server/%s", filename);

        int file_fd = open(file_path, O_RDONLY);
        if (file_fd < 0) {
            perror("文件打开失败");
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

    //取出数据库连接
    MYSQL* p_mysql = qam->p_mysql;
    //从队列中取出任务
    cmd_tast* t = (cmd_tast*)tast_queue_pop(qam->q);//线程一般会在这阻塞和唤醒

    //取出文件名
    char* content = t->content;
    char* filename = strtok(content," ");
    filename = strtok(NULL," ");
    printf("filename:%s\n",filename);

    //从已完成监听套接字中连接队列中取出一个连接请求
    int listenfd = t->peerfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int socketfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    printf("已和客户端子线程函数建立了TCP连接\n");
    //----------------------已和客户端子线程函数建立了TCP连接------------------------------
    if(t->cmdType == CMD_TYPE_UPLOAD){
        handl_upload(socketfd, filename);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        handl_download(socketfd, filename);
    }

    return NULL;
}

void update_progress_bar(long current, long total, clock_t* last_time) {
    double progress = (double)current / total;
    int bar_width = 50;
    
    // 计算传输速度
    clock_t now = clock();
    double elapsed = (double)(now - *last_time) / CLOCKS_PER_SEC;
    double speed = (elapsed > 0) ? ((current - (progress * total)) / 1048576.0 / elapsed) : 0;
    
    printf("\033[33m[");
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) printf("\033[32m=");
        else if (i == pos) printf("\033[34m>");
        else printf(" ");
    }
    printf("\033[33m] \033[0m");
    
    printf("\033[36m%.2f%%\033[0m | ", progress * 100);
    printf("Size: \033[35m%.2fMB\033[0m | ", current / 1048576.0);
    printf("Speed: \033[31m%.2fMB/s\033[0m\r", speed);
    fflush(stdout);
    
    *last_time = now;
}
