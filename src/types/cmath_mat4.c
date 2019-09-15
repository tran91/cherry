#include "cmath.h"

static inline void __set_scalar(const unsigned n, float * restrict l, const float r)
{
    int i;
    for(i = 0; i < n; ++i) {
        l[i] = r;
    }
}

struct mat4
{
    union {
        struct { 
            float   
                m00, m01, m02, m03,
                m10, m11, m12, m13,
                m20, m21, m22, m23,
                m30, m31, m32, m33; 
        };
        float m[16];
        float mm[4][4];
    };
};
make_type(mat4);

static void mat4_init(struct mat4 *p, key k)
{
    __set_scalar(16, p->m, 0);
    p->m00 = 1;
    p->m11 = 1;
    p->m22 = 1;
    p->m33 = 1;
}

static void mat4_clear(struct mat4 *p)
{

}

void mat4_load_string(id pid, const char *buf)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    const char *ptr = buf;
    char * endptr;
    int i = 0;

next:
    if(!ptr || !*ptr || i == 16) goto end;

    switch (*ptr) {
    case '1': case '2': case '3':
    case '4': case '5': case '6':
    case '7': case '8': case '9':
    case '0': case '+': case '-':
        raw->m[i] = (float)strtod(ptr, &endptr);
        ptr = endptr;
        i++;
        goto next;
    default:
        ptr++;
        goto next;
    }

end:
    ;
}

void mat4_set_identity(id pid)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    __set_scalar(16, raw->m, 0);
    raw->m00 = 1;
    raw->m11 = 1;
    raw->m22 = 1;
    raw->m33 = 1;
}

void mat4_set_array(id pid, const float f[16])
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    memcpy(raw->m, f, sizeof(float[16]));
}

void mat4_set_mat4(id pid, id cid)
{
    struct mat4 *r1, *r2;

    mat4_fetch(pid, &r1);
    mat4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);

    memcpy(r1->m, r2->m, sizeof(float[16]));
}

void mat4_set_quaternion(id pid, id cid)
{
    struct mat4 *r1;
    float qx, qy, qz, qw;

    mat4_fetch(pid, &r1);
    assert(r1 != NULL);

    vec4_normalize(cid);
    vec4_get(cid, &qx, &qy, &qz, &qw);

    const float x = qx;
    const float y = qy;
    const float z = qz;
    const float w = qw;

    const float _2x = x + x;
    const float _2y = y + y;
    const float _2z = z + z;
    const float _2w = w + w;

    r1->m00 = 1.0f - _2y * y - _2z * z;
    r1->m01 = _2x * y + _2w * z;
    r1->m02 = _2x * z - _2w * y;
    r1->m03 = 0.0f;

    r1->m10 = _2x * y - _2w * z;
    r1->m11 = 1.0f - _2x * x - _2z * z;
    r1->m12 = _2y * z + _2w * x;
    r1->m13 = 0.0f;

    r1->m20 = _2x * z + _2w * y;
    r1->m21 = _2y * z - _2w * x;
    r1->m22 = 1.0f - _2x * x - _2y * y;
    r1->m23 = 0.0f;

    r1->m30 = 0.0f;
    r1->m31 = 0.0f;
    r1->m32 = 0.0f;
    r1->m33 = 1.0f;
}

