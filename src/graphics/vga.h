#ifndef __vga_h
#define __vga_h

#include "types/id.h"

extern const unsigned VGA_FLOAT;
extern const unsigned VGA_INT;

extern const unsigned VGA_ARRAY;
extern const unsigned VGA_ELEMENT;

extern const unsigned VGA_STATIC;
extern const unsigned VGA_DYNAMIC;
extern const unsigned VGA_STREAM;

extern const unsigned VGA_TRIANGLES;

extern const unsigned VGA_ALWAYS;
extern const unsigned VGA_NEVER;
extern const unsigned VGA_LESS;
extern const unsigned VGA_EQUAL;
extern const unsigned VGA_LEQUAL;
extern const unsigned VGA_GREATER;
extern const unsigned VGA_NOTEQUAL;
extern const unsigned VGA_GEQUAL;

extern const unsigned VGA_KEEP;
extern const unsigned VGA_REPLACE;
extern const unsigned VGA_INCR;
extern const unsigned VGA_INCR_WRAP;
extern const unsigned VGA_DECR;
extern const unsigned VGA_DECR_WRAP;
extern const unsigned VGA_INVERT;

extern const unsigned VGA_ZERO;
extern const unsigned VGA_ONE;
extern const unsigned VGA_SRC_COLOR;
extern const unsigned VGA_ONE_MINUS_SRC_COLOR;
extern const unsigned VGA_DST_COLOR;
extern const unsigned VGA_ONE_MINUS_DST_COLOR;
extern const unsigned VGA_SRC_ALPHA;
extern const unsigned VGA_ONE_MINUS_SRC_ALPHA;
extern const unsigned VGA_DST_ALPHA;
extern const unsigned VGA_ONE_MINUS_DST_ALPHA;
extern const unsigned VGA_CONSTANT_COLOR;
extern const unsigned VGA_ONE_MINUS_CONSTANT_COLOR;
extern const unsigned VGA_CONSTANT_ALPHA;
extern const unsigned VGA_ONE_MINUS_CONSTANT_ALPHA;

extern const unsigned VGA_FRONT;
extern const unsigned VGA_BACK;
extern const unsigned VGA_CCW;
extern const unsigned VGA_CW;

extern const unsigned VGA_FALSE;
extern const unsigned VGA_TRUE;

extern const unsigned VGA_RGBA;
extern const unsigned VGA_RGB;

type(vga_attribute);
/*
 * @pid: vga_attribute
 */
void vga_attribute_fill(id pid, const void *buf, const unsigned size, const unsigned usage);

/*
 * @pid: vga_attribute
 */
void vga_attribute_replace(id pid, const unsigned offset, const void *buf, const unsigned size);


type(vga_attribute_group);
/*
 * @pid: vga_attribute_group
 * @attr: vga_attribute
 */
void vga_attribute_group_add(id pid, id attr, const char *name, signed data_type, signed size, unsigned normalized, signed stride, unsigned offset);


/*
 * texture
 */
type(vga_texture);
void vga_texture_get_width(id pid, unsigned *width);
void vga_texture_get_height(id pid, unsigned *height);
void vga_texture_load_file(id pid, const char *path);
void vga_texture_load_raw(id pid, unsigned width, unsigned height, unsigned internal_format, unsigned format, const void *ptr);

/*
 * framebuffer
 */
type(vga_framebuffer);
void vga_framebuffer_set_size(id pid, unsigned width, unsigned height);
void vga_framebuffer_set_depth(id pid, char depth, char stencil);
void vga_framebuffer_set_multisampling(id pid, unsigned char samples);
void vga_framebuffer_add_texture(id pid, const char *name);
void vga_framebuffer_get_texture(id pid, const char *name, id *tex);
void vga_framebuffer_begin(id pid);
void vga_framebuffer_end(id pid);

type(vga_screen);
void vga_screen_get_size(id pid, unsigned *width, unsigned *height);
void vga_screen_set_size(id pid, unsigned width, unsigned height);

type(vga_screenbuffer);
void vga_screenbuffer_begin(id pid);
void vga_screenbuffer_end(id pid);
void vga_screenbuffer_set_screen(id pid, id sid);

/*
 * program
 */
type(vga_program);

/*
 * @pid: vga_program
 */
void vga_program_load(id pid, const char *vert_path, const char *frag_path);

/*
 * @pid: vga_program
 * @group: vga_attribute_group
 */
void vga_program_draw_array(id pid, id group, unsigned mode, int first, int count);

/*
 * @pid: vga_program
 */
void vga_program_set_uniform_int(id pid, int value, const char *name, const signed index);
void vga_program_set_uniform_float(id pid, float value, const char *name, const signed index);
void vga_program_set_uniform_vec2_scalar(id pid, float x, float y, const char *name, const signed index);
void vga_program_set_uniform_vec3_scalar(id pid, float x, float y, float z, const char *name, const signed index);
void vga_program_set_uniform_vec4_scalar(id pid, float x, float y, float z, float w, const char *name, const signed index);
void vga_program_set_uniform_mat4_scalar(id pid, float m[16], const char *name, const signed index);
void vga_program_set_uniform_vec2(id pid, id vid, const char *name, const signed index);
void vga_program_set_uniform_vec3(id pid, id vid, const char *name, const signed index);
void vga_program_set_uniform_vec4(id pid, id vid, const char *name, const signed index);
void vga_program_set_uniform_mat4(id pid, id vid, const char *name, const signed index);

/*
 * @pid: vga_program
 */
typedef struct {
    char enable;
    char mask;
    unsigned func;
} depth_opt;
#define make_depth_opt(...) \
    (depth_opt){\
        .enable = 1,\
        .func = VGA_LESS,\
        .mask = VGA_TRUE,\
        __VA_ARGS__\
    }
void vga_program_set_depth(id pid, const depth_opt opt);

/*
 * @pid: vga_program
 */
typedef struct {
    char enable;
    struct {
        unsigned sfail;
        unsigned dpfail;
        unsigned dppass;
    } test;
    struct {
        unsigned func;
        signed ref;
        unsigned mask;
    } func;
    unsigned mask;
} stencil_opt;
#define make_stencil_opt(x) \
    (stencil_opt){\
        .enable = 1,\
        .test.sfail = VGA_KEEP,\
        .test.dpfail = VGA_KEEP,\
        .test.dppass = VGA_KEEP,\
        .func.func = VGA_ALWAYS,\
        .func.ref = 1,\
        .func.mask = 0xFF,\
        .mask = 0xFF,\
        x\
    }
void vga_program_set_stencil(id pid, const stencil_opt opt);

typedef struct {
    char enable;
    unsigned sfactor;
    unsigned dfactor;
} blend_opt;
#define make_blend_opt(x) \
    (blend_opt){\
        .enable = 1,\
        .sfactor = VGA_SRC_ALPHA,\
        .dfactor = VGA_ONE_MINUS_SRC_ALPHA,\
        x\
    }
void vga_program_set_blend(id pid, const blend_opt opt);

typedef struct {
    char enable;
    unsigned func;
    unsigned mode;
} cull_opt;
#define make_cull_opt(x) \
    (cull_opt){\
        .enable = 1,\
        .func = VGA_BACK,\
        .mode = VGA_CCW,\
        x\
    }
void vga_program_set_cull(id pid, const cull_opt opt);

#endif