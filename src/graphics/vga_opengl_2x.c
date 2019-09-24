#ifdef BUILD_OPENGL_2

#include "vga.h"
#include "image.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "types/vector.h"
#include "types/buffer.h"

#if OS == IOS
    #define GL_GLEXT_PROTOTYPES
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>

    #define glGenVertexArrays               glGenVertexArraysOES
    #define glBindVertexArray               glBindVertexArrayOES
    #define glDeleteVertexArrays            glDeleteVertexArraysOES

    #define glMapBuffer                     glMapBufferOES
    #define glUnmapBuffer                   glUnmapBufferOES
    #define glMapBufferRange                glMapBufferRangeEXT

    #define GL_MAP_WRITE_BIT                GL_MAP_WRITE_BIT_EXT
    #define GL_MAP_READ_BIT                 GL_MAP_READ_BIT_EXT
    #define GL_MAP_INVALIDATE_RANGE_BIT     GL_MAP_INVALIDATE_RANGE_BIT_EXT
    #define GL_MAP_INVALIDATE_BUFFER_BIT    GL_MAP_INVALIDATE_BUFFER_BIT_EXT
    #define GL_MAP_FLUSH_EXPLICIT_BIT       GL_MAP_FLUSH_EXPLICIT_BIT_EXT
    #define GL_MAP_UNSYNCHRONIZED_BIT       GL_MAP_UNSYNCHRONIZED_BIT_EXT

    #define glDrawArraysInstanced           glDrawArraysInstancedEXT
    #define glVertexAttribDivisor           glVertexAttribDivisorEXT

    #define glFramebufferTexture(a,b,c,d) glFramebufferTexture2D(a,b, GL_TEXTURE_2D, c,d)
#elif OS == OSX
    #define __gl_h_
    #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
    #include <OpenGL/gl3.h>
    #include <OpenGL/glext.h>
#elif OS == DROID
    #define GL_GLEXT_PROTOTYPES
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    #define glGenVertexArrays glGenVertexArraysOES
    #define glBindVertexArray glBindVertexArrayOES
    #define glDeleteVertexArrays glDeleteVertexArraysOES

    #define glMapBuffer glMapBufferOES
    #define glUnmapBuffer glUnmapBufferOES

    #define glFramebufferTexture(a,b,c,d) glFramebufferTexture2D(a,b, GL_TEXTURE_2D, c,d)
#elif OS == WEB
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>

    #define GL_RGBA8_OES GL_RGBA8
    #define GL_DEPTH24_STENCIL8_OES GL_DEPTH_COMPONENT16

    #define glFramebufferTexture(a,b,c,d) glFramebufferTexture2D(a,b, GL_TEXTURE_2D, c,d)

#elif OS == WINDOWS
    #include <GL/glew.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>

    #define GL_RGBA8_OES GL_RGBA8
    #define GL_DEPTH24_STENCIL8_OES GL_DEPTH24_STENCIL8
#endif

const unsigned int VGA_FLOAT = GL_FLOAT;
const unsigned int VGA_INT = GL_INT;

const unsigned int VGA_ARRAY = GL_ARRAY_BUFFER;
const unsigned int VGA_ELEMENT = GL_ELEMENT_ARRAY_BUFFER;

const unsigned int VGA_STATIC = GL_STATIC_DRAW;
const unsigned int VGA_DYNAMIC = GL_DYNAMIC_DRAW;
const unsigned int VGA_STREAM = GL_STREAM_DRAW;

const unsigned int VGA_TRIANGLES = GL_TRIANGLES;

const unsigned int VGA_ALWAYS = GL_ALWAYS;
const unsigned int VGA_NEVER = GL_NEVER;
const unsigned int VGA_LESS = GL_LESS;
const unsigned int VGA_EQUAL = GL_EQUAL;
const unsigned int VGA_LEQUAL = GL_LEQUAL;
const unsigned int VGA_GREATER = GL_GREATER;
const unsigned int VGA_NOTEQUAL = GL_NOTEQUAL;
const unsigned int VGA_GEQUAL = GL_GEQUAL;

