#include "file.h"

#if OS == DROID
    #include <android/log.h>
    #include <zip/zip.h>
#endif

#if OS == IOS
    #import <Foundation/Foundation.h>
#endif

struct file
{
    void *ptr;
    unsigned type;
};
make_type(file);

enum {
    DESKTOP_FILE = 1,
    IOS_INTERAL,
    IOS_EXTERNAL,
    ANDROID_INTERNAL,
    ANDROID_EXTERNAL
};

static void clear(struct file *p)
{
    if (p->ptr) {
        #if OS == LINUX || OS == OSX
            switch (p->type) {
                case DESKTOP_FILE:
                    fclose(p->ptr);
                    break;
            }
        #endif

        #if OS == DROID
            switch (d->type) {
                case ANDROID_INTERNAL:
                    zip_fclose(d->ptr);
                    break;
                case ANDROID_EXTERNAL:
                    fclose(d->ptr);
                    break;
            }
        #endif

        p->ptr = NULL;
    }
}

static void check(const char *path)
{
    struct stat st = {0};
    char buf[256], *start;

    sprintf(buf, "%s", path);
    start = buf;
    while(*start) {
        if(*start == '/' && start > buf) {
            *start = '\0';
            if(stat(buf, &st) == -1) {
                mkdir(buf, 0700);
            }
            *start = '/';
        }

        start++;
    }
}

#if OS == OSX || OS == LINUX

static void path(const char *url, const char **p, const char **mode)
{
    if (strncmp(url, "inner://", sizeof("inner://") - 1) == 0) {
        *mode = "rb";
        *p = url + sizeof("inner://") - 1;
    } else if (strncmp(url, "local://", sizeof("local://") - 1) == 0) {
        *mode = "wb+";
        *p = url + sizeof("local://") - 1;
    } else {
        *p = NULL;
        *mode = NULL;
    }
}

static void init(struct file *p, key k)
{
    const char *pth;
    const char *mode;

    p->ptr = NULL;
    p->type = 0;

    if (k.type != KEY_LITERAL) return;

    path(k.ptr, &pth, &mode);
    if (!pth) return;

    if (strcmp(mode, "wb+") == 0) {
        check(pth);
    }

    p->ptr = fopen(pth, mode);
    p->type = DESKTOP_FILE;
}

void file_write(id pid, void *buf, unsigned len)
{
    struct file *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    fwrite(buf, 1, len, raw->ptr);
}

void file_read(id pid, char buf[8192], unsigned *read)
{
    struct file *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    *read = fread(buf, 1, 8192, raw->ptr);
}

void file_seek(id pid, unsigned offset)
{
    struct file *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    fseek(raw->ptr, offset, SEEK_SET);
}

#endif