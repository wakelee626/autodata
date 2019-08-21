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
#define SYSTEM_CMD_NUM  10   //bn_system()函数参数最多个数
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
 * @brief 获取全部分区检测状态(不支持集群)
 * @param action_e  输入为enum storage_action_e
 * @param id 输入为0x00021001 等操作类型id
 * @param inbuffer 输入为对应命令的输入项
 * @param outbuffer  输出为对应命令的输出项
 * @retval 0 成功
 * @retval enum common_error_e 失败
 */
int hikos_log(
    enum storage_action_e action_e, const long id, void *inbuffer,void **outbuffer,char *fmt,...);

int log_hikos_get(enum storage_action_e action_e, const long id, void *inbuffer, void 
**outbuffer);


/*
* @brief 记录操作日志\升级日志
 * @param flag  LOG_OPERATION | LOG_UPGREDE  分别表示记录操作日志和升级日志
 * @param log_id  日志码
 * @param log_details  NULL
 * @retval 0 成功
 * @retval enum common_error_e 失败

*/
//int node_set_log(const int flag,const int log_id, const char *log_details, const char *fmt, ...);

/*
* @brief 记录报警日志
 * @param inbuffer 输入为对应命令的输入项,格式 'i:0x5f00;m:msg;l:1'
 * @retval 0 成功
 * @retval enum common_error_e 失败

*/
int record_alarm_log(void *inbuffer);

int report_log(const int type,void **outbuffer);

#endif  //_HIKOS_WATCHDOG_H
