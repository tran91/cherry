#include "vector.h"

struct vector
{
    id *start;
    unsigned len;
};
make_type(vector);

static void init(struct vector *p, key k)
{
    p->start = NULL;
    p->len = 0;
}

static void clear(struct vector *p)
{
    id cid;

    if (p->start) {
        while (p->len > 0) {
            cid = p->start[p->len - 1];
            p->len--;
            release(cid);
        }
        free(p->start);
        p->start = NULL;
        p->len = 0;
    }
}

void vector_push(id pid, id cid)
{
    struct vector *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    raw->start = realloc(raw->start, sizeof(id) * (raw->len + 1));
    raw->start[raw->len] = cid;
    raw->len++;
    retain(cid);
}

void vector_get(id pid, unsigned index, id *cid)
{
    struct vector *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= index) {
        invalidate(cid);
    } else {
        *cid = raw->start[index];
    }
}

void vector_clear(id pid)
{
    struct vector *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);
    clear(raw);
}

void vector_remove(id pid, unsigned index)
{
    struct vector *raw;
    id cid;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= index) return;

    cid = raw->start[index];
    if (index < raw->len - 1) {
        memmove(raw->start + index, raw->start + index + 1, sizeof(id) * (raw->len - index - 1));
    }
    raw->len--;
    release(cid);
}

void vector_swap(id pid, unsigned idx1, unsigned idx2)
{
    struct vector *raw;
    id cid;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= idx1 || raw->len <= idx2 || idx1 == idx2) return;

    cid = raw->start[idx1];
    raw->start[idx1] = raw->start[idx2];
    raw->start[idx2] = cid;
}

void vector_get_size(id pid, unsigned *s)
{
    struct vector *raw;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    *s = raw->len;
}

void vector_bring_to_back(id pid, unsigned index)
{
    struct vector *raw;
    id cid;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= index || index == raw->len - 1) return;

    cid = raw->start[index];
    raw->start[index] = raw->start[raw->len - 1];
    raw->start[raw->len - 1] = cid;
}

void vector_bring_to_front(id pid, unsigned index)
{
    struct vector *raw;
    id cid;
    
    fetch(pid, &raw);
    assert(raw != NULL);

    if (index == 0 || raw->len <= index) return;

    cid = raw->start[index];
    raw->start[index] = raw->start[0];
    raw->start[0] = cid;
}