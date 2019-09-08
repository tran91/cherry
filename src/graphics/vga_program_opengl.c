#ifdef BUILD_OPENGL

#include "vga_program.h"
#include "opengl.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "types/vector.h"

static id current_program = id_null;

struct vga_program
{
    unsigned glid;
    
    depth_opt depth;
    stencil_opt stencil;
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
}

void vga_program_draw_array(id pid, const draw_array_opt * const opt, ...)
{
    begin(pid);
    end(pid);

    
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

#endif