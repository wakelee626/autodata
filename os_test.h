#define TEST_GET_ALL 0x00001000
#define TEST_LIB_LIST 0

typedef struct test_s
{
    list_head list;
    char key1[16];
    char key2[16];
    char key3[16];
    struct test_s *next;
} test_t;

int hikos_test(
    enum storage_action_e action_e, const long id, void *inbuffer,void **outbuffer);

