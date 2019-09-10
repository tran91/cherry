#include "buffer.h"
#include "file.h"

struct buffer
{
    unsigned char *ptr;
    unsigned len;
};
make_type(buffer);

static void load_file(struct buffer *p, const char *path)
{
    id fid;
    unsigned char buf[8192];
    unsigned count;

    file_new(&fid);
    file_open(fid, path);
    file_read(fid, buf, 8192, &count);
    while (count > 0) {
        p->ptr = realloc(p->ptr, p->len + count + 1);
        memcpy(p->ptr + p->len, buf, count);
        p->len += count;
        p->ptr[p->len] = '\0';
        file_read(fid, buf, 8192, &count);
    }
    release(fid);
}

static void buffer_init(struct buffer *p, key k)
{
    p->ptr = NULL;
    p->len = 0;

    if (k.type != KEY_LITERAL) return;

    if (strncmp(k.ptr, "inner://", sizeof("inner://") - 1) == 0
        || strncmp(k.ptr, "local://", sizeof("local://") - 1) == 0) {
        load_file(p, k.ptr);
    }
}

static void buffer_clear(struct buffer *p)
{
    if (p->ptr) {
        free(p->ptr);
        p->ptr = NULL;
        p->len = 0;
    }
}

void buffer_erase(id pid)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    raw->len = 0;
}

void buffer_append_file(id pid, const char *path)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);

    load_file(raw, path);
}

void buffer_append(id pid, const void *buf, const unsigned len)
{
    struct buffer *raw;

    if (len == 0 || !buf) return;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    raw->ptr = realloc(raw->ptr, raw->len + len + 1);
    memcpy(raw->ptr + raw->len, buf, len);
    raw->len += len;
    raw->ptr[raw->len] = '\0';
}

void buffer_append_buffer(id pid, id cid)
{
    struct buffer *r2;

    buffer_fetch(pid, &r2);
    assert(r2 != NULL);
    buffer_append(pid, r2->ptr, r2->len);
}

void buffer_get_length(id pid, unsigned *len)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    *len = raw->len;
}

void buffer_replace(id pid, const void *search, const unsigned slen, const void *replace, const unsigned rlen)
{
    struct buffer *raw;
    const char *s, *r;
    int i, j, c;

    buffer_fetch(pid, &raw);
    assert(raw != NULL && search != NULL && replace != NULL);

    if (!raw->ptr || slen == 0) return;

    i = j = c = 0;
    s = search;
    r = replace;
    while (i < raw->len) {
        if (raw->ptr[c + j] == s[j]) {
            j++;
            if (j == slen) {
                if (slen == rlen) {
                    memcpy(raw->ptr + c, r, rlen);                 
                } else if (slen < rlen) {
                    raw->ptr = realloc(raw->ptr, raw->len + (rlen - slen) + 1);                    
                    memmove(raw->ptr + c + rlen, raw->ptr + c + slen, raw->len - c - slen);                    
                    memcpy(raw->ptr + c, r, rlen);
                    raw->len += (rlen - slen);
                    raw->ptr[raw->len] = '\0';
                    i += (rlen - slen);
                } else {
                    memcpy(raw->ptr + c, r, rlen);
                    memcpy(raw->ptr + c + rlen, raw->ptr + c + slen, raw->len - c - slen);
                    raw->len -= (slen - rlen);
                    raw->ptr[raw->len] = '\0';
                    i -= (slen - rlen);
                }
                j = 0;
                c = i + 1;
            }
        } else {
            j = 0;
            c = i + 1;
        }
        ++i;
    }
}

void buffer_get_ptr(id pid, const void *ptr)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    *(void **)ptr = raw->ptr;
}