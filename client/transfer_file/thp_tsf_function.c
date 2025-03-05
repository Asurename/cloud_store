//客户端
#include "thp_tsf_function.h"
#include <mysql/mysql.h>
#include <openssl/sha.h>//链接加-lssl -lcrypto

#define SAVE_OFFSET_THRESHOLD (10 * 1024 * 1024) // 每 1MB 保存一次偏移量

int should_exit = 0; // 标志变量，用于捕获退出信号
off_t last_saved_offset = 0; // 上次保存的偏移量
void handle_signal(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nCaught signal %d, saving offset and exiting...\n", sig);
        should_exit = 1;
    }
}

void update_progress_bar(long current, long total, clock_t* last_time);

off_t read_offset(const char *offset_file) {
    FILE *fp = fopen(offset_file, "rb");
    if (!fp) return 0;
    off_t offset;
    fread(&offset, sizeof(offset), 1, fp);
    fclose(fp);
    return offset;
}


void save_offset(const char *offset_file, off_t offset) {
    FILE *fp = fopen(offset_file, "wb");
    if (!fp) return;
    fwrite(&offset, sizeof(offset), 1, fp);
    fclose(fp);
}


void handl_upload(int socketfd, char* filename) {
         //上传文件
        printf("即将上传...\n");

        char input_path[512];
        char offset_path[512];
        // 构造输入文件路径
        snprintf(input_path, sizeof(input_path), "../fileshouse_client/%s", filename);
        // 构造偏移量文件路径
        snprintf(offset_path, sizeof(offset_path), "../fileshouse_client/%s_upload_offset.dat", filename);


        // 打开输入文件
        int input_fd = open(input_path, O_RDONLY);
        if (input_fd < 0) {
            perror("文件打开失败");
            close(socketfd);
        }


    // 计算文件哈希值
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    // 临时保存当前文件偏移
    off_t current_offset = lseek(input_fd, 0, SEEK_CUR);
    lseek(input_fd, 0, SEEK_SET);  // 回到文件开头计算完整哈希
    
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) {
        SHA256_Update(&sha256, buffer, bytes_read);
    }
    SHA256_Final(hash, &sha256);
    lseek(input_fd, current_offset, SEEK_SET);  // 恢复原来的偏移
    
    // 转换哈希值为字符串
    char hash_str[SHA256_DIGEST_LENGTH*2+1];
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&hash_str[i*2], "%02x", hash[i]);
    
    // 发送哈希值给服务端
    if (send(socketfd, hash_str, sizeof(hash_str), 0) <= 0) {
        perror("发送哈希值失败");
        close(input_fd);
        close(socketfd);
        return;
    }




        // 获取文件大小
        struct stat file_stat;
        if (fstat(input_fd, &file_stat) < 0) {
            perror("fstat failed");
            close(input_fd);
            close(socketfd);
        }
        long file_size = file_stat.st_size;

        // 发送文件大小给服务端
        if (send(socketfd, &file_size, sizeof(file_size), 0) <= 0) {
            perror("Failed to send file size");
            close(input_fd);
            close(socketfd);
        }

        // 接收服务端请求的起始偏移量
        off_t offset;
        if (recv(socketfd, &offset, sizeof(offset), 0) <= 0) {
            perror("Failed to receive offset");
            close(input_fd);
            close(socketfd);
        }
        printf("Resuming upload from offset: %ld\n", offset);

        // 读取断点偏移量
        off_t local_offset = read_offset(offset_path);
        if (local_offset > offset) {
            offset = local_offset; // 使用本地记录的偏移量
        }

        // 定位到文件的起始偏移量
        lseek(input_fd, offset, SEEK_SET);

        // 上传文件内容
        long total_sent = offset;
        memset(buffer, 0, sizeof(buffer));
        clock_t last_time = clock();  // 初始化计时器
        while (total_sent < file_size) {
            if (should_exit) break; // 如果捕获到退出信号，退出循环

            int bytes_to_send = (file_size - total_sent > 4096) ? 4096 : (file_size - total_sent);
            int bytes_read = read(input_fd, buffer, bytes_to_send);
            if (bytes_read <= 0) {
                perror("read failed");
                break;
            }

            int bytes_sent = send(socketfd, buffer, bytes_read, 0);
            if (bytes_sent <= 0) {
                perror("send failed");
                break;
            }

            total_sent += bytes_sent;

            // 更新进度条
            update_progress_bar(total_sent, file_size, &last_time);
            // 每隔一定数据量保存一次偏移量
            if (total_sent - last_saved_offset >= SAVE_OFFSET_THRESHOLD) {
                save_offset(offset_path, total_sent);
                last_saved_offset = total_sent;
            }
        }

        // 确保退出前保存最后一次偏移量
        if (total_sent > last_saved_offset) {
            save_offset(offset_path, total_sent);
        }

        // 删除偏移量文件（如果上传完成）
        if (total_sent >= file_size) {
            remove(offset_path);
        }

        printf("\nFile uploaded successfully.\n");

        // 关闭文件和连接
        close(input_fd);
        close(socketfd);

        printf("上传成功.\n");

}


