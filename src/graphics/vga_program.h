#ifndef __vga_program_h
#define __vga_program_h

#include "types/id.h"

type(vga_program);

enum {
    VGA_FLOAT,
    VGA_INT
};

/*
 * @pid: vga_program
 */
void vga_program_load(id pid, const char *vert_path, const char *frag_path);

typedef struct {
    id attr;
    const char *name;
    signed type;
    signed size;
} draw_array_opt;
void vga_program_draw_array(id pid, const draw_array_opt * const opt, ...);

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
void vga_program_set_stencil(id pid, const stencil_opt opt);

#endif