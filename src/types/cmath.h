#ifndef __cmath_h
#define __cmath_h

#include "id.h"

type(number);
void number_add_number(id pid, id cid);
void number_sub_number(id pid, id cid);
void number_mul_number(id pid, id cid);
void number_div_number(id pid, id cid);
void number_set_number(id pid, id cid);
void number_add(id pid, double n);
void number_sub(id pid, double n);
void number_mul(id pid, double n);
void number_div(id pid, double n);
void number_set(id pid, double n);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
type(vec2);
void vec2_normalize(id pid);
void vec2_length(id pid, float *r);
void vec2_distance(id pid, id cid, float *r);
void vec2_add_vec2(id pid, id cid);
void vec2_sub_vec2(id pid, id cid);
void vec2_mul_vec2(id pid, id cid);
void vec2_div_vec2(id pid, id cid);
void vec2_set_vec2(id pid, id cid);
void vec2_add_scalar(id pid, float n);
void vec2_sub_scalar(id pid, float n);
void vec2_mul_scalar(id pid, float n);
void vec2_div_scalar(id pid, float n);
void vec2_set_scalar(id pid, float n);
void vec2_add(id pid, float x, float y);
void vec2_sub(id pid, float x, float y);
void vec2_mul(id pid, float x, float y);
void vec2_div(id pid, float x, float y);
void vec2_set(id pid, float x, float y);
void vec2_get(id pid, float *x, float *y);
void vec2_get_x(id pid, float *n);
void vec2_get_y(id pid, float *n);

type(vec3);
void vec3_cross_vec3(id pid, id cid);
void vec3_dot_vec3(id pid, id cid, float *r);
void vec3_cross(id pid, float x, float y, float z);
void vec3_dot(id pid, float x, float y, float z, float *r);
void vec3_normalize(id pid);
void vec3_length(id pid, float *r);
void vec3_distance(id pid, id cid, float *r);
void vec3_add_vec3(id pid, id cid);
void vec3_sub_vec3(id pid, id cid);
void vec3_mul_vec3(id pid, id cid);
void vec3_div_vec3(id pid, id cid);
void vec3_set_vec3(id pid, id cid);
void vec3_add_scalar(id pid, float n);
void vec3_sub_scalar(id pid, float n);
void vec3_mul_scalar(id pid, float n);
void vec3_div_scalar(id pid, float n);
void vec3_set_scalar(id pid, float n);
void vec3_add(id pid, float x, float y, float z);
void vec3_sub(id pid, float x, float y, float z);
void vec3_mul(id pid, float x, float y, float z);
void vec3_div(id pid, float x, float y, float z);
void vec3_set(id pid, float x, float y, float z);
void vec3_get(id pid, float *x, float *y, float *z);
void vec3_get_x(id pid, float *n);
void vec3_get_y(id pid, float *n);
void vec3_get_z(id pid, float *n);

type(vec4);
void vec4_normalize(id pid);
void vec4_length(id pid, float *r);
void vec4_distance(id pid, id cid, float *r);
void vec4_set_quaternion_identity(id pid);
void vec4_add_vec4(id pid, id cid);
void vec4_sub_vec4(id pid, id cid);
void vec4_mul_vec4(id pid, id cid);
void vec4_div_vec4(id pid, id cid);
void vec4_set_vec4(id pid, id cid);
void vec4_add_scalar(id pid, float n);
void vec4_sub_scalar(id pid, float n);
void vec4_mul_scalar(id pid, float n);
void vec4_div_scalar(id pid, float n);
void vec4_set_scalar(id pid, float n);
void vec4_set(id pid, float x, float y, float z, float w);
void vec4_add(id pid, float x, float y, float z, float w);
void vec4_sub(id pid, float x, float y, float z, float w);
void vec4_mul(id pid, float x, float y, float z, float w);
void vec4_div(id pid, float x, float y, float z, float w);
void vec4_get(id pid, float *x, float *y, float *z, float *w);
void vec4_get_x(id pid, float *n);
void vec4_get_y(id pid, float *n);
void vec4_get_z(id pid, float *n);
void vec4_get_w(id pid, float *n);
void vec4_get_r(id pid, float *n);
void vec4_get_g(id pid, float *n);
void vec4_get_b(id pid, float *n);
void vec4_get_a(id pid, float *n);

type(mat4);
void mat4_load_string(id pid, const char *ptr);
void mat4_set_identity(id pid);
void mat4_set_array(id pid, const float f[16]);
void mat4_set_mat4(id pid, id cid);
void mat4_inverse(id pid);
void mat4_transpose(id pid);
void mat4_set_perspective(id pid, float fov, float aspect, float near, float far);
void mat4_set_orthographic(id pid, float left, float right, float bottom, float top, float near, float far);
void mat4_set_look_at(id pid, float eye_x, float eye_y, float eye_z, float target_x, float target_y, float target_z, float up_x, float up_y, float up_z);
void mat4_set_look_at_vec3(id pid, id eye, id target, id up);
void mat4_mul_mat4(id pid, id cid);
void mat4_mul_project_vec3(id pid, id cid);
void mat4_translate_vec3(id pid, id cid);
void mat4_scale_vec3(id pid, id cid);
void mat4_rotate_vec3(id pid, id cid, float radian);
void mat4_translate(id pid, float x, float y, float z);
void mat4_scale(id pid, float x, float y, float z);
void mat4_rotate(id pid, float x, float y, float z, float radian);
void mat4_rotate_x(id pid, float radian);
void mat4_rotate_y(id pid, float radian);
void mat4_rotate_z(id pid, float radian);
void mat4_get(id pid, float m[16]);

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(X) ((X) * M_PI / 180)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG(X) ((X) * 180 / M_PI)
#endif

#ifndef EPSILON
#define EPSILON 0.0000000016
#endif

static inline float rand_rf(float min, float max)
{
        return ((max - min) * MIN(0.999f,((float)rand()/(float)(RAND_MAX)))) + min;
}

static inline int rand_ri(int min, int max)
{
        return (int)((max - min) * MIN(0.999f,((float)rand() / (float)RAND_MAX))) + min;
}

#endif