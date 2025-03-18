//服务端
#include "../lib.h"
#include <sys/sendfile.h>
#include <openssl/sha.h>
#include <l8w8jwt/decode.h>
#include <l8w8jwt/encode.h>
//#include "../token/token.h"
typedef struct {
   int parent_id;
   char *filename;//已有
   int owner_id;
   char* hash;//已有
   int filesize;//已有
   int type;//已有
   char* file_path;//已有
}info_t;
void update_progress_bar(long current, long total, clock_t* last_time);

bool mysql_check_hash_table(MYSQL *mysql, const char *hash);//检查数据库中是否存在哈希值
bool mysql_check_virtualPath_table(MYSQL *mysql, const char *file_path);//检查数据库中是否存在文件路径
bool mysql_check_virtualPath_and_hash(MYSQL *mysql, const char *file_path, const char *hash);//检查数据库中是否存在文件路径和哈希值都相同
void mysql_update_hash(MYSQL *mysql, const char *file_path, const char *new_hash);//更新数据库中传入file_path的哈希值
void mysql_add_newline(MYSQL *mysql, const char *currentPath, const char *hash, info_t info);
void mysql_get_hash_by_filePath(MYSQL *mysql, const char *file_path, char *hash);//根据文件路径获取哈希值

void handl_upload(int socketfd, char* filename, char* currentPath, MYSQL * p_mysql){

    // 接收客户端发送的哈希值
    char client_hash[65] = {0};
    if (recv(socketfd, client_hash, sizeof(client_hash), 0) <= 0) {
        perror("接收哈希值失败");
        close(socketfd);
    }
    printf("客户端发送的哈希值: %s\n", client_hash);


    //接受上传文件
    printf("正在接受来自客户端xxx的文件...\n");//可加客户ip

    // 接收文件大小
    long file_size;
    if (recv(socketfd, &file_size, sizeof(file_size), 0) <= 0) {
        perror("Failed to receive file size");
        close(socketfd);
    }
    printf("File size: %ld bytes\n", file_size);


    //------------------------------以下是接入数据库逻辑--------------------------
    // 构建虚拟路径
    char* virtualPath = (char*)malloc(512);
    snprintf(virtualPath, 512, "%s/%s", currentPath, filename);
    printf("virtualPath: %s\n", virtualPath);

    //填充结构体
    info_t info;
    info.filename = filename;
    info.hash = client_hash;
    info.filesize = file_size;
    info.file_path = virtualPath;
    info.type = 2;

    bool hash_exits = mysql_check_hash_table(p_mysql, client_hash);
    bool virtualPath_exits = mysql_check_virtualPath_table(p_mysql, virtualPath);
    bool virtualPath_and_hash_exits =mysql_check_virtualPath_and_hash(p_mysql, virtualPath, client_hash);

    // 同一个目录下上传 一模一样的文件，告之客户端：文件已存在，无需上传！
    if(virtualPath_and_hash_exits && virtualPath_exits){
        printf("文件已存在，无需上传！\n");
        // 回复标志1
        send(socketfd, "1", 1, 0);

        close(socketfd);
        free(virtualPath);
        return;//直接结束，断开数据连接。
    }
    // 同一个目录下上传 名字一样但内容不一样的文件，告之客户端：文件已存在，但是内容不一样，是否覆盖？
    if(!virtualPath_and_hash_exits && virtualPath_exits){
        // 回复标志2:文件名已存在，但是内容不一样，是否覆盖？
        char choice;
        send(socketfd, "2", 1, 0);
        recv(socketfd, &choice, 1, 0);
        if(choice == 'y'){
            mysql_update_hash(p_mysql, virtualPath, client_hash);
            printf("文件开始覆盖！\n");
            //跳转到文件上传逻辑---->>>>>>
        }else{
            printf("文件上传已取消！\n");
            close(socketfd);
            free(virtualPath);
            return; //直接结束，断开数据连接。
        }

    }
    // 不同目录下上传 内容一样的文件
    if(hash_exits &&!virtualPath_exits ){
        // 回复标志3:秒传
        send(socketfd, "3", 1, 0);
        mysql_add_newline(p_mysql, currentPath, client_hash,info);
        printf("文件秒传！只用数据库加一行！\n");
        close(socketfd);
        free(virtualPath);
        return; //直接结束，断开数据连接。
    }

    // 不同目录下上传内容不一样的文件
    if(!hash_exits &&!virtualPath_exits){
        // 回复标志4:什么都不同，最普遍的情况
        send(socketfd, "4", 1, 0);
        //跳转到文件上传逻辑---->>>>>>
    }

//------------------------------以下是文件上传逻辑--------------------------  

    // 打开目标文件 output_path="../fileshouse_server/"+hash
    char output_path[512];
    snprintf(output_path, sizeof(output_path), "../fileshouse_server/%s", client_hash);

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

    

     // 接收完文件后验证哈希
    printf("\n开始验证文件完整性...\n");

    // 重新打开文件计算哈希
    int verify_fd = open(output_path, O_RDONLY);
    unsigned char server_hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer2[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(verify_fd, buffer2, sizeof(buffer))) > 0) {
        SHA256_Update(&sha256, buffer2, bytes_read);
    }
    SHA256_Final(server_hash, &sha256);
    close(verify_fd);

    // 转换服务端哈希为字符串
    char server_hash_str[65] = {0};
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(&server_hash_str[i*2], "%02x", server_hash[i]);

    // 比较哈希值
    if (strcmp(client_hash, server_hash_str) == 0) {
        //传输完数据，插入数据库
        mysql_add_newline(p_mysql, currentPath, client_hash,info);
        printf("文件校验成功，哈希值匹配！\n");
    } else {
        printf("警告：文件校验失败！\n");
        printf("客户端哈希：%s\n", client_hash);
        printf("服务端哈希：%s\n", server_hash_str);
    }


    // 关闭文件和连接
    close(output_fd);
    close(socketfd);





    printf("客户端xxx上传文件完成，断开数据TCP连接");
}

