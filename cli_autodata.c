#include<stdio.h>
#include<unistd.h>
#include<getopt.h>
#include<string.h>
#include "os_list.h"
#include "os_storage.h"

char global_path_sub[1024] = {0};
char global_path_product[1024] = {0};
char global_path_abnormal[1024] = {0};

CSV_T *global_sub = NULL;
CSV_T *global_product = NULL;
CSV_T *global_abnormal = NULL;

int col_sub = 0, col_product = 0 , col_abnormal = 0;
char path_sub[128] = {0}, path_product[128] = {0}, path_abnormal[128] = {0};

#define CLI_STORAGE "/home/hikos/system/bin/cli_storage"

typedef struct {
    list_head list;
    int count;
    char scene[128];
} PRODUCT_ABNORMAL;

typedef struct {
    int count;
    int abnormal;

    list_head HEAD_ABNORMAL;
    PRODUCT_ABNORMAL *p_abnormal;
} PRODUCT_T; 


typedef struct {
	list_head list;
	int count;
	char material_number[1024];	
	char scene[128];
} SUB_ABNORMAL;

typedef struct
{
	list_head list;
	int count;
	char material_number[1024];
	char scene[128];
	
} SUB_DELIVER; 

typedef struct {
	int count;
	int abnormal;
	list_head HEAD_ABNORMAL;
	SUB_ABNORMAL *s_abnormal;
	SUB_DELIVER *s_deliver;
} SUB_T;

PRODUCT_T *p = NULL;
SUB_T *s = NULL;

LIST_HEAD(pt_head);
LIST_HEAD(st_head);
LIST_HEAD(sd_head);

static void help(void)
{

	printf("-p product_csv\n");
	printf("-e abnormal_csv\n");
	printf("-s sub_csv\n");
	printf("please use col:path\n");
	printf("生产表中生产树数量和部件表中部件数量不能放在最后一列表的位置\n");
    return ;
}

static int get_csv_info(
    int *col_sub, int *col_product, int *col_abnormal, 
    char *path_sub, char *path_product, char *path_abnormal)
{
    int ret = 0;

    if (2 != sscanf(global_path_sub, "%d:%s", col_sub, path_sub))
    {
        ret = -1;
        printf("%s: parameter error\n", global_path_sub);
        goto out;
    }

    if (2 != sscanf(global_path_product, "%d:%s", col_product, path_product))
    {
        ret = -1;
        printf("%s: parameter error\n", global_path_product); 
        goto out;
    }

    if (2 != sscanf(global_path_abnormal, "%d:%s", col_abnormal, path_abnormal))
    {
        ret = -1;
        printf("%s: parameter error\n", global_path_abnormal); 
        goto out;
    }

    if (-1 == access(path_sub, F_OK))
    {
        //ret = -1;
        printf("%s: No such file\n", path_sub);
        goto out;
    }

    if (-1 == access(path_product, F_OK))
    {
        ret = -1;
        printf("%s: No such file\n", path_product);
        goto out;
    }

    if (-1 == access(path_abnormal, F_OK))
    {
        ret = -1;
        printf("%s: No such file\n", path_abnormal);
    }

out:
    return ret;
}

 static void print()
{

	printf("this is print\n");
    char num = 'A';
    int flag = 0;
    CSV_T *p_csv = NULL, *p_csv_tmp = NULL;
    CSV_ROW_T *p_csv_row = NULL, *p_csv_row_tmp = NULL;
    
    if (global_abnormal && !bn_list_empty((list_head *)global_abnormal))
    {
        list_for_each_entry_safe(p_csv, p_csv_tmp, (list_head *)global_abnormal, list)
        {
            num = 'A';
            flag = 0;
            list_for_each_entry_safe(p_csv_row, p_csv_row_tmp, &p_csv->head_row, list)
            {
                if (p_csv_row->buf)
                {
                    common_trim_buf(COMMON_TRIM_LEFT_BLANK|COMMON_TRIM_RIGHT_BLANK, p_csv_row->buf);
    
                    if ('A' == num && '#' == p_csv_row->buf[0])
                    {
                        flag = 1;
                    }
    
                    if (1 == flag)
                    {
                        goto ignore;
                    }
    
                    printf("list%c:%s\n", num, p_csv_row->buf);
    
                ignore:
                    free(p_csv_row->buf);
                    p_csv_row->buf = NULL;
                }
    
                bn_list_del(&p_csv_row->list);
                free(p_csv_row);
                p_csv_row = NULL;
    
                num++;
            }
    
            bn_list_del(&p_csv->list);
            free(p_csv);
            p_csv = NULL;
    
            printf("\n");
        }
    }

} 

