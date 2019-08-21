#ifndef __LIST_H__
#define __LIST_H__

#include <inttypes.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#ifdef __linux__
#  include <linux/types.h>
#endif

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __GNUC__
# define __typeof__ typeof
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct list_head{
    struct list_head *next, *prev;
}list_head;


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void bn_list_init(struct list_head *head)
{
	head->next = head->prev = head;
}

/* 
 * β����
 * Insert an element before 'head'.
 * If 'head' is the list head, this adds an element to the end of the list.
 */
static inline void bn_list_add(struct list_head *head, struct list_head *elem)
{
	assert(head->next);

	elem->next = head;
	elem->prev = head->prev;

	head->prev->next = elem;
	head->prev = elem;
}

/* 
 * ͷ����
 * Insert an element after 'head'.
 * If 'head' is the list head, this adds an element to the front of the list.
 */
static inline void bn_list_add_h(struct list_head *head, struct list_head *elem)
{
	assert(head->next);

	elem->next = head->next;
	elem->prev = head;

	head->next->prev = elem;
	head->next = elem;
}

/* 
 * ɾ��������elemԪ��
 * Delete an element from its list.
 * Note that this doesn't change the element itself - it may still be safe
 * to follow its pointers.
 */
static inline void bn_list_del(struct list_head *elem)
{
	elem->next->prev = elem->prev;
	elem->prev->next = elem->next;
}

/* 
 * ��elem���������Ƴ���������β
 * Remove an element from existing list and insert before 'head'.
 */
static inline void bn_list_move(struct list_head *head, struct list_head *elem)
{
    bn_list_del(elem);
    bn_list_add(head, elem);
}

/*
 * �ж������Ƿ�Ϊ��
 * Is the list empty?
 */
static inline int bn_list_empty(const struct list_head *head)
{
	return head->next == head;
}

static inline int bn_list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 * ��head��������һ���ڵ��ƶ�����ͷ
 */
