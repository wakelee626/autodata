#ifndef __HIKOS_COMMON_H__
#define __HIKOS_COMMON_H__

#define COMMON_VER 0x00005001
#define COMMON_CRON 0x00005002
#define COMMON_RECORD_LOG 0x00005003
#define COMMON_GET_KEY_VALUE 0x00005004
#define COMMON_SET_KEY_VALUE 0x00005005
#define MATCH_COUNT 10
#define MATCH_LEN 256


#define COMMON_TRIM_ANNOTATIONS 0X00000001  /* 去除起始为'#'的字符串 */
#define COMMON_TRIM_LINE_FEEDS  0x00000010  /* 去除字符串末尾的换行符 */
#define COMMON_TRIM_LEFT_BLANK  0X00000100  /* 去除字符串左侧的空格 */
#define COMMON_TRIM_RIGHT_BLANK 0x00001000  /* 去除字符串右侧的空格 */


#define COMMON_TRIM_ALL         COMMON_TRIM_ANNOTATIONS|COMMON_TRIM_LINE_FEEDS|\
                                COMMON_TRIM_LEFT_BLANK|COMMON_TRIM_RIGHT_BLANK

#define COMMON_PATH_DEBUG       "/tmp/common_debug"
#define COMMON_PATH_LOG         "/home/hikos/system/log/hikos.log"
#define COMMON_PATH_CRON        "/home/hikos/system/config/cron"

#define MATCH_COUNT 10
#define MATCH_LEN 256

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

enum common_printlog_e
{
    /* resovled defect in log.h, reuse following code  */
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
};
/*
typedef struct
{
    list_head list;
    char *buf;
} CSV_ROW_T;

typedef struct 
{
    list_head list;
    list_head head_row;
    CSV_ROW_T *csv_row;
} CSV_T;
*/
#define common_print(print_e, fstring...) \
    _common_print(print_e, __FILE__, __FUNCTION__, __LINE__, ##fstring)
    
void _common_print(enum common_printlog_e print_e, 
    const char *file, const char *func, const int line, char *fstring, ...);
int common_cron(void);
int common_trim_buf(const long type, char *buf);
int common_daemon_generate(void);
int common_get_ctime(char *ctime);
int common_read_write_file(enum common_file_ops file_ops, 
    const char *path, mode_t mode, int operation, 
    char *buf, int count, off_t *offset);
int common_rwrite_file_value(enum common_file_ops file_ops,char *file_path,
    char *name,char *name_value,char *value,int len_name_value);
FILE * common_fopen(const char * path, const char * mode, int operation);
int common_file_read_write(int *fd, 
                 enum common_file_ops file_ops, 
                 char *buf, int count, off_t *offset);
int common_match(const char *pattern, char *str, 
    char result[MATCH_COUNT][MATCH_LEN]);
int common_file_ops(enum common_file_ops file_ops, 
                     const char *path, mode_t mode, int lock, 
                     char *buf, int count, off_t *offset);
void common_file_close(int *fd);
int common_file_open(const char *path, mode_t mode, int lock);
int command_get_csv(char *file, int column, CSV_T **outbuffer);

#endif /* __HIKOS_COMMON_H__ */

