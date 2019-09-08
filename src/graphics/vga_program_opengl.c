#ifdef BUILD_OPENGL

#include "vga_program.h"
#include "opengl.h"
#include "types/buffer.h"
#include "types/cmath.h"

struct vga_program
{
    unsigned glid;
    
    struct {
        char enable;
        char mask;
        unsigned func;
    } depth;
};
make_type(vga_program);

static void init(struct vga_program *p, key k)
{
    p->glid = 0;
}

static void clear(struct vga_program *p)
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
    clear(raw);

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

void vga_program_begin(id pid)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_add_attribute(id pid, id aid, const char *name, const signed data_type, const signed size)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_int(id pid, int value, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_float(id pid, float value, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec2_scalar(id pid, float x, float y, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec3_scalar(id pid, float x, float y, float z, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec4_scalar(id pid, float x, float y, float z, float w, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_mat4_scalar(id pid, float m[16], const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec2(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec3(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_vec4(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_uniform_mat4(id pid, id vid, const char *name, const signed index)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

void vga_program_set_depth(id pid, const char enable, const char mask, const char func)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    raw->depth.enable = enable;
    raw->depth.mask = mask;
    raw->depth.func = func;
}

void vga_program_end(id pid)
{
    struct vga_program *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
}

#endif