static int set_product_a(char *tmp,int product_abnormal_count)
{
	int ret = 0;
	char num = 'A';
	char col_value[256] = {0};
	char value[256] = {0};
	PRODUCT_ABNORMAL * cur;
	PRODUCT_ABNORMAL* p=(PRODUCT_ABNORMAL*)malloc(sizeof(PRODUCT_ABNORMAL));
	if(p == NULL)
	{	
		printf("malloc PRODUCT_ABNORMAL error\n");
		ret = -1;
		goto out;
	}
	strcpy(p->scene,tmp);
	list_for_each_entry(cur,&pt_head,list)
	{
	if(0 == strcmp(cur->scene,tmp))
	{
		cur->count++;
		goto out;
	}
	}
	
		p->count = 1;
		strcpy(p->scene,tmp);
		bn_list_add(&pt_head,&p->list);

	out:
		return ret;
}

 static int product_abnormal_count = 0;

 void get_product_abnormal_data(void)
{
    char num = 'A';
    int flag = 0;
    int flag_abnormal = 0;
    char value[256] = {0};
    char col_value[256] = {0};
    CSV_T *p_csv = NULL, *p_csv_tmp = NULL;
    CSV_ROW_T *p_csv_row = NULL, *p_csv_row_tmp = NULL;
    
    if (global_abnormal && !bn_list_empty((list_head *)global_abnormal))
    {
        list_for_each_entry_safe(p_csv, p_csv_tmp, (list_head *)global_abnormal, list)
        {
            num = 'A';
            flag = 0;
            flag_abnormal = 0;
            list_for_each_entry_safe(p_csv_row, p_csv_row_tmp, &p_csv->head_row, list)
            {
                if (p_csv_row->buf)
                {
                    common_trim_buf(COMMON_TRIM_LEFT_BLANK|COMMON_TRIM_RIGHT_BLANK, p_csv_row->buf);
    
                    if ('A' == num && '#' == p_csv_row->buf[0])
                    {
                        flag = 1;
                    }
    
                    if (1 == flag)
                    {
                        goto ignore;
                    }
    
                    //printf("list%c:%s\n", num, p_csv_row->buf);

                    snprintf(col_value, sizeof(col_value), 
                        "list%c:%s", num, p_csv_row->buf);

					
						if(strstr(col_value,"listG:"))
							{

								if(NULL != strcpy(value,(strstr(col_value,":")+1)))
								{
									product_abnormal_count += 1;
									set_product_a(value,product_abnormal_count);
								}
							}
					
                    if (strstr(col_value, "listF:"))
                    {
                        if(NULL != strcpy(value,(strstr(col_value,":")+1)))
                        	{
                        
                            	if (strstr(value, "后端—生产不良"))
                            	{
                                	flag_abnormal = 1;
                            	}
                        	}
                    }
    
                ignore:
                    if (flag_abnormal) 
                    {
                        free(p_csv_row->buf);
                        p_csv_row->buf = NULL;
                    }
                }

                if (flag_abnormal)
                {
              //      PRODUCT_T->abnormal++
                //    PRODUCT_ABMORMAL_T 
                }

                if (flag_abnormal)
                {
                    bn_list_del(&p_csv_row->list);
                    free(p_csv_row);
                    p_csv_row = NULL;
                }
        
                num++;
            }

            if (flag_abnormal)
            {
                bn_list_del(&p_csv->list);
                free(p_csv);
                p_csv = NULL;
            }
            
            //printf("\n");
        }
    }
    
    return ;
}
static int set_product_t(int count_product)
{
	int ret = 0;
	p=(PRODUCT_T*)malloc(sizeof(PRODUCT_T));
	if(p == NULL)
	{
		printf("product_t malloc error\n");
		ret = -1;
		goto out;
	}
	p->count = count_product;
	p->abnormal = product_abnormal_count;

	out:
		return ret;
}

