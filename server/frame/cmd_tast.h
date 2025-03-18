#ifndef __CMD_TAST_FUNC_H
#define __CMD_TAST_FUNC_H

#include "../config/config.h"
//任务类型枚举类
typedef enum {
    CMD_TYPE_LS = 1,
    CMD_TYPE_PWD,
    CMD_TYPE_CD,
    CMD_TYPE_DOWNLOAD,
    CMD_TYPE_DOWNLOAD_BG,
    CMD_TYPE_UPLOAD,
    CMD_TYPE_MKDIR,
    CMD_TYPE_RMDIR,
    CMD_TYPE_RM,
    CMD_TYPE_NOTCMD,

    CMD_TYPE_REPLY,

    CMD_TYPE_LOGIN1,
    CMD_TYPE_LOGIN2,
    CMD_TYPE_LOGIN1_OK,
    CMD_TYPE_LOGIN2_OK,
    CMD_TYPE_LOGIN1_ERROR,
    CMD_TYPE_LOGIN2_ERROR,

    CMD_TYPE_REGIT1,
    CMD_TYPE_REGIT1_ERROR,//用户名以及存在
    CMD_TYPE_REGIT1_OK,//用户名不存在可以注册
    CMD_TYPE_REGIT2,
    CMD_TYPE_REGIT_OKK,//注册成功
}CMD;
//任务结构体
//服务端与客户端交流使用的结构体（很重要！！）
//peerfd:
    //客户端发送时：无意义
    //服务端发送时：消息处理方式  不打印/打印/下载/上传
    //在服务端内：与客户端通信的fd
//content:
    //客户端发送时：命令传输字符串
    //服务端发送时：要打印的内容
    //在服务端内：当容器存东西
//cmdType:
    //命令类型，看上面的枚举类型
//path:
    //客户端发送时：客户端当前路径
    //服务端发送时：命令处理之后的客户端的路径
    //在服务端内：用于命令参数，最好不要改动
typedef struct {
    int peerfd;
    char content[MAX_CMD_SIZE];
    CMD cmdType;
    char path[PATH_SIZE];
    int Is_printf;
    char remain[MAX_CMD_SIZE];
}cmd_tast;

#endif
