#include "vector.h"

struct vector
{
    id *start;
    unsigned len;
};
make_type(vector);

static void vector_init(struct vector *p, key k)
{
    p->start = NULL;
    p->len = 0;
}

static void vector_clear(struct vector *p)
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
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    raw->start = realloc(raw->start, sizeof(id) * (raw->len + 1));
    raw->start[raw->len] = cid;
    raw->len++;
    retain(cid);
}

void vector_push_vector(id pid, id cid)
{
    struct vector *r1, *r2;
    int i;
    
    vector_fetch(pid, &r1);
    vector_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);

    if (r2->len == 0) return;

    r1->start = realloc(r1->start, sizeof(id) * (r1->len + r2->len));
    for (i = 0; i < r2->len; ++i) {
        r1->start[r1->len + i] = r2->start[i];
        retain(r2->start[i]);
    }
    r1->len += r2->len;
}

void vector_set(id pid, unsigned index, id cid)
{    
    struct vector *raw;
    id oid;
    int i;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    if (index >= raw->len) {
        raw->start = realloc(raw->start, sizeof(id) * (index + 1));
        for (i = raw->len; i <= index; ++i) {
            raw->start[i] = id_null;
        }
        raw->len = index + 1;
    }

    retain(cid);
    oid = raw->start[index];
    raw->start[index] = cid;
    release(oid);
}

void vector_get(id pid, unsigned index, id *cid)
{
    struct vector *raw;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= index) {
        invalidate(cid);
    } else {
        *cid = raw->start[index];
    }
}

void vector_remove_all(id pid)
{
    struct vector *raw;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);
    vector_clear(raw);
}

void vector_remove(id pid, unsigned index)
{
    struct vector *raw;
    id cid;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= index) return;

    cid = raw->start[index];
    if (index < raw->len - 1) {
        memmove(raw->start + index, raw->start + index + 1, sizeof(id) * (raw->len - index - 1));
    }
    raw->len--;
    release(cid);
}

void vector_remove_id(id pid, id obj)
{
    struct vector *raw;
    int i;
    id cid;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    for (i = 0; i < raw->len; ++i) {
        cid = raw->start[i];
        if (id_equal(obj, cid)) {
            if (i < raw->len - 1) {
                memmove(raw->start + i, raw->start + i + 1, sizeof(id) * (raw->len - i - 1));
            }
            raw->len--;
            i--;
            release(cid);
        }
    }
}

void vector_swap(id pid, unsigned idx1, unsigned idx2)
{
    struct vector *raw;
    id cid;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->len <= idx1 || raw->len <= idx2 || idx1 == idx2) return;

    cid = raw->start[idx1];
    raw->start[idx1] = raw->start[idx2];
    raw->start[idx2] = cid;
}

void vector_get_size(id pid, unsigned *s)
{
    struct vector *raw;
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    *s = raw->len;
}

void vector_bring_to_back(id pid, unsigned index)
{
    struct vector *raw;
    id cid;
    
    vector_fetch(pid, &raw);
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
    
    vector_fetch(pid, &raw);
    assert(raw != NULL);

    if (index == 0 || raw->len <= index) return;

    cid = raw->start[index];
    raw->start[index] = raw->start[0];
    raw->start[0] = cid;
}