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
 * 尾插入
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
 * 头插入
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
 * 删除链表中elem元素
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
 * 将elem从链表中移除放在链表尾
 * Remove an element from existing list and insert before 'head'.
 */
static inline void bn_list_move(struct list_head *head, struct list_head *elem)
{
    bn_list_del(elem);
    bn_list_add(head, elem);
}

/*
 * 判断链表是否为空
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
 * 将head链表的最后一个节点移动到表头
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
 * 判断head链表是否只有一个节点
 */
static inline int list_is_singular(const struct list_head *head)
{
	return !bn_list_empty(head) && (head->next == head->prev);
}

/*
 * 判断elem是否是链表中的第一个元素
 * Is this the first element of the list?
 */
static inline int bn_list_start(const struct list_head *head, const struct list_head *elem)
{
	return elem->prev == head;
}

/*
 * 判断elem是否是链表中的最后一个元素
 * Is this the last element of the list?
 */
static inline int bn_list_end(const struct list_head *head, const struct list_head *elem)
{
	return elem->next == head;
}

/*
 * 返回链表中第一个元素
 * Return first element of the list or NULL if empty
 */
static inline struct list_head *bn_list_first(const struct list_head *head)
{
	return (bn_list_empty(head) ? NULL : head->next);
}

/*
 * 返回链表中最后一个元素
 * Return last element of the list or NULL if empty
 */
static inline struct list_head *bn_list_last(const struct list_head *head)
{
	return (bn_list_empty(head) ? NULL : head->prev);
}

/* 
 * 返回elem之前的元素
 * Return the previous element of the list, or NULL if we've reached the start.
 */
static inline struct list_head *bn_list_prev(const struct list_head *head, const struct list_head *elem)
{
	return (bn_list_start(head, elem) ? NULL : elem->prev);
}

/* 
 * 返回elem之后的元素
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
 * 静态内联函数list_replace 接受两个参数：old 和new ，
 * 并通过new 替换old 。而list_replace_init 函数则是通过调用list_replace 进行替换，
 * 之后调用INIT_LIST_HEAD 对被替换的old 进行链表初始化。
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

/* 返回链表大小
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
 * 合并链表
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
 * 供下面的调用
 * link表示指向struct list_head成员变量的地址，type表示这个结构体的类型
 * member表示struct list_head在该结构体中的变量名称。
 * 通过已知的指向member子项的指针，获得整个结构体的指针（地址）
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
 * 这个宏就是取得这个链表第一元素的所指结构体的首地址
 * ptr表示指向struct list_head成员变量的地址，type表示这个结构体的类型，member表示struct list_head在该结构体中的变量名称。
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
 * 这个实际上就是一个for循环，从头到尾遍历链表。prefetch()用于预取以此提高效率。
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
 * pos表示结构体变量；head表示这个链表的开始节点，是list_head类型；member是list_head在结构体当中的变量的名字。
 * 这个函数的功能就是如果pos非空，那么pos的值就为其本身，如果pos为空，那么就从链表头强制扩展一个虚pos指针，这个宏定义是为了在list_for_each_entry_continue()中使用做准备的。
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * 这个函数得遍历可以不从链表的头开始遍历，可以从一个指定的pos节点遍历
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
 * 遍历的顺序相反。
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
 * 这个函数的遍历是从当前这个点开始遍历的。
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
 * 和list_for_each_entry_continue()遍历类似，这个带了safe是为了防止删除节点而造成断链的发生。
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
 * 从当前的节点开始遍历。
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

/** 
 * 从头到尾遍历
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 * head:记录保存数据的链表  pos:把list中的元素依取出  member:组成pos的元素
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/** 
 * 从尾到头遍历
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
 * 和list_for_each_entry的遍历类似，这个带了safe是为了防止删除节点而造成断链的发生
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
 * 和list_for_each_entry_safe类似，不过遍历的顺序刚好相反。
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member))	     
	     	
/*
 * 通过遍历释放链表成员,先将链表移出，再将分配的内存释放。
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