const unsigned int VGA_KEEP = GL_KEEP;
const unsigned int VGA_REPLACE = GL_REPLACE;
const unsigned int VGA_INCR = GL_INCR;
const unsigned int VGA_INCR_WRAP = GL_INCR_WRAP;
const unsigned int VGA_DECR = GL_DECR;
const unsigned int VGA_DECR_WRAP = GL_DECR_WRAP;
const unsigned int VGA_INVERT = GL_INVERT;

const unsigned int VGA_ZERO = GL_ZERO;
const unsigned int VGA_ONE = GL_ONE;
const unsigned int VGA_SRC_COLOR = GL_SRC_COLOR;
const unsigned int VGA_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR;
const unsigned int VGA_DST_COLOR = GL_DST_COLOR;
const unsigned int VGA_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR;
const unsigned int VGA_SRC_ALPHA = GL_SRC_ALPHA;
const unsigned int VGA_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
const unsigned int VGA_DST_ALPHA = GL_DST_ALPHA;
const unsigned int VGA_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;
const unsigned int VGA_CONSTANT_COLOR = GL_CONSTANT_COLOR;
const unsigned int VGA_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR;
const unsigned int VGA_CONSTANT_ALPHA = GL_CONSTANT_ALPHA;
const unsigned int VGA_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA;

const unsigned int VGA_FRONT = GL_FRONT;
const unsigned int VGA_BACK = GL_BACK;
const unsigned int VGA_CCW = GL_CCW;
const unsigned int VGA_CW = GL_CW;

const unsigned int VGA_FALSE = GL_FALSE;
const unsigned int VGA_TRUE = GL_TRUE;

const unsigned int VGA_RGBA = GL_RGBA;
const unsigned int VGA_RGB = GL_RGB;
const unsigned int VGA_RED = GL_RED;

const unsigned int VGA_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE;
const unsigned int VGA_REPEAT = GL_REPEAT;

/*
 * attribute
 */
struct vga_attribute
{
    unsigned int glid;
    char created;
    unsigned int target;
};
make_type(vga_attribute);

static void vga_attribute_init(struct vga_attribute *p, key k)
{
    glGenBuffers(1, &p->glid);
    p->created = 1;
    p->target = GL_ARRAY_BUFFER;
}

static void vga_attribute_clear(struct vga_attribute *p)
{
    if (p->created) {
        glDeleteBuffers(1, &p->glid);
        p->created = 0;
    }
    p->target = GL_ARRAY_BUFFER;
}

void vga_attribute_fill(id pid, const void *buf, const unsigned int size, const unsigned int usage)
{
    struct vga_attribute *raw;

    vga_attribute_fetch(pid, &raw);
    assert(raw != NULL);

    glBindBuffer(raw->target, raw->glid);
    glBufferData(raw->target, size, buf, usage);
    glBindBuffer(raw->target, 0);
}

void vga_attribute_replace(id pid, const unsigned int offset, const void *buf, const unsigned int size)
{
    struct vga_attribute *raw;

    vga_attribute_fetch(pid, &raw);
    assert(raw != NULL);

    glBindBuffer(raw->target, raw->glid);
    glBufferSubData(raw->target, offset, size, buf);
    glBindBuffer(raw->target, 0);
}

/*
 * attribute_group
 */
struct vga_attribute_group
{
    unsigned int glid;
    char created;
    id map;
};
make_type(vga_attribute_group);

static void vga_attribute_group_init(struct vga_attribute_group *p, key k)
{
    glGenVertexArrays(1, &p->glid);
    p->created = 1;
    map_new(&p->map);
}

static void vga_attribute_group_clear(struct vga_attribute_group *p)
{
    if (p->created) {
        p->created = 0;
        glDeleteVertexArrays(1, &p->glid);
    }
    release(p->map);
}