static void get_product_data(void)
{
    FILE *pp = NULL;
    char cmd[256] = {0};
    char buf[256] = {0};
    int flag = 0;
    int count_product = 0;
    char col_last = 'A' + col_product - 1;
    char last_col_str[16] = {0};
    char value[256] = {0};
    int num = 0;

    sprintf(last_col_str, "list%c:", col_last);

    snprintf(cmd, sizeof(cmd), 
        "%s -S %d:%s 2>/dev/null", CLI_STORAGE, col_product, path_product);

    pp = popen(cmd, "r");
    if (!pp)
    {
        printf("%s: do command error\n", cmd);
        goto out;
    }

    while (fgets(buf, sizeof(buf), pp))
    {
    	//printf("%s\n",buf);
        if (strlen(buf) > 0 && '\n' == buf[strlen(buf) - 1])
        {
            buf[strlen(buf) - 1] = '\0';
        }

        if (!strstr(buf, "list"))
        {
            continue;
        }

        if (strstr(buf, "listB:3004"))
        {
            flag = 1;            
        }
        else if (strstr(buf, "listC"))
        {
            if (1 == sscanf(buf, "listC:%s", value))
            {
                if (strstr(value, "服务包") || 
                    strstr(value, "组件") || 
                    strstr(value, "软件") || 
                    strstr(value, "改制料包") || 
                    strstr(value, "云计算"))
                {
                    flag = 0;
                }
            }
        }
        else if (strstr(buf, "listD"))
        {
            if (1 == sscanf(buf, "listD:%s", value))
            {
                if (strstr(value, "改制包") || 
                    strstr(value, "硬盘包"))
                {
                    flag = 0;
                }
            }
        }
        else if (strstr(buf, "listE"))
        {
            sscanf(buf, "listE:%d", &num);
        }
		
        else if (strstr(buf, last_col_str))
        {
        	
            if (1 == flag)
            {
                count_product += flag * num;
            }

            flag = 0;
            num = 0;
        }
    }

	set_product_t(count_product);
out:
    if (pp)
    {
        pclose(pp);
        pp = NULL;
    }

    //printf("count_product:%d\n", count_product);
    
    return ;
}

static int set_sub_a(char *tmp,int sub_abnormal_count,char *material_number)
{
	int ret = 0;
	SUB_ABNORMAL * s = (SUB_ABNORMAL*)malloc(sizeof(SUB_ABNORMAL));
	SUB_ABNORMAL * cur;
	if(s == NULL)
	{
		printf("sub_a malloc error\n");
		ret = -1;
		goto out;
	}
	list_for_each_entry(cur,&st_head,list)
	{
		if(0 == strcmp(cur->scene,tmp))
		{
			cur->count++;
			if( !strstr(cur->material_number,material_number))
			{
				strncat(strncat(cur->material_number, ",", sizeof(cur->material_number) - 1), 
					material_number, sizeof(cur->material_number) - 1);
			}
			goto out;
		}
	}
	s->count = 1;
	strcpy(s->scene,tmp);
	strcpy(s->material_number,material_number);
	bn_list_add(&st_head,&s->list);

	out:
		return ret;
}

 static int set_sub_d(char *material_number,char *tmp,int sence_count)
{
	//printf("%s\n",tmp);
	int ret = 0;
	SUB_DELIVER* d = (SUB_DELIVER*)malloc(sizeof(SUB_DELIVER));
	SUB_DELIVER* cur;
	if(d == NULL)
	{
		printf("sub_a malloc error\n");
		ret = -1;
		goto out;
	}
	list_for_each_entry(cur,&sd_head,list)
	{
		if(0 == strcmp(cur->scene,tmp))
		{
			cur->count += sence_count;
			if( !strstr(cur->material_number,material_number))
			{
				strncat(strncat(cur->material_number, ",", sizeof(cur->material_number) - 1), 
					material_number, sizeof(cur->material_number) - 1);
			}
			goto out;
		}
	}
	d->count = sence_count;
	strcpy(d->scene,tmp);
	strcpy(d->material_number,material_number);
	bn_list_add(&sd_head,&d->list);

	out:
		return ret;
}


static int sub_abnormal_count = 0;

