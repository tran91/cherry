#ifndef __list_h
#define __list_h

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_HEAD_INIT(p) {&(p), &(p)}

static inline void list_head_init(struct list_head *list)
{
	list->prev = list;
	list->next = list;
}

static inline void __list_add(struct list_head *newp,
	struct list_head *prev,
	struct list_head *next)
{
	next->prev = newp;
	newp->next = next;
	newp->prev = prev;
	prev->next = newp;
}

static inline void list_add(struct list_head *newp,
	struct list_head * head)
{
	__list_add(newp, head, head->next);
}

static inline void list_add_tail(struct list_head *newp,
	struct list_head *head)
{
	__list_add(newp, head->prev, head);
}

static inline void __list_del(struct list_head *prev,
	struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *head)
{
	__list_del(head->prev, head->next);
}

static inline void list_del_init(struct list_head *head)
{
	__list_del(head->prev, head->next);
	list_head_init(head);
}

static inline int list_singular(struct list_head *head)
{
	return head == head->next;
}

#define list_for_each(p, list) \
	for(p = (list)->next; p != (list); p = p->next)

#define list_back_each(p, list) \
	for(p = (list)->prev; p != (list); p = p->prev)

#define list_while_not_singular(p, list) \
	while(!list_singular(list) && (p = (list)->next))

#define list_for_each_secure(head, list, work)                          \
	{                                                               \
		struct list_head __temp = LIST_HEAD_INIT(__temp);       \
		list_while_not_singular(head, list) {                   \
				list_del_init(head);                                 \
				list_add(head, &__temp);                   \
				work                                            \
		}                                                       \
		list_while_not_singular(head, &__temp) {                \
				list_del_init(head);                                 \
				list_add(head, list);                      \
		}                                                       \
	}

#endif
