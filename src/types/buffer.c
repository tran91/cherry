#include "buffer.h"
#include "file.h"
#include "cmath.h"

struct buffer
{
    unsigned char *ptr;
    unsigned len;
    unsigned reserved;
};
make_type(buffer);

static inline void __realloc(struct buffer *p, const unsigned size)
{
    if (p->reserved < size) {
        p->reserved = size;
        p->ptr = realloc(p->ptr, p->reserved);
    }
}

static void load_file(struct buffer *p, const char *path)
{
    id fid;
    unsigned char buf[8192];
    unsigned count;

    file_new(&fid);
    file_open(fid, path);
    file_read(fid, buf, 8192, &count);
    while (count > 0) {
        __realloc(p, p->len + count + 1);
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
    p->reserved = 0;

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
        p->reserved = 0;
    }
}

void buffer_erase(id pid)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    raw->len = 0;
}

void buffer_reserve(id pid, unsigned len)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
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

    if (len == 0) return;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    __realloc(raw, raw->len + len + 1);
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

void buffer_append_float_string(id pid, const char *buf, float *max)
{
    struct buffer *raw;
    const char *ptr = buf;
    char * endptr;
    float f;

    if (max) *max = 0;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);

next:
    if(!ptr || !*ptr) goto end;

    switch (*ptr) {
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
    case '0': case '+': case '-':
        __realloc(raw, raw->len + sizeof(float));
        f = (float)strtod(ptr, &endptr);
        if (max) *max = MAX(f, *max);
        memcpy(raw->ptr + raw->len, &f, sizeof(float));
        raw->len += sizeof(float);
        ptr = endptr;
        goto next;
    default:
        ptr++;
        goto next;
    }

end:
    ;
}

void buffer_append_int_string(id pid, const char *buf, int *max)
{
    struct buffer *raw;
    const char *ptr = buf;
    char * endptr;
    int d;

    if (max) *max = 0;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);

next:
    if(!ptr || !*ptr) goto end;

    switch (*ptr) {
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
    case '0': case '+': case '-':
        __realloc(raw, raw->len + sizeof(int));
        d = strtol(ptr, &endptr, 10);
        if (max) *max = MAX(d, *max);
        memcpy(raw->ptr + raw->len, &d, sizeof(int));
        raw->len += sizeof(int);
        ptr = endptr;
        goto next;
    default:
        ptr++;
        goto next;
    }

end:
    ;
}

void buffer_get_length(id pid, unsigned *len)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    *len = raw->len;
}

void buffer_get_length_with_stride(id pid, unsigned stride, unsigned *len)
{
    struct buffer *raw;

    buffer_fetch(pid, &raw);
    assert(raw != NULL);
    *len = raw->len / stride;
}

void buffer_cut_with_stride(id pid, unsigned stride, unsigned index)
{
    struct buffer *raw;
    unsigned slen;

    buffer_fetch(pid, &raw);
    assert(raw != NULL && stride > 0);
    slen = raw->len / stride;

    if (slen <= index) return;    

    if (index < slen - 1) {
        memmove((char *)raw->ptr + index * stride, (char *)raw->ptr + (index + 1) * stride, stride * (slen - index - 1));
    }
    raw->len -= stride;
}

void buffer_get_with_stride(id pid, unsigned stride, unsigned index, void *mem)
{
    struct buffer *raw;
    unsigned slen;

    buffer_fetch(pid, &raw);
    assert(raw != NULL && stride > 0);
    slen = raw->len / stride;

    if (slen <= index) return;

    memcpy(mem, (char *)raw->ptr + index * stride, stride);
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
                    __realloc(raw, raw->len + (rlen - slen) + 1);                   
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