void vga_attribute_group_set(id pid, id attr, const char *name, const unsigned int index, signed data_type, signed size, unsigned int normalized, signed stride, unsigned int offset)
{
    struct vga_attribute_group *raw;
    struct vga_attribute *a_raw;

    vga_attribute_group_fetch(pid, &raw);
    vga_attribute_fetch(attr, &a_raw);
    assert(raw != NULL);

    map_set(raw->map, key_chars(name), attr);

    glBindVertexArray(raw->glid);
    glBindBuffer(a_raw->target, a_raw->glid);
    glVertexAttribPointer(index, size, data_type, normalized, stride, (void*)((size_t)offset));
    glEnableVertexAttribArray(index);
    glBindVertexArray(0);
}


/*
 * textures
 */
struct vga_texture
{
    unsigned int glid;    
    unsigned int width;
    unsigned int height;
    char created;
};
make_type(vga_texture);

static void vga_texture_init(struct vga_texture *p, key k)
{
    glGenTextures(1, &p->glid);
    p->created = 1;
    p->width = 0;
    p->height = 0;
}

static void vga_texture_clear(struct vga_texture *p)
{
    if (p->created) {
        glDeleteTextures(1, &p->glid);
        p->glid = 0;
        p->width = 0;
        p->height = 0;
        p->created = 0;
    }
}

void vga_texture_get_width(id pid, unsigned int *width)
{
    struct vga_texture *raw;

    vga_texture_fetch(pid, &raw);
    assert(raw != NULL);

    *width = raw->width;
}

void vga_texture_get_height(id pid, unsigned int *height)
{
    struct vga_texture *raw;

    vga_texture_fetch(pid, &raw);
    assert(raw != NULL);

    *height = raw->height;
}

void vga_texture_load_file(id pid, const char *path)
{
    struct vga_texture *raw;
    id img;
    unsigned int channels;
    const unsigned char *ptr;

    vga_texture_fetch(pid, &raw);
    assert(raw != NULL);

    image_new(&img);
    image_load_file(img, path);
    image_get_size(img, &raw->width, &raw->height);
    image_get_ptr(img, &ptr);
    image_get_number_channels(img, &channels);
    
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, raw->glid);
    switch (channels) {
        case 4:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw->width, raw->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
            break;
        case 3:
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, raw->width, raw->height, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
            break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    release(img);
}

void vga_texture_set_wrap(id pid, unsigned int s, unsigned int t)
{
    struct vga_texture *raw;

    vga_texture_fetch(pid, &raw);
    assert(raw != NULL);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, raw->glid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void vga_texture_load_raw(id pid, unsigned int width, unsigned int height, unsigned int internal_format, unsigned int format, const void *ptr)
{
    struct vga_texture *raw;

    vga_texture_fetch(pid, &raw);
    assert(raw != NULL);

    raw->width = width;
    raw->height = height;

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, raw->glid);
    if (format == VGA_RGBA) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, raw->width, raw->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
    } else if (format == VGA_RGB) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, raw->width, raw->height, 0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
    }  else if (format == VGA_RED) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, raw->width, raw->height, 0, GL_RED, GL_UNSIGNED_BYTE, ptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/*
 * renderbuffer
 */
struct vga_renderbuffer
{
    unsigned int glid;
    unsigned int width;
    unsigned int height;
    char created;
};
make_type(vga_renderbuffer);

static void vga_renderbuffer_init(struct vga_renderbuffer *p, key k)
{
    glGenRenderbuffers(1, &p->glid);
    p->created = 1;
    p->width = 0;
    p->height = 0;
}

static void vga_renderbuffer_clear(struct vga_renderbuffer *p)
{
    if (p->created) {
        glDeleteRenderbuffers(1, &p->glid);
        p->created = 0;
        p->width = 0;
        p->height = 0;
    }
}

/*
 * framebuffer
 */
struct vga_framebuffer
{
    unsigned int width;
    unsigned int height;

    struct {
        unsigned int glid;
        unsigned int created;
        id tex;
    } resolver;

    struct {
        unsigned int glid;
        unsigned int created;
        id ren;
        unsigned int samples;
        unsigned int build_samples;
    } sampler;

    unsigned int has_tex;
    id depth_stencil;    
    char has_depth;
    char has_stencil;
    char depth_stencil_complete;
};
make_type(vga_framebuffer);

