#ifdef BUILD_OPENGL

#include "vga.h"
#include "image.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "types/vector.h"

#if OS == IOS
    #define GL_GLEXT_PROTOTYPES
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    //#define GL_MAX_SAMPLES GL_MAX_SAMPLES_APPLE
#elif OS == OSX
    #define __gl_h_
    #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
    #include <OpenGL/gl3.h>
    #include <OpenGL/glext.h>
#elif OS == DROID
    #define GL_GLEXT_PROTOTYPES
    #include <EGL/egl.h>
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
#elif OS == WEB
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#elif OS == WINDOWS
    #include <GL/glew.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

const unsigned VGA_FLOAT = GL_FLOAT;
const unsigned VGA_INT = GL_INT;

const unsigned VGA_ARRAY = GL_ARRAY_BUFFER;
const unsigned VGA_ELEMENT = GL_ELEMENT_ARRAY_BUFFER;

const unsigned VGA_TRIANGLES = GL_TRIANGLES;

const unsigned VGA_ALWAYS = GL_ALWAYS;
const unsigned VGA_NEVER = GL_NEVER;
const unsigned VGA_LESS = GL_LESS;
const unsigned VGA_EQUAL = GL_EQUAL;
const unsigned VGA_LEQUAL = GL_LEQUAL;
const unsigned VGA_GREATER = GL_GREATER;
const unsigned VGA_NOTEQUAL = GL_NOTEQUAL;
const unsigned VGA_GEQUAL = GL_GEQUAL;

const unsigned VGA_KEEP = GL_KEEP;
const unsigned VGA_REPLACE = GL_REPLACE;
const unsigned VGA_INCR = GL_INCR;
const unsigned VGA_INCR_WRAP = GL_INCR_WRAP;
const unsigned VGA_DECR = GL_DECR;
const unsigned VGA_DECR_WRAP = GL_DECR_WRAP;
const unsigned VGA_INVERT = GL_INVERT;

const unsigned VGA_ZERO = GL_ZERO;
const unsigned VGA_ONE = GL_ONE;
const unsigned VGA_SRC_COLOR = GL_SRC_COLOR;
const unsigned VGA_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR;
const unsigned VGA_DST_COLOR = GL_DST_COLOR;
const unsigned VGA_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR;
const unsigned VGA_SRC_ALPHA = GL_SRC_ALPHA;
const unsigned VGA_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
const unsigned VGA_DST_ALPHA = GL_DST_ALPHA;
const unsigned VGA_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;
const unsigned VGA_CONSTANT_COLOR = GL_CONSTANT_COLOR;
const unsigned VGA_ONE_MINUS_CONSTANT_COLOR = GL_ONE_MINUS_CONSTANT_COLOR;
const unsigned VGA_CONSTANT_ALPHA = GL_CONSTANT_ALPHA;
const unsigned VGA_ONE_MINUS_CONSTANT_ALPHA = GL_ONE_MINUS_CONSTANT_ALPHA;

const unsigned VGA_FRONT = GL_FRONT;
const unsigned VGA_BACK = GL_BACK;
const unsigned VGA_CCW = GL_CCW;
const unsigned VGA_CW = GL_CW;

const unsigned VGA_FALSE = GL_FALSE;
const unsigned VGA_TRUE = GL_TRUE;

/*
 * attribute
 */
struct vga_attribute
{
    unsigned glid;
    char created;
    unsigned target;
};
make_type_detail(vga_attribute);

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

void vga_attribute_fill(id pid, const void *buf, const unsigned size, const unsigned usage)
{
    struct vga_attribute *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    glBindBuffer(raw->target, raw->glid);
    glBufferData(raw->target, size, buf, usage);
    glBindBuffer(raw->target, 0);
}

void vga_attribute_replace(id pid, const unsigned offset, const void *buf, const unsigned size)
{
    struct vga_attribute *raw;

    fetch(pid, &raw);
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
    unsigned glid;
    char created;
    id map;
};
make_type_detail(vga_attribute_group);

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

void vga_attribute_group_add(id pid, id attr, const char *name, signed data_type, signed size, unsigned normalized, signed stride, unsigned offset)
{
    struct vga_attribute_group *raw;
    struct vga_attribute *a_raw;
    unsigned index;

    fetch(pid, &raw);
    fetch(attr, &a_raw);
    assert(raw != NULL);

    map_get_size(raw->map, &index);
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
    unsigned glid;
    unsigned width;
    unsigned height;
};
make_type_detail(vga_texture);

static void vga_texture_init(struct vga_texture *p, key k)
{
    p->glid = 0;
    p->width = 0;
    p->height = 0;
}

static void vga_texture_clear(struct vga_texture *p)
{
    if (p->glid > 0) {
        glDeleteTextures(1, &p->glid);
        p->glid = 0;
        p->width = 0;
        p->height = 0;
    }
}