void handl_download(int socketfd, char* filename,  char* currentPath, MYSQL * p_mysql){
         //发送下载文件
        printf("下载处理中...\n");

        
        // 根据文件路径获取哈希值
        char* virtual_file_path = (char*)malloc(512);
        snprintf(virtual_file_path, 512, "%s/%s", currentPath, filename);
        char file_hash[65] = {0};
        mysql_get_hash_by_filePath(p_mysql, virtual_file_path, file_hash);
        printf("virtual_file_path:%s",virtual_file_path);

        // 打开要发送的文件 output_path="../fileshouse_server/"+hash
        char file_path[256];
        snprintf(file_path, sizeof(file_path), "../fileshouse_server/%s", file_hash);
    


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
    //取出虚拟当前路径
    char* currentPath = t->path;
    printf("currentPath:%s\n",currentPath);

    //从已完成监听套接字中连接队列中取出一个连接请求
    int listenfd = t->peerfd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int socketfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_addr_len);
    printf("已和客户端子线程函数建立了TCP连接\n");
    //----------------------已和客户端子线程函数建立了TCP连接------------------------------
    cmd_tast t2;
    memset(&t2,0,sizeof(t2));
    recv(socketfd,(void*)&t2,sizeof(t2),MSG_WAITALL);
    int validation_result =  decode(t2.remain);
    if(validation_result != L8W8JWT_VALID)
    {
        printf("令牌验证失败\n");
        close(socketfd);
        return (void*)-1;
    }
    printf("令牌验证成功\n");
    if(t->cmdType == CMD_TYPE_UPLOAD){
        handl_upload(socketfd, filename, currentPath, p_mysql);
    }
    if(t->cmdType == CMD_TYPE_DOWNLOAD){
        handl_download(socketfd, filename, currentPath, p_mysql);
    }

    return NULL;
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

