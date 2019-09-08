#include "image.h"

struct image
{
    unsigned char *ptr;
    unsigned width;
    unsigned height;
};
make_type(image);

static void init(struct image *p, key k)
{
    p->ptr = NULL;
    p->width = 0;
    p->height = 0;
}

static void clear(struct image *p)
{
    if (p->ptr) {
        free(p->ptr);
        p->ptr = NULL;
    }
}

void image_load_file(id pid, const char *path)
{

}

void image_get_size(id pid, unsigned *width, unsigned *height)
{
    struct image *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *width = raw->width;
    *height = raw->height;
}

void image_get_ptr(id pid, const unsigned char **ptr)
{
    struct image *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *ptr = raw->ptr;
}