static void vga_framebuffer_init(struct vga_framebuffer *p, key k)
{
    p->resolver.glid = 0;
    p->resolver.created = 0;
    p->resolver.tex = id_null;

    p->sampler.glid = 0;
    p->sampler.created = 0;
    p->sampler.ren = id_null;
    p->sampler.samples = 0;
    p->sampler.build_samples = 0;
    
    p->has_tex = 0;

    p->width = 0;
    p->height = 0;
    p->depth_stencil = id_null;
    p->has_depth = 0;
    p->has_stencil = 0;
    p->depth_stencil_complete = 0;
}

static void vga_framebuffer_clear(struct vga_framebuffer *p)
{
    if (p->resolver.created == 1) {
        glDeleteFramebuffers(1, &p->resolver.glid);
        p->resolver.created = 0;
    }
    release(p->resolver.tex);
    p->resolver.tex = id_null;

    if (p->sampler.created == 1) {
        glDeleteFramebuffers(1, &p->sampler.glid);
        p->sampler.created = 0;
    }
    release(p->sampler.ren);
    p->sampler.ren = id_null;
    p->sampler.samples = 0;

    p->has_tex = 0;

    p->width = 0;
    p->height = 0;
    release(p->depth_stencil);
    p->has_depth = 0;
    p->has_stencil = 0;
    p->depth_stencil_complete = 0;
}

static void vga_framebuffer_build(struct vga_framebuffer *p)
{
    struct vga_renderbuffer *vrb;
    struct vga_texture *vtx;

    if (p->width == 0 || p->height == 0) return;

    /* build resolver framebuffer */
    if (p->resolver.created == 0) {
        glGenFramebuffers(1, &p->resolver.glid);
        p->resolver.created = 1;
    }

    /* build sampler framebuffer */
    if (p->sampler.samples != p->sampler.build_samples) {
        if (p->sampler.build_samples == 0) {
            p->sampler.samples = 0;
            if (p->sampler.created == 1) {
                glDeleteFramebuffers(1, &p->sampler.glid);
                p->sampler.created = 0;
            }
            release(p->sampler.ren);
            p->sampler.ren = id_null;
            p->sampler.samples = 0;
        } else {
            if (p->sampler.created == 0) {
                p->sampler.created = 1;
                glGenFramebuffers(1, &p->sampler.glid);
            }
            p->sampler.samples = p->sampler.build_samples;
        }
    }

    /* check resolver textures */
    if (p->has_tex) {
        if (id_validate(p->resolver.tex)) {
            vga_texture_fetch(p->resolver.tex, &vtx);
            if (vtx->width != p->width || vtx->height != p->height) {
                vga_texture_load_raw(p->resolver.tex, p->width, p->height, VGA_RGBA, VGA_RGBA, NULL);                        
            }
        } else {
            vga_texture_new(&p->resolver.tex);
            vga_texture_load_raw(p->resolver.tex, p->width, p->height, VGA_RGBA, VGA_RGBA, NULL);

            vga_texture_fetch(p->resolver.tex, &vtx);
            glBindFramebuffer(GL_FRAMEBUFFER, p->resolver.glid);
            glBindTexture(GL_TEXTURE_2D, vtx->glid);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vtx->glid, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    /* check sampler renderers */
    #if OS != WEB
    if (p->sampler.created > 0) {
        if (id_validate(p->sampler.ren)) {
            vga_renderbuffer_fetch(p->sampler.ren, &vrb);
            if (vrb->width != p->width || vrb->height != p->height) {                    
                vrb->width = p->width;
                vrb->height = p->height;
                glBindRenderbuffer(GL_RENDERBUFFER, vrb->glid);                    
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, p->sampler.samples, GL_RGBA8_OES, p->width, p->height);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);                          
            }
        } else {
            vga_renderbuffer_new(&p->sampler.ren);
            vga_renderbuffer_fetch(p->sampler.ren, &vrb);
            vrb->width = p->width;
            vrb->height = p->height;
            glBindRenderbuffer(GL_RENDERBUFFER, vrb->glid);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, p->sampler.samples, GL_RGBA8_OES, p->width, p->height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);  

            glBindFramebuffer(GL_FRAMEBUFFER, p->sampler.glid);
            glBindRenderbuffer(GL_RENDERBUFFER, vrb->glid);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, vrb->glid);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    #endif

    /* build depth */
    if (p->depth_stencil_complete) return;
    if (p->has_depth || p->has_stencil) {
        vga_renderbuffer_fetch(p->depth_stencil, &vrb);
        if (!vrb) {
            vga_renderbuffer_new(&p->depth_stencil);
            vga_renderbuffer_fetch(p->depth_stencil, &vrb);
        }
        if (p->sampler.created == 1) {
            #if OS != WEB
            /* detach resolver depth buffer */
            glBindFramebuffer(GL_FRAMEBUFFER, p->resolver.glid);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            /* attach sampler depth buffer */
            glBindFramebuffer(GL_FRAMEBUFFER, p->sampler.glid);
            glBindRenderbuffer(GL_RENDERBUFFER, vrb->glid);
            if (vrb->width != p->width || vrb->height != p->height) {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, p->sampler.samples, GL_DEPTH24_STENCIL8_OES, p->width, p->height);
                vrb->width = p->width;
                vrb->height = p->height;
            }            
            if(p->has_depth) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, vrb->glid);
            } else {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            }
            if(p->has_stencil) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, vrb->glid);
            } else {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            #endif
        } else {
            /* attach resolver depth buffer */
            glBindFramebuffer(GL_FRAMEBUFFER, p->resolver.glid);
            glBindRenderbuffer(GL_RENDERBUFFER, vrb->glid);
            if (vrb->width != p->width || vrb->height != p->height) {
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, p->width, p->height);
                vrb->width = p->width;
                vrb->height = p->height;
            }            
            if(p->has_depth) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, vrb->glid);
            } else {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            }
            if(p->has_stencil) {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, vrb->glid);
            } else {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    } else {
        release(p->depth_stencil);
        if (p->resolver.created == 1) {
            glBindFramebuffer(GL_FRAMEBUFFER, p->resolver.glid);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        if (p->sampler.created == 1) {
            glBindFramebuffer(GL_FRAMEBUFFER, p->sampler.glid);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,GL_RENDERBUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }
    p->depth_stencil_complete = 1;
}

void vga_framebuffer_set_size(id pid, unsigned int width, unsigned int height)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL && width > 0 && height > 0 && raw->resolver.created != 2);

    if (raw->width == width && raw->height == height) return;
    raw->width = width;
    raw->height = height;
    raw->depth_stencil_complete = 0;

    vga_framebuffer_build(raw);
}