void vga_texture_get_width(id pid, unsigned *width)
{
    struct vga_texture *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *width = raw->width;
}

void vga_texture_get_height(id pid, unsigned *height)
{
    struct vga_texture *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *height = raw->height;
}

void vga_texture_load_file(id pid, const char *path)
{
    struct vga_texture *raw;
    id img;
    unsigned channels;
    const unsigned char *ptr;

    fetch(pid, &raw);
    assert(raw != NULL);

    vga_texture_clear(raw);
    glGenTextures(1, &raw->glid);

    image_new(&img);
    image_load_file(img, path);
    image_get_size(img, &raw->width, &raw->height);
    image_get_ptr(img, &ptr);
    image_get_number_channels(img, &channels);

    glActiveTexture(GL_TEXTURE0);
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

/*
 * programs
 */

static id current_program = id_null;

struct vga_program
{
    unsigned glid;
    
    depth_opt depth;
    stencil_opt stencil;
    blend_opt blend;
    cull_opt cull;
};
make_type_detail(vga_program);

static void vga_program_init(struct vga_program *p, key k)
{
    p->glid = 0;
}

static void vga_program_clear(struct vga_program *p)
{
    if (p->glid > 0) {
        glDeleteProgram(p->glid);
        p->glid = 0;
    }
}

#define LITERAL(p) p, sizeof(p) - 1

static void get_complete_shader(id buf, const char *path)
{
    const char *ptr;

    ptr = (const char *)glGetString(GL_VERSION);
    if(strstr(ptr, "ES")) {
        buffer_append(buf, LITERAL("#version 300 es"));
    } else {
        buffer_append(buf, LITERAL("#version "));
        ptr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        while(*ptr) {
            if(*ptr != '.') {
                buffer_append(buf, ptr, 1);
            }
            ptr++;
        }
    }

    buffer_append(buf, LITERAL("\n"));
    buffer_append_file(buf, path);
}

void vga_program_load(id pid, const char *vert_path, const char *frag_path)
{
    struct vga_program *raw;
    id vbuf, fbuf;
    unsigned vs, fs, vf;
    const char *ptr;
    signed r;
    char info[512];

    fetch(pid, &raw);
    assert(raw != NULL);
    vga_program_clear(raw);

    buffer_new(&vbuf);
    buffer_new(&fbuf);

    get_complete_shader(vbuf, vert_path);
    get_complete_shader(fbuf, frag_path);

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
        fetch(pid, &raw);
        assert(raw != NULL);
    
        current_program = pid;
        glUseProgram(raw->glid);
    }
}

static void end(id pid)
{
    /* depth and stencil affects global state in opengl */
    static depth_opt current_depth;
    static stencil_opt current_stencil;
    static blend_opt current_blend;
    static cull_opt current_cull;

    struct vga_program *raw;

    fetch(pid, &raw);
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
            glBlendFunc(current_blend.sfactor, current_blend.dfactor);
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
}

void vga_program_draw_array(id pid, id group, unsigned mode, int first, int count)
{
    struct vga_attribute_group *vag;

    begin(pid);
    end(pid);

    fetch(group, &vag);
    assert(vag != NULL);
    
    glBindVertexArray(vag->glid);
    glDrawArrays(mode, first, count);
    glBindVertexArray(0);
}

void vga_program_set_uniform_int(id pid, int value, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform1i(i, value);
}

void vga_program_set_uniform_float(id pid, float value, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform1f(i, value);
}

void vga_program_set_uniform_vec2_scalar(id pid, float x, float y, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform2f(i, x, y);
}

void vga_program_set_uniform_vec3_scalar(id pid, float x, float y, float z, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform3f(i, x, y, z);
}

void vga_program_set_uniform_vec4_scalar(id pid, float x, float y, float z, float w, const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    i = glGetUniformLocation(raw->glid, name);
    glUniform4f(i, x, y, z, w);
}

void vga_program_set_uniform_mat4_scalar(id pid, float m[16], const char *name, const signed index)
{
    struct vga_program *raw;
    int i;

    begin(pid);

    fetch(pid, &raw);
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

    fetch(pid, &raw);
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

    fetch(pid, &raw);
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

    fetch(pid, &raw);
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

    fetch(pid, &raw);
    assert(raw != NULL);
    i = glGetUniformLocation(raw->glid, name);
    mat4_get(vid, m);
    glUniformMatrix4fv(i, 1, 0, m);
}

void vga_program_set_depth(id pid, const depth_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    raw->depth = opt;
}

void vga_program_set_stencil(id pid, const stencil_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    raw->stencil = opt;
}

void vga_program_set_blend(id pid, const blend_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    raw->blend = opt;
}

void vga_program_set_cull(id pid, const cull_opt opt)
{
    struct vga_program *raw;

    begin(pid);

    fetch(pid, &raw);
    assert(raw != NULL);

    raw->cull = opt;
}

#endif