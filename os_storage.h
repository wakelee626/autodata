#ifndef _OS_STORAGE_H
#define _OS_STORAGE_H

#include "os_list.h"
#include "os_cjson.h"
#define STORAGE_VERSION "OS V1.3.5 20190630"
#define STORAGE_BASE_DIR "/home/os/system"
#define STORAGE_BIN_PATH STORAGE_BASE_DIR"/bin"
#define STORAGE_CS_PATH STORAGE_BASE_DIR"/config/capability_sets.cfg"

enum storage_action_e
{
    STORAGE_ACTION_GET = 1,
    STORAGE_ACTION_ADD,
    STORAGE_ACTION_SET,
    STORAGE_ACTION_DEL,
    STORAGE_ACTION_CHK,
}; 

typedef struct 
{
    void *key;
    void *value;
} KEYVALUE_T;

typedef struct storage_cs_file_s
{
    list_head list;
    int api_enable;
    char api_interface[64];
    char api_command[64];
    char api_action[4];
} cs_file_t;

typedef struct storage_cgroup_s
{
    int flag;
    char cmd[256];
    char cpu[32];
    char cpuset[32];
    char mem[32];
    char read[32];
    char write[32];
} cgroup_t; 

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

typedef int (*hikos_callback) (
    enum storage_action_e action_e, const long id, void *inbuffer, void **outbuffer);

#include "os_common.h"
#include "os_log.h"
#include "os_cjson.h"
#include "os_test.h"
/*
#include "./hikos_hardraid.h"
#include "./hikos_sys.h"
#include "./hikos_disk.h"
#include "./hikos_bond.h"
#include "./hikos_watchdog.h"
#include "./hikos_network.h"
#include "../hikos_include/hikos_softraid.h"
*/

int hikos_storage(hikos_callback callback_f, 
    enum storage_action_e action_e, const long id, void *inbuffer, void **outbuffer);
int storage_autotest_handle(void);
void storage_add_json(const char *buf, char *global_json);
void storage_get_json(void);
void storage_del_json(void);
void storage_get_version(void);
int storage_get_baseinfo(int json, int json_mode);
int storage_cgroup_handle(char *cgroup_str);
int storage_man_handle(void);
void storage_interface_cgroup_release(cgroup_t *cgroup);
void storage_cgroup_unity(const char *command);
int storage_csv_handle(char *file, int column, CSV_T **outbuffer);

#endif 