void vga_framebuffer_set_depth(id pid, char depth, char stencil)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL && raw->resolver.created != 2);

    if (raw->has_depth == depth && raw->has_depth == stencil) return;
    raw->has_depth = depth;
    raw->has_stencil = stencil;
    raw->depth_stencil_complete = 0;

    vga_framebuffer_build(raw);
}

void vga_framebuffer_set_multisampling(id pid, unsigned char samples)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL && raw->resolver.created != 2);

#if OS != WEB
    raw->sampler.build_samples = samples;
    vga_framebuffer_build(raw);
#endif
}

void vga_framebuffer_add_texture(id pid, const char *name)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL && raw->resolver.created != 2);

    raw->has_tex = 1;
    vga_framebuffer_build(raw);
}

void vga_framebuffer_get_texture(id pid, const char *name, id *tex)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL && raw->resolver.created != 2);

    *tex = raw->resolver.tex;
}

void vga_framebuffer_begin(id pid)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->resolver.created == 0) return;

    if (raw->sampler.created) {
        glBindFramebuffer(GL_FRAMEBUFFER, raw->sampler.glid);
        glViewport(0, 0, raw->width, raw->height);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, raw->resolver.glid);
        glViewport(0, 0, raw->width, raw->height);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void vga_framebuffer_end(id pid)
{
    struct vga_framebuffer *raw;

    vga_framebuffer_fetch(pid, &raw);
    assert(raw != NULL);

    if (raw->resolver.created == 0) return;

    if (raw->sampler.created) {
    #if OS != WEB
#if OS == IOS
        glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, raw->sampler.glid);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, raw->resolver.glid);
        glResolveMultisampleFramebufferAPPLE();
        GLenum discards[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
        glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 2, discards);
