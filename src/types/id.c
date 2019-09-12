#include "id.h"
#include <stdlib.h>

struct data
{
    int ref_count;
    unsigned type;
};

static struct {
    struct data **dt;
    unsigned *mask;
    unsigned len;
    unsigned using;
} __created__ = {NULL, NULL, 0, 0};

static struct {
    unsigned *id;
    unsigned len;
} __recycled__ = {NULL, 0};

struct type {
    void(*init)(void *, key);
    void(*clear)(void *);
    unsigned size;
};

static struct {
    struct type *info;
    unsigned len;
} __type__ = {
    .info = NULL,
    .len = 0
};

static volatile unsigned barrier = 0;

static void clean()
{
    free(__type__.info);
    __type__.len = 0;
    __type__.info = NULL;
}

void invalidate(id *pid)
{
    pid->mask = -1;
    pid->index = -1;
}

void require(void(*init)(void*, key), void(*clear)(void*), unsigned size, signed *type)
{
    if (*type >= 0) return;

    lock(&barrier);
    if (!__type__.info) {
        atexit(clean);
    }
    __type__.info = realloc(__type__.info, sizeof(struct type) * (__type__.len + 1));
    *type = __type__.len;
    __type__.info[__type__.len].size = size;
    __type__.info[__type__.len].init = init;
    __type__.info[__type__.len].clear = clear;
    __type__.len++;
    unlock(&barrier);
}

void create(unsigned type, id *pid)
{
    struct data *d;

    d = malloc(sizeof(struct data) + __type__.info[type].size);
    d->ref_count = 1;
    d->type = type;

    lock(&barrier);
    if (__recycled__.len == 0) {
        __created__.dt = realloc(__created__.dt, sizeof(struct data *) * (__created__.len + 1));
        __created__.mask = realloc(__created__.mask, sizeof(unsigned) * (__created__.len + 1));
        __created__.mask[__created__.len] = 0;
        __created__.dt[__created__.len] = d;
        pid->index = __created__.len;
        pid->mask = 0;
        __created__.len++;
    } else {
        pid->index = __recycled__.id[__recycled__.len - 1];
        pid->mask = __created__.mask[pid->index];
        __created__.dt[pid->index] = d;
        __recycled__.len--;
    }
    __created__.using++;
    unlock(&barrier);
}

void build(id pid, key k)
{
    struct data *d;
    d = __created__.dt[pid.index];
    __type__.info[d->type].init(d + 1, k);
}

void release(id pid)
{
    struct data *d;

    if (pid.index < 0 || pid.index >= __created__.len || pid.mask != __created__.mask[pid.index]) return;
    d = __created__.dt[pid.index];
    if (!d) return;
    if (__sync_sub_and_fetch((volatile signed *)&d->ref_count, 1) == 0) {
        lock(&barrier);
        __created__.dt[pid.index] = NULL;
        __created__.mask[pid.index]++;
        __created__.using--;
        __recycled__.id = realloc(__recycled__.id, sizeof(unsigned) * (__recycled__.len + 1));
        __recycled__.id[__recycled__.len] = pid.index;
        __recycled__.len++;
        unlock(&barrier);
        __type__.info[d->type].clear(d + 1);
        free(d);

        lock(&barrier);
        if (__created__.using == 0 && __created__.dt) {
            free(__created__.dt);
            free(__created__.mask);
            __created__.dt = NULL;
            __created__.mask = NULL;
            __created__.len = 0;
            if (__recycled__.id) {
                free(__recycled__.id);
                __recycled__.id = NULL;
                __recycled__.len = 0;
            }
        }
        unlock(&barrier);
    }
}

void retain(id pid)
{
    struct data *d;
    if (pid.index < 0 || pid.index >= __created__.len || pid.mask != __created__.mask[pid.index]) return;
    
    d = __created__.dt[pid.index];
    if (d) {
        __sync_add_and_fetch((volatile signed *)&d->ref_count, 1);
    }
}

void fetch(id pid, unsigned type, void *pt)
{
    void **p = pt;
    struct data *d;
    if (pid.index < 0 || pid.index >= __created__.len || pid.mask != __created__.mask[pid.index]) {
        *p = NULL;
        return;
    }
    
    d = __created__.dt[pid.index];
    if (!d) {
        *p = NULL;
    } else {
        assert(d->type == type); /* protect data type */
        *p = d + 1;
    }
}

void which(id pid, signed *type)
{
    struct data *d;
    if (pid.index < 0 || pid.index >= __created__.len || pid.mask != __created__.mask[pid.index]) {
        *type = -1;
        return;
    }
    d = __created__.dt[pid.index];
    if (!d) {
        *type = -1;
    } else {
        *type = d->type;
    }
}

/*
 * empty
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function" /* empty_fetch is going not to be called */
struct empty {};
make_type(empty);
static void empty_init(struct empty *p, key k){}
static void empty_clear(struct empty *p){}
#pragma GCC diagnostic pop