void mat4_inverse(id pid)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    struct mat4 mb = *raw;
    float s0 = mb.m00 * mb.m11 - mb.m10 * mb.m01;
    float s1 = mb.m00 * mb.m12 - mb.m10 * mb.m02;
    float s2 = mb.m00 * mb.m13 - mb.m10 * mb.m03;
    float s3 = mb.m01 * mb.m12 - mb.m11 * mb.m02;
    float s4 = mb.m01 * mb.m13 - mb.m11 * mb.m03;
    float s5 = mb.m02 * mb.m13 - mb.m12 * mb.m03;

    float c5 = mb.m22 * mb.m33 - mb.m32 * mb.m23;
    float c4 = mb.m21 * mb.m33 - mb.m31 * mb.m23;
    float c3 = mb.m21 * mb.m32 - mb.m31 * mb.m22;
    float c2 = mb.m20 * mb.m33 - mb.m30 * mb.m23;
    float c1 = mb.m20 * mb.m32 - mb.m30 * mb.m22;
    float c0 = mb.m20 * mb.m31 - mb.m30 * mb.m21;
    float det = s0 * c5 - s1 * c4 + s2 * c3 +
            s3 * c2 - s4 * c1 + s5 * c0;

    if(det == 0.0f) return;

    __set_scalar(16, raw->m, 1.0f / det);

    raw->m00 *= (mb.m11 * c5 - mb.m12 * c4 + mb.m13 * c3);
    raw->m01 *= (-mb.m01 * c5 + mb.m02 * c4 - mb.m03 * c3);
    raw->m02 *= (mb.m31 * s5 - mb.m32 * s4 + mb.m33 * s3);
    raw->m03 *= (-mb.m21 * s5 + mb.m22 * s4 - mb.m23 * s3);

    raw->m10 *= (-mb.m10 * c5 + mb.m12 * c2 - mb.m13 * c1);
    raw->m11 *= (mb.m00 * c5 - mb.m02 * c2 + mb.m03 * c1);
    raw->m12 *= (-mb.m30 * s5 + mb.m32 * s2 - mb.m33 * s1);
    raw->m13 *= (mb.m20 * s5 - mb.m22 * s2 + mb.m23 * s1);

    raw->m20 *= (mb.m10 * c4 - mb.m11 * c2 + mb.m13 * c0);
    raw->m21 *= (-mb.m00 * c4 + mb.m01 * c2 - mb.m03 * c0);
    raw->m22 *= (mb.m30 * s4 - mb.m31 * s2 + mb.m33 * s0);
    raw->m23 *= (-mb.m20 * s4 + mb.m21 * s2 - mb.m23 * s0);

    raw->m30 *= (-mb.m10 * c3 + mb.m11 * c1 - mb.m12 * c0);
    raw->m31 *= (mb.m00 * c3 - mb.m01 * c1 + mb.m02 * c0);
    raw->m32 *= (-mb.m30 * s3 + mb.m31 * s1 - mb.m32 * s0);
    raw->m33 *= (mb.m20 * s3 - mb.m21 * s1 + mb.m22 * s0);
}

void mat4_transpose(id pid)
{
    struct mat4 *raw, tmp;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    tmp = *raw;

    raw->m[0] = tmp.m[0];
    raw->m[1] = tmp.m[4];
    raw->m[2] = tmp.m[8];
    raw->m[3] = tmp.m[12];

    raw->m[4] = tmp.m[1];
    raw->m[5] = tmp.m[5];
    raw->m[6] = tmp.m[9];
    raw->m[7] = tmp.m[13];

    raw->m[8] = tmp.m[2];
    raw->m[9] = tmp.m[6];
    raw->m[10] = tmp.m[10];
    raw->m[11] = tmp.m[14];

    raw->m[12] = tmp.m[3];
    raw->m[13] = tmp.m[7];
    raw->m[14] = tmp.m[11];
    raw->m[15] = tmp.m[15];
}

void mat4_set_perspective(id pid, float fov, float aspect, float near, float far)
{
    struct mat4 *raw;
    float cotan;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    cotan = 1.0f / tanf(fov / 2.0f);
    raw->m[0] = cotan / aspect;
    raw->m[1] = 0.0f;
    raw->m[2] = 0.0f;
    raw->m[3] = 0.0f;

    raw->m[4] = 0.0f;
    raw->m[5] = cotan;
    raw->m[6] = 0.0f;
    raw->m[7] = 0.0f;

    raw->m[8] = 0.0f;
    raw->m[9] = 0.0f;
    raw->m[10] = (far + near) / (near - far);
    raw->m[11] = -1.0f;

    raw->m[12] = 0.0f;
    raw->m[13] = 0.0f;
    raw->m[14] = (2.0f * far * near) / (near - far);
    raw->m[15] = 0.0f;
}