#else
        glBindFramebuffer(GL_READ_FRAMEBUFFER, raw->sampler.glid);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, raw->resolver.glid);
        glBlitFramebuffer(0, 0,
                raw->width,
                raw->height, 0, 0,
                raw->width,
                raw->height,
                GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif
    #endif
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

/*
 * screen
 */
struct vga_screen
{
    unsigned int glid;
    unsigned int width;
    unsigned int height;
};
make_type(vga_screen);

static void vga_screen_init(struct vga_screen *p, key k)
{
    p->glid = 0;
    p->width = 0;
    p->height = 0;
}

static void vga_screen_clear(struct vga_screen *p)
{

}

void vga_screen_set_size(id pid, unsigned int width, unsigned int height)
{
    struct vga_screen *raw;

    vga_screen_fetch(pid, &raw);
    assert(raw != NULL);

    raw->width = width;
    raw->height = height;
}

void vga_screen_get_size(id pid, unsigned int *width, unsigned int *height)
{
    struct vga_screen *raw;

    vga_screen_fetch(pid, &raw);
    assert(raw != NULL);

    *width = raw->width;
    *height = raw->height;
}

void vga_screen_set_glid(id pid, unsigned int glid)
{
    struct vga_screen *raw;

    vga_screen_fetch(pid, &raw);
    assert(raw != NULL);

    raw->glid = glid;
}

static void vga_screen_get_glid(id pid, unsigned int *glid)
{
    struct vga_screen *raw;

    vga_screen_fetch(pid, &raw);
    assert(raw != NULL);

    *glid = raw->glid;
}

/*
 * screenbuffer
 */
struct vga_screenbuffer
{
    unsigned int glid;
    unsigned int width;
    unsigned int height;
    id scr;
};
make_type(vga_screenbuffer);

static void vga_screenbuffer_init(struct vga_screenbuffer *p, key k)
{
    p->glid = 0;
    p->scr = id_null;
}

static void vga_screenbuffer_clear(struct vga_screenbuffer *p)
{
    release(p->scr);
}

void vga_screenbuffer_begin(id pid)
{
    struct vga_screenbuffer *raw;

    vga_screenbuffer_fetch(pid, &raw);
    assert(raw != NULL);

    glBindFramebuffer(GL_FRAMEBUFFER, raw->glid);
    glViewport(0, 0, raw->width, raw->height);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void vga_screenbuffer_end(id pid)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void vga_screenbuffer_get_screen(id pid, id *sid)
{
    struct vga_screenbuffer *raw;

    vga_screenbuffer_fetch(pid, &raw);
    assert(raw != NULL);

    *sid = raw->scr;
}

void vga_screenbuffer_set_screen(id pid, id sid)
{
    struct vga_screenbuffer *raw;

    vga_screenbuffer_fetch(pid, &raw);
    assert(raw != NULL);

    retain(sid);
    release(raw->scr);
    raw->scr = sid;

    vga_screen_get_size(sid, &raw->width, &raw->height);
    vga_screen_get_glid(sid, &raw->glid);
}

/*
 * programs
 */

static id current_program = id_null;

struct vga_program
{
    unsigned int glid;
    
    depth_opt depth;
    stencil_opt stencil;
    blend_opt blend;
    cull_opt cull;
    id mtexs;
};
make_type(vga_program);

static void vga_program_init(struct vga_program *p, key k)
{
    memset(p, 0, sizeof(struct vga_program));
    p->glid = 0;
    map_new(&p->mtexs);
}

static void vga_program_clear(struct vga_program *p)
{
    if (p->glid > 0) {
        glDeleteProgram(p->glid);
        p->glid = 0;
    }
    release(p->mtexs);
}

#define LITERAL(p) p, sizeof(p) - 1

static void get_complete_shader(id buf, unsigned char has_precision, const char *path)
{
    int range[2], precision;

    buffer_append(buf, LITERAL("#version 100\n"));
    if (has_precision) {
        glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT, range, &precision);
        if (precision == 23) {
            buffer_append(buf, LITERAL("precision highp float;\n"));
        } else {
            buffer_append(buf, LITERAL("precision mediump float;\n"));
        }
    }
    buffer_append_file(buf, path);
}

void vga_program_load(id pid, const char *vert_path, const char *frag_path)
{
    struct vga_program *raw;
    id vbuf, fbuf;
    unsigned int vs, fs, vf;
    const char *ptr;
    signed r;
    char info[512];

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    vga_program_clear(raw);
    map_new(&raw->mtexs);

    buffer_new(&vbuf);
    buffer_new(&fbuf);

    get_complete_shader(vbuf, 0, vert_path);
    get_complete_shader(fbuf, 1, frag_path);

    vs = glCreateShader(GL_VERTEX_SHADER);
    buffer_get_ptr(vbuf, &ptr);
    glShaderSource(vs, 1, &ptr, 0);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &r);
    if(!r) {
        glGetShaderInfoLog(vs, 512, 0, info);
        debug("vertex shader error:\n%s\n", info);
        glDeleteShader(vs);
        goto finish;
    }

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    buffer_get_ptr(fbuf, &ptr);
    glShaderSource(fs, 1, &ptr, 0);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &r);
    if(!r) {
        glGetShaderInfoLog(fs, 512, 0, info);
        debug("fragment shader error:\n%s\n", info);
        glDeleteShader(vs);
        glDeleteShader(fs);
        goto finish;
    }

    vf = glCreateProgram();
    glAttachShader(vf, vs);
    glAttachShader(vf, fs);
    glLinkProgram(vf);
    glGetProgramiv(vf, GL_LINK_STATUS, &r);
    if(!r) {
        glGetProgramInfoLog(vf, 512, 0, info);
        debug("link shader error:\n%s\n", info);
        glDeleteShader(vs);
        glDeleteShader(fs);
        goto finish;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    raw->glid = vf;

finish:
    release(vbuf);
    release(fbuf);
}