void handl_download(int socketfd, char* filename) {
        //下载文件
        printf("即将下载...\n");

        // 打开输出文件
        char output_path[256];
        snprintf(output_path, sizeof(output_path), "../fileshouse_client/%s", filename);

        int output_fd = open(output_path, O_RDWR | O_CREAT, 0644);
        if (output_fd < 0) {
            perror("File open failed");
        }

        char offset_path[256];
        snprintf(offset_path, sizeof(offset_path), "../fileshouse_client/%s_download_offset.dat", filename);
        // 读取断点偏移量
        off_t offset = read_offset(offset_path);
        printf("Resuming download from offset: %ld\n", offset);

        // 发送起始偏移量给服务端
        send(socketfd, &offset, sizeof(offset), 0);

        // 接收文件大小
        long file_size;
        if (recv(socketfd, &file_size, sizeof(file_size), 0) <= 0) {
            perror("Failed to receive file size");
            close(output_fd);
            close(socketfd);
        }

        
        // 调整文件大小以容纳完整文件
        if (ftruncate(output_fd, file_size) < 0) {
            perror("ftruncate failed");
            close(output_fd);
        }

        // 使用 mmap 映射文件到内存
        void *file_map = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, output_fd, 0);
        if (file_map == MAP_FAILED) {
            perror("mmap failed");
            close(output_fd);
        }



        // 接收文件内容
        char buffer[4096] = {0};
        long total_received = offset;
        clock_t last_time = clock();
        while (total_received < file_size) {
            if (should_exit) break; // 如果捕获到退出信号，退出循环

            int bytes_to_read = (file_size - total_received > 4096) ? 4096 : (file_size - total_received);
             int bytes_received = recv(socketfd, (char *)file_map + total_received, bytes_to_read, 0);
            if (bytes_received <= 0) {
                perror("recv failed");
                break;
            }


            total_received += bytes_received;

            // 更新进度条
            update_progress_bar(total_received, file_size, &last_time);

            // 每隔一定数据量保存一次偏移量
            if (total_received - last_saved_offset >= SAVE_OFFSET_THRESHOLD) {
                save_offset(offset_path, total_received);
                last_saved_offset = total_received;
            }
        }

        // 确保退出前保存最后一次偏移量
        if (total_received > last_saved_offset) {
            save_offset(offset_path, total_received);
        }

        // 删除偏移量文件（如果下载完成）
        if (total_received >= file_size) {
            remove(offset_path);
        }


        // 关闭文件和连接
        munmap(file_map, file_size);
        close(output_fd);
        close(socketfd);



        printf("\n下载任务完成，断开数据TCP连接。\n");
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
    //取出文件名
    char* content = t->content;//download 1000mb_file
    char* filename = strtok(content," ");
    filename = strtok(NULL," ");
    printf("filename:%s\n",filename);
    
    //创建新socket，并connect到服务器
    int socketfd = tcp_connect("127.0.0.1", 12222);
    printf("已和服务端子线程函数建立了TCP连接\n");
    //----------------------已和服务端子线程函数建立了TCP连接------------------------------

    if(t->cmdType == CMD_TYPE_UPLOAD){
        handl_upload(socketfd, filename);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        handl_download(socketfd, filename);

    }

    return 0;
}

void update_progress_bar(long current, long total, clock_t* last_time) {
    double progress = (double)current / total;
    int bar_width = 30;  // 缩短进度条长度
    
    printf("\033[33m[");
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) printf("\033[32m=");
        else if (i == pos) printf("\033[34m>");
        else printf(" ");
    }
    printf("\033[33m] \033[0m");
    
    // 移除速度显示，只保留百分比和大小
    printf("\033[36m%.2f%%\033[0m | ", progress * 100);
    printf("Size: \033[35m%.2fMB\033[0m\r", current / 1048576.0);
    fflush(stdout);
    
    *last_time = clock();  // 简化时间更新逻辑
}