// 检查数据库中是否存在哈希值
bool mysql_check_hash_table(MYSQL *mysql, const char *hash)
{
    // 确保输入的哈希值不为空
    if (!hash || strlen(hash) == 0)
    {
        fprintf(stderr, "Error: Hash value is empty.\n");
        return false;
    }

    // 构造 SQL 查询
    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM virtual_file_table WHERE hash = '%s'", hash);
    //  "SELECT COUNT(*) FROM virtual_file_table WHERE type = 1");
    // 执行查询
    if (mysql_query(mysql, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(mysql));
        return false;
    }

    // 获取结果集
    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result)
    {
        fprintf(stderr, "Error: %s\n", mysql_error(mysql));
        return false;
    }

    // 提取计数值
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row)
    {
        fprintf(stderr, "No rows returned.\n");
        mysql_free_result(result);
        return false;
    }

    int count = atoi(row[0]); // 将结果转换为整数
    printf("%d\n", count);
    // 清理资源
    mysql_free_result(result);

    // 如果计数值大于 0，表示哈希值存在
    return count > 0;
}

// 检查数据库中是否存在文件路径
bool mysql_check_virtualPath_table(MYSQL *mysql, const char *file_path)
{
    // 确保输入的文件路径不为空
    if (!file_path || strlen(file_path) == 0)
    {
        fprintf(stderr, "Error: File path is empty.\n");
        return false;
    }

    // 构造 SQL 查询
    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM virtual_file_table WHERE file_path = '%s'", file_path);

    // 执行查询
    if (mysql_query(mysql, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(mysql));
        return false;
    }

    // 获取结果集
    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result)
    {
        fprintf(stderr, "Error: %s\n", mysql_error(mysql));
        return false;
    }

    // 提取计数值
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row)
    {
        fprintf(stderr, "No rows returned.\n");
        mysql_free_result(result);
        return false;
    }

    int count = atoi(row[0]); // 将结果转换为整数

    // 清理资源
    mysql_free_result(result);

    // 如果计数值大于 0，表示文件路径存在
    return count > 0;
}

// 检查数据库中是否存在文件路径和哈希值都相同
bool mysql_check_virtualPath_and_hash(MYSQL *mysql, const char *file_path, const char *hash)
{
    // 确保输入的文件路径和哈希值不为空
    if (!file_path || strlen(file_path) == 0)
    {
        fprintf(stderr, "Error: File path is empty.\n");
        return false;
    }
    if (!hash || strlen(hash) == 0)
    {
        fprintf(stderr, "Error: Hash value is empty.\n");
        return false;
    }

    // 构造 SQL 查询
    char query[1024];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM virtual_file_table WHERE file_path = '%s' AND hash = '%s'",
             file_path, hash);

    // 执行查询
    if (mysql_query(mysql, query))
    {
        fprintf(stderr, "Query failed: %s\n", mysql_error(mysql));
        return false;
    }

    // 获取结果集
    MYSQL_RES *result = mysql_store_result(mysql);
    if (!result)
    {
        fprintf(stderr, "Error: %s\n", mysql_error(mysql));
        return false;
    }

    // 提取计数值
    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row)
    {
        fprintf(stderr, "No rows returned.\n");
        mysql_free_result(result);
        return false;
    }

    int count = atoi(row[0]); // 将结果转换为整数

    // 清理资源
    mysql_free_result(result);

    // 如果计数值大于 0，表示文件路径和哈希值都存在
    return count > 0;
}

