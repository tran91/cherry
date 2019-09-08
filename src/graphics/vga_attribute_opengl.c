#ifdef BUILD_OPENGL

#include "vga_attribute.h"
#include "opengl.h"

struct vga_attribute
{
    unsigned glid;
    char created;
};
make_type(vga_attribute);

static void init(struct vga_attribute *p, key k)
{
    glGenBuffers(1, &p->glid);
    p->created = 1;
}

static void clear(struct vga_attribute *p)
{
    if (p->created) {
        glDeleteBuffers(1, &p->glid);
        p->created = 0;
    }
}

#endif