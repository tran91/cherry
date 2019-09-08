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

/*
 * @pid: vga_program
 */
void vga_program_begin(id pid);
/*
 * @pid: vga_program
 * @aid: vga_attribute
 */
void vga_program_add_attribute(id pid, id aid, const char *name, const signed data_type, const signed size);

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
void vga_program_set_depth(id pid, const char enable, const char mask, const char func);

/*
 * @pid: vga_program
 */
void vga_program_end(id pid);

#endif