void mat4_set_orthographic(id pid, float left, float right, float bottom, float top, float near, float far)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = far + near;
    float fsn = far - near;

    raw->m00 = 2.0f / rsl;
    raw->m01 = 0.0f;
    raw->m02 = 0.0f;
    raw->m03 = 0.0f;

    raw->m10 = 0.0f;
    raw->m11 = 2.0f / tsb;
    raw->m12 = 0.0f;
    raw->m13 = 0.0f;

    raw->m20 = 0.0f;
    raw->m21 = 0.0f;
    raw->m22 = -2.0f / fsn;
    raw->m23 = 0.0f;

    raw->m30 = -ral / rsl;
    raw->m31 = -tab / tsb;
    raw->m32 = -fan / fsn;
    raw->m33 = 1.0f;
}
void mat4_set_look_at(id pid, float eye_x, float eye_y, float eye_z, float target_x, float target_y, float target_z, float up_x, float up_y, float up_z)
{
    id n, u, v;
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    vec3_new(&n);
    vec3_new(&u);
    vec3_new(&v);

    vec3_set(n, eye_x, eye_y, eye_z);
    vec3_sub(n, target_x, target_y, target_z);
    vec3_normalize(n);

    vec3_set(u, up_x, up_y, up_z);
    vec3_cross_vec3(u, n);
    vec3_normalize(u);

    vec3_set_vec3(v, n);
    vec3_cross_vec3(v, u);

    vec3_get_x(u, &raw->m00);
    vec3_get_x(v, &raw->m01);
    vec3_get_x(n, &raw->m02);
    raw->m03 = 0.0f;

    vec3_get_y(u, &raw->m10);
    vec3_get_y(v, &raw->m11);
    vec3_get_y(n, &raw->m12);
    raw->m13 = 0.0f;

    vec3_get_z(u, &raw->m20);
    vec3_get_z(v, &raw->m21);
    vec3_get_z(n, &raw->m22);
    raw->m23 = 0.0f;

    vec3_mul_scalar(u, -1);
    vec3_dot(u, eye_x, eye_y, eye_z, &raw->m30);
    vec3_mul_scalar(v, -1);
    vec3_dot(v, eye_x, eye_y, eye_z, &raw->m31);
    vec3_mul_scalar(n, -1);
    vec3_dot(n, eye_x, eye_y, eye_z, &raw->m32);
    raw->m33 = 1.0f;

    release(n);
    release(u);
    release(v);
}

void mat4_set_look_at_vec3(id pid, id eye, id target, id up)
{
    float e[3], t[3], u[3];
    vec3_get(eye, e, e + 1, e + 2);
    vec3_get(target, t, t + 1, t + 2);
    vec3_get(up, u, u + 1, u + 2);
    mat4_set_look_at(pid, e[0], e[1], e[2], t[0], t[1], t[2], u[0], u[1], u[2]);
}

void mat4_mul_mat4(id pid, id cid)
{
    struct mat4 *r1, *r2, tmp;

    mat4_fetch(pid, &r1);
    mat4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);

    tmp = *r1;

    r1->m[0]  = tmp.m[0] * r2->m[0]  + tmp.m[4] * r2->m[1]  + tmp.m[8] * r2->m[2]   + tmp.m[12] * r2->m[3];
    r1->m[4]  = tmp.m[0] * r2->m[4]  + tmp.m[4] * r2->m[5]  + tmp.m[8] * r2->m[6]   + tmp.m[12] * r2->m[7];
    r1->m[8]  = tmp.m[0] * r2->m[8]  + tmp.m[4] * r2->m[9]  + tmp.m[8] * r2->m[10]  + tmp.m[12] * r2->m[11];
    r1->m[12] = tmp.m[0] * r2->m[12] + tmp.m[4] * r2->m[13] + tmp.m[8] * r2->m[14]  + tmp.m[12] * r2->m[15];

    r1->m[1]  = tmp.m[1] * r2->m[0]  + tmp.m[5] * r2->m[1]  + tmp.m[9] * r2->m[2]   + tmp.m[13] * r2->m[3];
    r1->m[5]  = tmp.m[1] * r2->m[4]  + tmp.m[5] * r2->m[5]  + tmp.m[9] * r2->m[6]   + tmp.m[13] * r2->m[7];
    r1->m[9]  = tmp.m[1] * r2->m[8]  + tmp.m[5] * r2->m[9]  + tmp.m[9] * r2->m[10]  + tmp.m[13] * r2->m[11];
    r1->m[13] = tmp.m[1] * r2->m[12] + tmp.m[5] * r2->m[13] + tmp.m[9] * r2->m[14]  + tmp.m[13] * r2->m[15];

    r1->m[2]  = tmp.m[2] * r2->m[0]  + tmp.m[6] * r2->m[1]  + tmp.m[10] * r2->m[2]  + tmp.m[14] * r2->m[3];
    r1->m[6]  = tmp.m[2] * r2->m[4]  + tmp.m[6] * r2->m[5]  + tmp.m[10] * r2->m[6]  + tmp.m[14] * r2->m[7];
    r1->m[10] = tmp.m[2] * r2->m[8]  + tmp.m[6] * r2->m[9]  + tmp.m[10] * r2->m[10] + tmp.m[14] * r2->m[11];
    r1->m[14] = tmp.m[2] * r2->m[12] + tmp.m[6] * r2->m[13] + tmp.m[10] * r2->m[14] + tmp.m[14] * r2->m[15];

    r1->m[3]  = tmp.m[3] * r2->m[0]  + tmp.m[7] * r2->m[1]  + tmp.m[11] * r2->m[2]  + tmp.m[15] * r2->m[3];
    r1->m[7]  = tmp.m[3] * r2->m[4]  + tmp.m[7] * r2->m[5]  + tmp.m[11] * r2->m[6]  + tmp.m[15] * r2->m[7];
    r1->m[11] = tmp.m[3] * r2->m[8]  + tmp.m[7] * r2->m[9]  + tmp.m[11] * r2->m[10] + tmp.m[15] * r2->m[11];
    r1->m[15] = tmp.m[3] * r2->m[12] + tmp.m[7] * r2->m[13] + tmp.m[11] * r2->m[14] + tmp.m[15] * r2->m[15];
}