static void begin(id pid)
{
    struct vga_program *raw;

    if (!id_equal(pid, current_program)) {
        vga_program_fetch(pid, &raw);
        assert(raw != NULL);
    
        current_program = pid;
        glUseProgram(raw->glid);
    }
}

static void end(id pid)
{
    /* depth, stencil, blend, cull and texture states are independent with program state */
    static depth_opt current_depth;
    static stencil_opt current_stencil;
    static blend_opt current_blend;
    static cull_opt current_cull;
    // static id current_tex[32] = {id_null};
    struct vga_texture *tex;
    key k;
    id obj;
    unsigned int index;

    struct vga_program *raw;

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    if (memcmp(&current_depth, &raw->depth, sizeof(depth_opt)) != 0) {
        current_depth = raw->depth;
        if (current_depth.enable) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(current_depth.mask);
            glDepthFunc(current_depth.func);
        } else {
            glDisable(GL_DEPTH_TEST);
        }
    }

    if (memcmp(&current_stencil, &raw->stencil, sizeof(stencil_opt)) != 0) {
        current_stencil = raw->stencil;
        if (current_stencil.enable) {
            glEnable(GL_STENCIL_TEST);
            glStencilOp(current_stencil.test.sfail, current_stencil.test.dpfail, current_stencil.test.dppass);
            glStencilFunc(current_stencil.func.func, current_stencil.func.ref, current_stencil.func.mask);       
            glStencilMask(current_stencil.mask);
        } else {
            glDisable(GL_STENCIL_TEST);
        }
    }

    if (memcmp(&current_blend, &raw->blend, sizeof(blend_opt)) != 0) {
        current_blend = raw->blend;
        if (current_blend.enable) {
            glEnable(GL_BLEND);
            #if OS == WEB
                glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            #else
                glBlendFunc(current_blend.sfactor, current_blend.dfactor);
            #endif
        } else {
            glDisable(GL_BLEND);
        }
    }

    if (memcmp(&current_cull, &raw->cull, sizeof(cull_opt)) != 0) {
        current_cull = raw->cull;
        if (current_cull.enable) {
            glEnable(GL_CULL_FACE);
            glCullFace(current_cull.func);
            glFrontFace(current_cull.mode);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    index = 0;
    map_iterate(raw->mtexs, index, &k, &obj);
    while (id_validate(obj)) {
        // if (!id_equal(current_tex[index], obj)) {
            // current_tex[index] = obj;
            vga_texture_fetch(obj, &tex);
            glActiveTexture(GL_TEXTURE0 + index);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tex->glid);
            glUniform1i(glGetUniformLocation(raw->glid, k.ptr), (int)index);
        // }
        index++;
        map_iterate(raw->mtexs, index, &k, &obj);
    }
}

