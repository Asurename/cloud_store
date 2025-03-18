#ifndef __CMD_TAST_FUNC_H
#define __CMD_TAST_FUNC_H

#include "../config/config.h"
//��������ö����
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
    CMD_TYPE_REGIT1_ERROR,//�û����Լ�����
    CMD_TYPE_REGIT1_OK,//�û��������ڿ���ע��
    CMD_TYPE_REGIT2,
    CMD_TYPE_REGIT_OKK,//ע��ɹ�
}CMD;
//����ṹ��
//�������ͻ��˽���ʹ�õĽṹ�壨����Ҫ������
//peerfd:
    //�ͻ��˷���ʱ��������
    //����˷���ʱ����Ϣ����ʽ  ����ӡ/��ӡ/����/�ϴ�
    //�ڷ�����ڣ���ͻ���ͨ�ŵ�fd
//content:
    //�ͻ��˷���ʱ��������ַ���
    //����˷���ʱ��Ҫ��ӡ������
    //�ڷ�����ڣ��������涫��
//cmdType:
    //�������ͣ��������ö������
//path:
    //�ͻ��˷���ʱ���ͻ��˵�ǰ·��
    //����˷���ʱ�������֮��Ŀͻ��˵�·��
    //�ڷ�����ڣ����������������ò�Ҫ�Ķ�
typedef struct {
    int peerfd;
    char content[MAX_CMD_SIZE];
    CMD cmdType;
    char path[PATH_SIZE];
    int Is_printf;
    char remain[MAX_CMD_SIZE];
}cmd_tast;

#endif
