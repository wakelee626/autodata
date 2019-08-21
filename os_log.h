/* watchdog command */
#ifndef _HIKOS_LOG_H
#define _HIKOS_LOG_H

#include "./os_list.h"
#include "./log.h"

#define CMD_LEN 1024
#define BUF_LEN 512
#define PATH_LEN 128
#define OPT_LEN 32

#define LOG_GET_MAINTAIN_LOG 0x00150001
#define LOG_GET_OPERATION_LOG 0x00150002
#define LOG_GET_UPGRADE_LOG 0x00150003
#define LOG_SET_OPERATION_LOG 0x00150004
#define LOG_SET_ALARM_LOG 0x00150005
#define LOG_SET_UPGRADE_LOG 0x00150006

#define LOG_OPERATION 0
#define LOG_UPGRADE 1

#ifndef _FILE_HANDLE_ERR
#define _FILE_HANDLE_ERR -100
#endif

#ifndef _LOG_INFO_S
#define _LOG_INFO_S
typedef struct log_info_s
{
    char time[128];
    char level[16];
    char logid[16];
    char device[32];
    char type[1024];
    char details[4096];
    char element[4096];
    char describe[4096];
	int merge_type;
} log_info_t;
#endif

#ifndef SYSTEM_CMD_NUM
#define SYSTEM_CMD_NUM  10   //bn_system()��������������
#endif

#define __command_line(cmd)  command_line("/bin/sh", "-c", cmd, NULL)

typedef struct new_log_struct
{
	char nodeId[64];
	char StartDateTime[64];
	char level[64];
	char logDetail[4096];
	char logID[64];
}NEWLOGINFO;

typedef struct log_s
{
    //list_head list;
    char key1[128];
    char key2[128];
    char key3[1024];
    struct log_s *next;
} log_t;

enum log_err_e
{
    ERR_LOG_FILE = 1,        
    ERR_LOG_MALLOC,
    ERR_LOG_PARAM,
    ERR_LOG_LEADER,
};

//static log_info_t *p_global_log = NULL;
//#define PATH_LOG_RULES_ENGLISH "/home/hikos/system/config/rules_oplog_english.csv"
//#define PATH_LOG_RULES_CHINESE "/home/hikos/system/config/rules_oplog_chinese.csv"

#define LOG_MODE_APPEND "a+"
#define LOG_MODE_MERGE "r+"

#define PATH_LOG_OPERATION "/home/hikos/system/log/syslog"
#define PATH_LOG_UPGRADE "/home/hikos/system/log/upgradelog"

void log_help(void);

/**
 * @brief ��ȡȫ���������״̬(��֧�ּ�Ⱥ)
 * @param action_e  ����Ϊenum storage_action_e
 * @param id ����Ϊ0x00021001 �Ȳ�������id
 * @param inbuffer ����Ϊ��Ӧ�����������
 * @param outbuffer  ���Ϊ��Ӧ����������
 * @retval 0 �ɹ�
 * @retval enum common_error_e ʧ��
 */
int hikos_log(
    enum storage_action_e action_e, const long id, void *inbuffer,void **outbuffer,char *fmt,...);

int log_hikos_get(enum storage_action_e action_e, const long id, void *inbuffer, void 
**outbuffer);


/*
* @brief ��¼������־\������־
 * @param flag  LOG_OPERATION | LOG_UPGREDE  �ֱ��ʾ��¼������־��������־
 * @param log_id  ��־��
 * @param log_details  NULL
 * @retval 0 �ɹ�
 * @retval enum common_error_e ʧ��

*/
//int node_set_log(const int flag,const int log_id, const char *log_details, const char *fmt, ...);

/*
* @brief ��¼������־
 * @param inbuffer ����Ϊ��Ӧ�����������,��ʽ 'i:0x5f00;m:msg;l:1'
 * @retval 0 �ɹ�
 * @retval enum common_error_e ʧ��

*/
int record_alarm_log(void *inbuffer);

int report_log(const int type,void **outbuffer);

#endif  //_HIKOS_WATCHDOG_H