static inline void list_rotate_left(struct list_head *head)
{
	struct list_head *first;

	if (!bn_list_empty(head)) {
		first = head->next;
		bn_list_move(first, head);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 * �ж�head�����Ƿ�ֻ��һ���ڵ�
 */
static inline int list_is_singular(const struct list_head *head)
{
	return !bn_list_empty(head) && (head->next == head->prev);
}

/*
 * �ж�elem�Ƿ��������еĵ�һ��Ԫ��
 * Is this the first element of the list?
 */
static inline int bn_list_start(const struct list_head *head, const struct list_head *elem)
{
	return elem->prev == head;
}

/*
 * �ж�elem�Ƿ��������е����һ��Ԫ��
 * Is this the last element of the list?
 */
static inline int bn_list_end(const struct list_head *head, const struct list_head *elem)
{
	return elem->next == head;
}

/*
 * ���������е�һ��Ԫ��
 * Return first element of the list or NULL if empty
 */
static inline struct list_head *bn_list_first(const struct list_head *head)
{
	return (bn_list_empty(head) ? NULL : head->next);
}

/*
 * �������������һ��Ԫ��
 * Return last element of the list or NULL if empty
 */
static inline struct list_head *bn_list_last(const struct list_head *head)
{
	return (bn_list_empty(head) ? NULL : head->prev);
}

/* 
 * ����elem֮ǰ��Ԫ��
 * Return the previous element of the list, or NULL if we've reached the start.
 */
static inline struct list_head *bn_list_prev(const struct list_head *head, const struct list_head *elem)
{
	return (bn_list_start(head, elem) ? NULL : elem->prev);
}

/* 
 * ����elem֮���Ԫ��
 * Return the next element of the list, or NULL if we've reached the end.
 */
static inline struct list_head *bn_list_next(const struct list_head *head, const struct list_head *elem)
{
	return (bn_list_end(head, elem) ? NULL : elem->next);
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 * ��̬��������list_replace ��������������old ��new ��
 * ��ͨ��new �滻old ����list_replace_init ��������ͨ������list_replace �����滻��
 * ֮�����INIT_LIST_HEAD �Ա��滻��old ���������ʼ����
 */
static inline void bn_list_replace(struct list_head *old,struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void bn_list_replace_init(struct list_head *old,
					struct list_head *new)
{
	bn_list_replace(old, new);
	INIT_LIST_HEAD(old);
}

/*
 * Set v to each element of a list in turn.
 */
#define bn_list_iterate(v, head) \
	for (v = (head)->next; v != head; v = v->next)

/* ���������С
 * Return the number of elements in a list by walking it.
 */
static inline unsigned int bn_list_size(const struct list_head *head)
{
	unsigned int s = 0;
	const struct list_head *v;

	bn_list_iterate(v, head)
	    s++;

	return s;
}

/**
 * �ϲ�����
 * Join 'head1' to the end of 'head'.
 */
static inline void bn_list_splice(struct list_head *head, struct list_head *head1)
{
	assert(head->next);
	assert(head1->next);

	if (bn_list_empty(head1))
	    return;

	head1->prev->next = head;
	head1->next->prev = head->prev;

	head->prev->next = head1->next;
	head->prev = head1->prev;

	bn_list_init(head1);
}

#define list_add(list, member) do { \
    typeof(list) *p = &list;        \
    while(*p) p = &(*p)->next;      \
    *p = member;                    \
    } while(0)

/**
 * ������ĵ���
 * link��ʾָ��struct list_head��Ա�����ĵ�ַ��type��ʾ����ṹ�������
 * member��ʾstruct list_head�ڸýṹ���еı������ơ�
 * ͨ����֪��ָ��member�����ָ�룬��������ṹ���ָ�루��ַ��
 */
#define list_entry(link, type, member) \
    ((type *)((char *)(link)-(unsigned long)(&((type *)0)->member)))

#define list_head(list, type, member)        \
    list_entry((list)->next, type, member)

#define list_tail(list, type, member)        \
    list_entry((list)->prev, type, member)

#define list_next(elm, member)                    \
    list_entry((elm)->member.next, typeof(*elm), member)

#define list_prev(elm, member)                    \
    list_entry((elm)->member.prev, typeof(*elm), member)
	   
/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 * ��������ȡ����������һԪ�ص���ָ�ṹ����׵�ַ
 * ptr��ʾָ��struct list_head��Ա�����ĵ�ַ��type��ʾ����ṹ������ͣ�member��ʾstruct list_head�ڸýṹ���еı������ơ�
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_struct within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 * ���ʵ���Ͼ���һ��forѭ������ͷ��β��������prefetch()����Ԥȡ�Դ����Ч�ʡ�
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; prefetch(pos->next), pos != (head); \
            pos = pos->next)

/**
 * __list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * This variant doesn't differ from list_for_each() any more.
 * We don't do prefetching in either case.
 */
#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 * pos��ʾ�ṹ�������head��ʾ�������Ŀ�ʼ�ڵ㣬��list_head���ͣ�member��list_head�ڽṹ�嵱�еı��������֡�
 * ��������Ĺ��ܾ������pos�ǿգ���ôpos��ֵ��Ϊ�䱾�����posΪ�գ���ô�ʹ�����ͷǿ����չһ����posָ�룬����궨����Ϊ����list_for_each_entry_continue()��ʹ����׼���ġ�
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ��������ñ������Բ��������ͷ��ʼ���������Դ�һ��ָ����pos�ڵ����
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ������˳���෴��
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ��������ı����Ǵӵ�ǰ����㿪ʼ�����ġ�
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ��list_for_each_entry_continue()�������ƣ��������safe��Ϊ�˷�ֹɾ���ڵ����ɶ����ķ�����
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = list_entry(pos->member.next, typeof(*pos), member), 		\
		n = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * �ӵ�ǰ�Ľڵ㿪ʼ������
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

/** 
 * ��ͷ��β����
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * head:��¼�������ݵ�����  pos:��list�е�Ԫ����ȡ��  member:���pos��Ԫ��
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/** 
 * ��β��ͷ����
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))	

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ��list_for_each_entry�ı������ƣ��������safe��Ϊ�˷�ֹɾ���ڵ����ɶ����ķ���
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * ��list_for_each_entry_safe���ƣ�����������˳��պ��෴��
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member))	     
	     	
/*
 * ͨ�������ͷ������Ա,�Ƚ������Ƴ����ٽ�������ڴ��ͷš�
 */
#define release_list(list, type, member)   \
    ({ type *_pos = NULL;     \
       while(!bn_list_empty(list)){        \
           _pos =  list_head(list, type, member);   \
           if(_pos){                                    \
                bn_list_del(&(_pos->member));        \
                free(_pos);                           \
                _pos = NULL;                          \
            }                                         \
        }                                      \
    })	


#ifdef __cplusplus
}
#endif				  

#endif		