void vga_program_draw_array(id pid, id group, unsigned int mode, int first, int count)
{
    struct vga_attribute_group *vag;

    begin(pid);
    end(pid);

    vga_attribute_group_fetch(group, &vag);
    assert(vag != NULL);
    
    glBindVertexArray(vag->glid);
    glDrawArrays(mode, first, count);
    glBindVertexArray(0);
}

void vga_program_set_texture(id pid, id tex, const char *name, const signed index)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    map_set(raw->mtexs, key_chars(name), tex);
}

void vga_program_set_uniform_int(id pid, int value, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform1i(i, value);
}

void vga_program_set_uniform_float(id pid, float value, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform1f(i, value);
}

void vga_program_set_uniform_float_array(id pid, float *value, unsigned int count, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform1fv(i, count, value);
}

void vga_program_set_uniform_vec2_scalar(id pid, float x, float y, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform2f(i, x, y);
}

void vga_program_set_uniform_vec3_scalar(id pid, float x, float y, float z, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform3f(i, x, y, z);
}

void vga_program_set_uniform_vec4_scalar(id pid, float x, float y, float z, float w, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform4f(i, x, y, z, w);
}


void vga_program_set_uniform_vec4_array(id pid, float *value, unsigned int count, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform4fv(i, count, value);
}

void vga_program_set_uniform_mat4_scalar(id pid, float m[16], const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniformMatrix4fv(i, 1, 0, m);
}

void vga_program_set_uniform_vec2(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;
    float x, y;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    i = glGetUniformLocation(raw->glid, name);
    vec2_get(vid, &x, &y);
    glUniform2f(i, x, y);
}

void vga_program_set_uniform_vec3(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;
    float x, y, z;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    i = glGetUniformLocation(raw->glid, name);
    vec3_get(vid, &x, &y, &z);
    glUniform3f(i, x, y, z);
}

void vga_program_set_uniform_vec4(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;
    float x, y, z, w;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    i = glGetUniformLocation(raw->glid, name);
    vec4_get(vid, &x, &y, &z, &w);
    glUniform4f(i, x, y, z, w);
}

void vga_program_set_uniform_mat4(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;
    float m[16];

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    i = glGetUniformLocation(raw->glid, name);
    mat4_get(vid, m);
    glUniformMatrix4fv(i, 1, 0, m);
}

void vga_program_bind_attribute_location(id pid, const char *name, const unsigned int index)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);
    glBindAttribLocation(raw->glid, index, name);
    glLinkProgram(raw->glid);
}

void vga_program_set_depth(id pid, const depth_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    raw->depth = opt;
}

void vga_program_set_stencil(id pid, const stencil_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    raw->stencil = opt;
}

void vga_program_set_blend(id pid, const blend_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    raw->blend = opt;
}

void vga_program_set_cull(id pid, const cull_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    vga_program_fetch(pid, &raw);
    assert(raw != NULL);

    raw->cull = opt;
}

void vga_get_max_texture_size(unsigned int *size)
{
    GLint v;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &v);
    *size = v;
}

void vga_get_max_vertex_uniform_vectors(unsigned int *size)
{
    GLint v;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &v);
    *size = v;
}

void vga_get_max_fragment_uniform_vectors(unsigned int *size)
{
    GLint v;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &v);
    *size = v;
}

#endif