void mat4_mul_project_vec3(id pid, id cid)
{
    struct mat4 *raw;
    float x, y, z, w, x2, y2, z2, w2;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    vec3_get(cid, &x, &y, &z);
    w = 1.0;
    
    x2 = raw->m[0] * x + raw->m[4] * y + raw->m[8] * z + raw->m[12] * w;
    y2 = raw->m[1] * x + raw->m[5] * y + raw->m[9] * z + raw->m[13] * w;
    z2 = raw->m[2] * x + raw->m[6] * y + raw->m[10] * z + raw->m[14] * w;
    w2 = raw->m[3] * x + raw->m[7] * y + raw->m[11] * z + raw->m[15] * w;

    vec3_set(cid, x2 / w2, y2 / w2, z2 / w2);
}

void mat4_translate_vec3(id pid, id cid)
{
    float x, y, z;

    vec3_get(cid, &x, &y, &z);
    mat4_translate(pid, x, y, z);
}

void mat4_scale_vec3(id pid, id cid)
{
    float x, y, z;

    vec3_get(cid, &x, &y, &z);
    mat4_scale(pid, x, y, z);
}

void mat4_rotate_vec3(id pid, id cid, float radian)
{
    float x, y, z;

    vec3_get(cid, &x, &y, &z);
    mat4_rotate(pid, x, y, z, radian);
}

void mat4_translate(id pid, float x, float y, float z)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    raw->m[12] += raw->m[0] * x + raw->m[4] * y + raw->m[8] * z;
    raw->m[13] += raw->m[1] * x + raw->m[5] * y + raw->m[9] * z;
    raw->m[14] += raw->m[2] * x + raw->m[6] * y + raw->m[10] * z;
}

void mat4_scale(id pid, float x, float y, float z)
{
    struct mat4 *raw;
    int i;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    for (i = 0; i < 4; ++i) {
        raw->mm[0][i] *= x;
        raw->mm[1][i] *= y;
        raw->mm[2][i] *= z;
    }
}

void mat4_rotate(id pid, float x, float y, float z, float radian)
{
    struct mat4 *raw;
    float length;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);
    length = sqrtf(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;

    const float cos = cosf(radian);
    const float cosp = 1.0f - cos;
    const float sin = sinf(radian);

    raw->m00 = cos + cosp * x * x;
    raw->m01 = cosp * x * y + z * sin;
    raw->m02 = cosp * x * z - y * sin;
    raw->m03 = 0.0f;

    raw->m10 = cosp * x * y - z * sin;
    raw->m11 = cos + cosp * y * y;
    raw->m12 = cosp * y * z + x * sin;
    raw->m13 = 0.0f;

    raw->m20 = cosp * x * z + y * sin;
    raw->m21 = cosp * y * z - x * sin;
    raw->m22 = cos + cosp * z * z;
    raw->m23 = 0.0f;

    raw->m30 = 0.0f;
    raw->m31 = 0.0f;
    raw->m32 = 0.0f;
    raw->m33 = 1.0f;
}

void mat4_rotate_x(id pid, float radian)
{
    mat4_rotate(pid, 1, 0, 0, radian);
}

void mat4_rotate_y(id pid, float radian)
{
    mat4_rotate(pid, 0, 1, 0, radian);
}

void mat4_rotate_z(id pid, float radian)
{
    mat4_rotate(pid, 0, 0, 1, radian);
}

void mat4_get(id pid, float m[16])
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    memcpy(m, raw->m, sizeof(float[16]));
}

void mat4_log(id pid)
{
    struct mat4 *raw;

    mat4_fetch(pid, &raw);
    assert(raw != NULL);

    debug("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
        raw->m[0], raw->m[1], raw->m[2], raw->m[3],
        raw->m[4], raw->m[5], raw->m[6], raw->m[7],
        raw->m[8], raw->m[9], raw->m[10], raw->m[11],
        raw->m[12], raw->m[13], raw->m[14], raw->m[15]);
}