static void get_sub_abnormal()
{
	char num = 'A';
    int flag = 0;
    int flag_abnormal = 0;
    char value[256] = {0};
    char col_value[256] = {0};
	char material_number[128];
    CSV_T *p_csv = NULL, *p_csv_tmp = NULL;
    CSV_ROW_T *p_csv_row = NULL, *p_csv_row_tmp = NULL;
    
    if (global_abnormal && !bn_list_empty((list_head *)global_abnormal))
    {
        list_for_each_entry_safe(p_csv, p_csv_tmp, (list_head *)global_abnormal, list)
        {
            num = 'A';
            flag = 0;
            flag_abnormal = 0;
            list_for_each_entry_safe(p_csv_row, p_csv_row_tmp, &p_csv->head_row, list)
            {
                if (p_csv_row->buf)
                {
                    common_trim_buf(COMMON_TRIM_LEFT_BLANK|COMMON_TRIM_RIGHT_BLANK, p_csv_row->buf);
    
                    if ('A' == num && '#' == p_csv_row->buf[0])
                    {
                        flag = 1;
                    }
    
                    if (1 == flag)
                    {
                        goto ignore;
                    }
    
                    //printf("list%c:%s\n", num, p_csv_row->buf);

                    snprintf(col_value, sizeof(col_value), 
                        "list%c:%s", num, p_csv_row->buf);

					
						if(strstr(col_value,"listB"))
							{
								if(NULL != strcpy(value,(strstr(col_value,":")+1)))
									{
										sub_abnormal_count += 1;
									}
								
							}

						if(strstr(col_value,"listH"))
						{
							if(NULL != strcpy(material_number,(strstr(col_value,":")+1)))
							{
									if(0 != strcmp(material_number,"物料"))
									{
											set_sub_a(value,sub_abnormal_count,material_number);
									}
							}
						}
			
    
                ignore:
                    if (flag_abnormal) 
                    {
                        free(p_csv_row->buf);
                        p_csv_row->buf = NULL;
                    }
                }

                if (flag_abnormal)
                {
              //      PRODUCT_T->abnormal++
                //    PRODUCT_ABMORMAL_T 
                }

                if (flag_abnormal)
                {
                    bn_list_del(&p_csv_row->list);
                    free(p_csv_row);
                    p_csv_row = NULL;
                }
        
                num++;
            }

            if (flag_abnormal)
            {
                bn_list_del(&p_csv->list);
                free(p_csv);
                p_csv = NULL;
            }
            
            //printf("\n");
        }
    }
    
    return ;
}

static int set_sub_t(int sub_count)
{
	int ret = 0;
	s=(SUB_T*)malloc(sizeof(SUB_T));
	if(s == NULL)
	{
		printf("sub_t malloc error\n");
		ret = -1;
		goto out;
	}
	s->abnormal = sub_abnormal_count-1;
	s->count = sub_count;
	out :
		return ret;
}
static void get_sub_data()
{
	FILE *pp = NULL;
    char cmd[256] = {0};
    char buf[256] = {0};
    int flag = 0;
    int count_sub = 0;
    char col_last = 'A' + col_sub- 1;
    char last_col_str[16] = {0};
    char value[256] = {0};
    int num = 0;
	char sence[256] = {0};
	char material_number[128] = {0};

    sprintf(last_col_str, "list%c:", col_last);

    snprintf(cmd, sizeof(cmd), 
        "%s -S %d:%s 2>/dev/null", CLI_STORAGE, col_sub, path_sub);

    pp = popen(cmd, "r");
    if (!pp)
    {
        printf("%s: do command error\n", cmd);
        goto out;
    }

    while (fgets(buf, sizeof(buf), pp))
    {
    	//printf("%s\n",buf);
        if (strlen(buf) > 0 && '\n' == buf[strlen(buf) - 1])
        {
            buf[strlen(buf) - 1] = '\0';
        }

        if (!strstr(buf, "list"))
        {
            continue;
        }
		else if(strstr(buf,"listA"))
		{
			strcpy(material_number,(strstr(buf,":")+1));
		}
		else if(strstr(buf,"listB"))
		{
			//sscanf(buf,"listB:%s",sence);
			strcpy(sence,(strstr(buf,":")+1));
		}
        else if (strstr(buf, "listD"))
        {
        	flag=1;
            sscanf(buf, "listD:%d", &num);
        }
        else if (strstr(buf, last_col_str))
        {
        	
            if (1 == flag)
            {
                count_sub += flag * num;
            }

            flag = 0;
            num = 0;
        }
		if(0 != num)
		{
			set_sub_d(material_number,sence,num);
		}
    }

	set_sub_t(count_sub);
	
out:
    if (pp)
    {
        pclose(pp);
        pp = NULL;
    }

   // printf("count_sub:%d\n", count_sub);
    
    return ;
}