// 更新数据库中传入 file_path 的哈希值
void mysql_update_hash(MYSQL *mysql, const char *file_path, const char *new_hash)
{
    // 确保输入的文件路径和新哈希值不为空
    if (!file_path || strlen(file_path) == 0)
    {
        fprintf(stderr, "Error: File path is empty.\n");
        return;
    }
    if (!new_hash || strlen(new_hash) == 0)
    {
        fprintf(stderr, "Error: New hash value is empty.\n");
        return;
    }

    // 构造 SQL 查询
    char query[1024];
    snprintf(query, sizeof(query),
             "UPDATE virtual_file_table SET hash = '%s' WHERE file_path = '%s'",
             new_hash, file_path);

    // 执行查询
    if (mysql_query(mysql, query))
    {
        fprintf(stderr, "Update failed: %s\n", mysql_error(mysql));
        return;
    }

    // 获取受影响的行数
    int affected_rows = mysql_affected_rows(mysql);
    if (affected_rows > 0)
    {
        printf("Updated hash for file_path '%s' to '%s'.\n", file_path, new_hash);
    }
    else
    {
        printf("No rows updated. File path '%s' may not exist in the database.\n", file_path);
    }
}
void mysql_add_newline(MYSQL *mysql,
                       const char *file_path,
                       const char *hash,
                       info_t info) {
    if (mysql == NULL || file_path == NULL || hash == NULL) {
        fprintf(stderr, "Invalid input parameters\n");
        return;
    }

    char query[1024];
    // 第一步：查询 parent_id 和 owner_id
    snprintf(query, sizeof(query), "SELECT id, owner_id FROM virtual_file_table WHERE file_path = '%s'", file_path);
    int ret = mysql_query(mysql, query);
    if (ret != 0) {
        fprintf(stderr, "Error executing query: %s\n", mysql_error(mysql));
        return;
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        fprintf(stderr, "Error getting result set: %s\n", mysql_error(mysql));
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        fprintf(stderr, "No matching record found for file_path: %s\n", file_path);
        mysql_free_result(res);
        return;
    }

    char parent_id_s[64];
    char owner_id_s[64];
    strncpy(parent_id_s, row[0], sizeof(parent_id_s) - 1);
    parent_id_s[sizeof(parent_id_s) - 1] = '\0';
    strncpy(owner_id_s, row[1], sizeof(owner_id_s) - 1);
    owner_id_s[sizeof(owner_id_s) - 1] = '\0';

    // 释放第一步查询的结果集
    mysql_free_result(res);

    // 第二步：插入新记录
    snprintf(query, sizeof(query),
             "INSERT INTO virtual_file_table (parent_id, filename, owner_id, hash, filesize, type, file_path) "
             "VALUES (%s, '%s', %s, '%s', %d, %d, '%s')",
             parent_id_s, info.filename, owner_id_s, info.hash, info.filesize, info.type, info.file_path);

    ret = mysql_query(mysql, query);
    if (ret != 0) {
        fprintf(stderr, "Error inserting new record: %s\n", mysql_error(mysql));
        return;
    }

    printf("New record inserted successfully.\n");
}

// 根据文件路径获取哈希值
void mysql_get_hash_by_filePath(MYSQL *mysql, const char *file_path, char *hash) {
    // 初始化 SQL 查询语句
    char query[1024];

    // 假设表名为 virtual_file_table，包含两列：file_path 和 hash
    snprintf(query, sizeof(query), "SELECT hash FROM virtual_file_table WHERE file_path = '%s'", file_path);

    // 执行 SQL 查询
    if (mysql_query(mysql, query) != 0) {
        fprintf(stderr, "MySQL query error: %s\n", mysql_error(mysql));
        // 如果查询出错，将哈希值置为空
        strcpy(hash, "");
        return;
    }

    // 获取查询结果
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == NULL) {
        fprintf(stderr, "MySQL result error: %s\n", mysql_error(mysql));
        // 如果获取结果出错，将哈希值置为空
        strcpy(hash, "");
        return;
    }

    // 获取结果集中的行数
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    // 检查是否有匹配的结果
    if ((row = mysql_fetch_row(result))) {
        // 检查目标缓冲区大小，避免缓冲区溢出
        size_t hash_length = strlen(row[0]);
        strcpy(hash, row[0]);
        
    } else {
        // 如果没有匹配的结果，将哈希值置为空
        strcpy(hash, "");
    }

    // 释放结果集
    mysql_free_result(result);
}
