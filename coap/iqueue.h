#ifndef __IQUEUE_H__
#define __IQUEUE_H__

typedef struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
} iqueue_head;

#define IQUEUE_INIT(ptr) ((ptr)->next = (ptr), (ptr)->prev = (ptr))
#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define ICONTAINEROF(ptr, type, member) ((type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)))
#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)
#define IQUEUE_ADD(node, head) ((node)->prev = (head), (node)->next = (head)->next, (head)->next->prev = (node), (head)->next = (node))
#define IQUEUE_ADD_TAIL(node, head) ((node)->prev = (head)->prev, (node)->next = (head), (head)->prev->next = (node), (head)->prev = (node))
#define IQUEUE_DEL(entry) ((entry)->next->prev = (entry)->prev, (entry)->prev->next = (entry)->next, (entry)->next = 0, (entry)->prev = 0)
#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )

#endif
