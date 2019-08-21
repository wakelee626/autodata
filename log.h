#ifndef _OS_LOG_H
#define _OS_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <time.h>
#include <sys/mman.h>

#include <stdio.h>
#include <string.h>
//#include <zmq.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
//#include <czmq.h>
//#include <zmq_utils.h>
//#include <zmsg.h>
//#include <zframe.h>
#include <execinfo.h>
#include <errno.h>


#define LOG_MODE_APPEND "a+"
#define LOG_MODE_MERGE "r+"


#define PATH_LOG_RULES_ENGLISH "/b_iscsi/config/rules_oplog_english.csv"
#define PATH_LOG_RULES_CHINESE "/b_iscsi/config/rules_oplog_chinese.csv"
//#define PATH_LOG_LOCAL "/b_iscsi/log/local_log"

#define PATH_LOG_LOCAL "/b_iscsi/log/syslog"
#define PATH_LOG_MERGE "/b_iscsi/log/mergelog"


#define PATH_LOG_GLOBAL_TO_MERGE "/b_iscsi/log/global_log_to_merge"
#define PATH_LOG_GLOBAL "/b_iscsi/log/newlogbak/global_log" 
#define DOMAIN_CFG "/etc/iraid/node/domain.conf"
#define DOMAIN_CONFIGING_STATUS "/tmp/add_domain_flag"
#define LOG_CMD_ZK_USABLE "/root/iraid/SCRIPTS/is_zk_usable"
#define LOG_FILE_LEADER_NODE "/b_iscsi/config/map_dir/leader_info.conf"
#define LOG_FILE_LOCAL_NODE "/etc/iraid/node/local_node.conf"

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

#ifndef _COMMON_FILE_OPS
#define _COMMON_FILE_OPS
enum common_file_ops
{
   FILE_OPS_READ = 0,
   FILE_OPS_WRITE,
};
#endif

#ifndef _COMMON_ERROR_E
#define _COMMON_ERROR_E
enum common_error_e
{
    ERROR_ARG = 100,
	ERROR_DB,
    ERROR_MSG,
    ERROR_LOCK,
    ERROR_PATH, 
    ERROR_MMAP,
    ERROR_FORK,
    ERROR_MALLOC,
    ERROR_THREAD, 
    ERROR_TIMEOUT,
    ERROR_DISABLE,
    ERROR_DAEMON,
    ERROR_PID_EXIST,
    ERROR_RULE,
    ERROR_RULE_FILTER,
    ERROR_FILE_COUNT,
    ERROR_UNKNOW=200,
};
#endif

#ifndef _COMMON_MODE_E
#define _COMMON_MODE_E
enum common_mode_e
{
    MODE_OS = 100,   /*OS*/
	MODE_RAID,       /*RAID*/
    MODE_CLUSTER,    /*CLUSTER*/
    MODE_FW,         /*FW*/
    MODE_SAN,        /*SAN*/
    MODE_NAS,        /*NAS*/
    MODE_CVR,        /*CVR*/
    MODE_DOCKER,     /*DOCKER*/
    MODE_CEPH,       /*CEPH*/
    MODE_UNKNOW=200, /*UNKNOW*/
};
#endif

#ifndef _COMMON_PRINT_E
#define _COMMON_PRINT_E
enum common_level_e
{
#ifndef _COMMON_PRINT_DEBUG_E
#define _COMMON_PRINT_DEBUG_E
        PRINT_DEBUG = 0x00000001,     /* 存在debug文件(/tmp/common_debug)才输出到屏幕 */
#endif 
    
#ifndef _COMMON_PRINT_INFO_E
#define _COMMON_PRINT_INFO_E
        PRINT_INFO = 0x00000010,      /* 将打印输出到屏幕，并且定向到message(标识码: HIKOS_INFO) */
#endif
    
#ifndef _COMMON_PRINT_WARN_E
#define _COMMON_PRINT_WARN_E
         PRINT_WARN = 0x00000100,      /* 将打印输出到屏幕，并且定向到message(标识码: HIKOS_WARN) */
#endif
        
#ifndef _COMMON_PRINT_ALERT_E
#define _COMMON_PRINT_ALERT_E
        PRINT_ALERT = 0x00001000,     /* 将打印输出到屏幕，并且定向到message(标识码: HIKOS_ALERT)，输出和定向文件包括栈信息 */
#endif


    PRINT_OUTPUT = 0x10000000,
    PRINT_UNKNOW = 0x00000000,
};
#endif

int _hikos_print(char* mode,enum common_level_e level_e, 
    const char *file, const char *func, const int line, char *fstring, ...);

#define hikos_print(model,level, fstring...) \
    _hikos_print(model,level, __FILE__, __FUNCTION__, __LINE__, ##fstring)

int do_record_log_t(const int log_id, const char *logid_str, 
    const char *log_errcode, const char *log_element);
int do_record_log_hikos(const int log_id, const char *logid_str, 
    const char *log_errcode, const char *path, const char *log_element);
int do_record_alarm_log_t(const char *logid_str, const char *level, const char *msg);

int node_set_log(const int log_id, const char *log_details, const char *fmt, ...);
int merge_log(char* log_file);

int merge_log_handle(const log_info_t *p_log_info_t);
int check_domain_exist(void);
//void merge_log_handle_msg(zmsg_t *data_in, zmsg_t **data_out);
#endif
