# Cherry

Cherry is a programming framework written in C

## Model

```C
/*
 * all interfaces should work with id rather than raw pointer
 * to get away from pointer dangling problem
 *
 * for more detail: src/types/id.h, src/types/id.c
 */
typedef struct id {
    signed mask; /* indicate whether id is living or not */
    signed index; /* point to the memory held by id */
} id;
/*
 * every id is created with counter equal 1
 *
 * retain/release is thread-safe
 */
void retain(id pid); /* increase memory counter held by id by 1 */
void release(id pid); /* decrease memory counter held by id by 1 */
void fetch(id pid, void *ptr); /* get memory held by id */
```

## EXAMPLE
```C
/*
 * your_data.h
 */
#include "types/id.h"

type(your_data);
void your_data_get_ptr(id pid, const char **ptr);
void your_data_append(id pid, const char *buf, const unsigned len);
```

```C
/*
 * your_data.c
 */
#include "your_data.h"

struct your_data {
    char *ptr;
    unsigned len;
};
make_type(your_data);

static void init(struct your_data *p, key k)
{
    p->ptr = NULL;
    p->len = 0;
}

static void clear(struct your_data *p)
{
    if (p->ptr) {
        free(p->ptr);
        p->ptr = NULL;
        p->len = 0;
    }
}

void your_data_get_ptr(id pid, const char **ptr)
{
    struct your_data *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);
    
    *ptr = raw->ptr;
}

void your_data_append(id pid, const char *buf, const unsigned len)
{
    struct your_data *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);
    
    raw->ptr = realloc(raw->ptr, raw->len + len + 1);
    memcpy(raw->ptr + raw->len, buf, len);
    raw->len += len;
    raw->ptr[raw->len] = '\0';
}
```