static void print_pa_list()
{
	FILE *fp = NULL;
	char buf[1024] = {0};
	PRODUCT_ABNORMAL *tmp;

	fp = fopen("/tmp/auto_data_product.log", "w");
	if (!fp)
	{
		goto out;
	}
	
	printf("生产不良分类  : \n");	
	list_for_each_entry(tmp,&pt_head,list)
	{
		snprintf(buf, sizeof(buf), "     %8d : %s\n",tmp->count, tmp->scene);
		fputs(buf, fp);
	}

out:
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}

	system("cat /tmp/auto_data_product.log 2>/dev/null|sort -rn");

	remove("/tmp/auto_data_product.log");
	
	return ;
}
static void print_sa_list(void)
{
	FILE *fp = NULL;
	char buf[1024] = {0};
	SUB_ABNORMAL *tmp;
	
	fp = fopen("/tmp/auto_data_subunit.log", "w");
	if (!fp)
	{
		goto out;
	}

	printf("组件不良分类   : \n");
	list_for_each_entry(tmp,&st_head,list)
	{
		snprintf(buf, sizeof(buf), "      %8d : %s  %s\n",tmp->count,tmp->material_number, tmp->scene);
		fputs(buf, fp);
	}
	
out:
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}

	system("cat /tmp/auto_data_subunit.log 2>/dev/null|sort -rn");

	remove("/tmp/auto_data_subunit.log");
	
	return ;
}

static void print_sd_list(void)
{
	FILE *fp = NULL;
	char buf[1024] = {0};
	SUB_DELIVER*tmp;
	
	fp = fopen("/tmp/auto_data_subdeliver.log", "w");
	if (!fp)
	{
		goto out;
	}

	printf("组件出货情况  : \n");
	list_for_each_entry(tmp,&sd_head,list)
	{
		snprintf(buf, sizeof(buf), "      %8d : %s  %s\n",tmp->count,tmp->material_number, tmp->scene);
		fputs(buf, fp);
	}
	
out:
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}

	system("cat /tmp/auto_data_subdeliver.log 2>/dev/null|sort -rn");

	remove("/tmp/auto_data_subdeliver.log");
	
	return ;
}
double do_defect_rate(int abnormal,int count)
{
	double defect_rate=0;
	defect_rate = (double)abnormal/(double)count;
	return defect_rate;
}
static int do_handle(void)
{
    int ret = 0;
    
    if (get_csv_info(
        &col_sub, &col_product, &col_abnormal, 
        path_sub, path_product, path_abnormal))
    {
        ret = -1;
		printf("get csv info error\n");
        goto out;
    }

    /* 获取abnormal表数据，并存入结构链表 */
    storage_csv_handle(path_abnormal, col_abnormal, &global_abnormal);

    /* 将生产表和不良数据表做对比，并获取数据 */

	get_product_abnormal_data();

	get_product_data();
	//print();
	get_sub_abnormal();
	get_sub_data();
	printf("product total : %d\n",p->count);
	printf("product abnor : %d(%lf%)\n",p->abnormal,do_defect_rate(p->abnormal,p->count)*100);
	printf("subunit total : %d\n",s->count);
	printf("subunit abnor : %d(%lf%)\n",s->abnormal,do_defect_rate(s->abnormal,s->count)*100);
	printf("\n\n");
	print_pa_list();
	printf("\n\n"); 
	print_sd_list();
	printf("\n\n");
	print_sa_list();

out:    
    return ret;
}

int main(int argc,char*argv[])
{
	int ret=0;
	int noption=0;

	const struct option long_options[] = {
		{"help", 0, 0,'h'},        
        {"sub_csv", 1, 0, 's'},
		{"product_csv", 1, 0, 'p'},
		{"abnormal_csv", 1, 0, 'e'},
		{0,0,0,0}
	};

	while (-1 != (noption = getopt_long_only(argc, argv, "hm:f:p:e:s:", long_options, NULL)))
	{
		switch (noption)
		{
			case 'p':
				strncpy(global_path_product, optarg, sizeof(global_path_product) - 1);
				break;
			case 'e':
				strncpy(global_path_abnormal, optarg, sizeof(global_path_abnormal)-1);
				break;
			case 's':
				strncpy(global_path_sub, optarg, sizeof(global_path_sub)-1);
				break;
			default:
				ret = -1;
                help();
				goto out;
		}
	}

    if (optind < argc)
    {
        ret = -1;
        help();
        goto out;
    }
    

out:
    return ret ? -1 : do_handle();
}
