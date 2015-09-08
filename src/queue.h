#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef struct queue_hdr_t 
{
	struct queue_hdr_t * next;
	struct queue_hdr_t * prev;
} queue_hdr_t;

#define queue_init(ptr) ((ptr)->next = (ptr), (ptr)->prev = (ptr))
#define queue_entry(ptr, type, member) ((type*)( ((char*)((type*)ptr)) - ((size_t) &((type *)0)->member)))
#define queue_add_head(node, head) ((node)->prev = (head), (node)->next = (head)->next, (head)->next->prev = (node), (head)->next = (node))
#define queue_add_tail(node, head) ((node)->prev = (head)->prev, (node)->next = (head), (head)->prev->next = (node), (head)->prev = (node))
#define queue_del(entry) ((entry)->next->prev = (entry)->prev, (entry)->prev->next = (entry)->next, (entry)->next = 0, (entry)->prev = 0)
#define queue_is_empty(entry) ((entry) == (entry)->next)

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )

#endif
