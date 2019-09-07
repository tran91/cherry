#include "bind.h"

struct bind {
    void *ptr;
    void(*del)(void*);
};
make_type(bind);

static void init(struct bind *p, key k)
{
    p->ptr = NULL;
    p->del = NULL;
}

static void clear(struct bind *p)
{
    void *ptr;
    void(*del)(void *);
    ptr = p->ptr;
    del = p->del;
    if (ptr) {
        p->ptr = NULL;
        p->del = NULL;
        if (del) {
            del(ptr);
        }
        free(ptr);
    }
}

void bind_set(id pid, unsigned size, void(*pinit)(void*), void(*pclear)(void*))
{
    struct bind *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    clear(raw);
    raw->ptr = malloc(size);
    raw->del = pclear;
    if (pinit) {
        pinit(raw->ptr);
    }
}

void bind_set_raw(id pid, void *copy, unsigned size)
{
    struct bind *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    clear(raw);
    raw->ptr = malloc(size);
    memcpy(raw->ptr, copy, size);
}

void bind_get(id pid, void *d)
{
    void **ptr = d;
    struct bind *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *ptr = raw->ptr;
}