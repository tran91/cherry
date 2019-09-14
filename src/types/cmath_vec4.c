#include "cmath.h"

struct vec4
{
    float x, y, z, w;
};
make_type(vec4);

static void vec4_init(struct vec4 *p, key k)
{
    p->x = p->y = p->z = p->w = 0;
}

static void vec4_clear(struct vec4 *p)
{
    
}

void vec4_set_quaternion(id pid, float ax, float ay, float az, float radian)
{
    struct vec4 *r1;

    float half = radian * 0.5f;
    float scale = sinf(half);

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);

    r1->x = ax * scale;
    r1->y = ay * scale;
    r1->z = az * scale;
    r1->w = cosf(half);
}

void vec4_normalize(id pid)
{
    struct vec4 *r1;
    float length;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);

    length = (float)sqrt(r1->x * r1->x + r1->y * r1->y + r1->z * r1->z + r1->w * r1->w);
    r1->x /= length;
    r1->y /= length;
    r1->z /= length;
    r1->w /= length;
}

void vec4_length(id pid, float *r)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);

    *r = (float)sqrt(r1->x * r1->x + r1->y * r1->y + r1->z * r1->z + r1->w * r1->w);
}

void vec4_distance(id pid, id cid, float *r)
{
    struct vec4 *r1, *r2;
    float x, y, z, w;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    x = r2->x - r1->x;
    y = r2->y - r1->y;
    z = r2->z - r1->z;
    w = r2->w - r1->w;
    *r = (float)sqrt(x * x + y * y + z * z + w * w);
}

void vec4_set_quaternion_identity(id pid)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = 0;
    r1->y = 0;
    r1->z = 0;
    r1->w = 1;
}

void vec4_add_vec4(id pid, id cid)
{
    struct vec4 *r1, *r2;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x += r2->x;
    r1->y += r2->y;
    r1->z += r2->z;
    r1->w += r2->w;
}

void vec4_sub_vec4(id pid, id cid)
{
    struct vec4 *r1, *r2;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x -= r2->x;
    r1->y -= r2->y;
    r1->z -= r2->z;
    r1->w -= r2->w;
}

void vec4_mul_vec4(id pid, id cid)
{
    struct vec4 *r1, *r2;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x *= r2->x;
    r1->y *= r2->y;
    r1->z *= r2->z;
    r1->w *= r2->w;
}

void vec4_div_vec4(id pid, id cid)
{
    struct vec4 *r1, *r2;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x /= r2->x;
    r1->y /= r2->y;
    r1->z /= r2->z;
    r1->w /= r2->w;
}

void vec4_set_vec4(id pid, id cid)
{
    struct vec4 *r1, *r2;

    vec4_fetch(pid, &r1);
    vec4_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x = r2->x;
    r1->y = r2->y;
    r1->z = r2->z;
    r1->w = r2->w;
}

void vec4_add_scalar(id pid, float n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += n;
    r1->y += n;
    r1->z += n;
    r1->w += n;
}

void vec4_sub_scalar(id pid, float n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= n;
    r1->y -= n;
    r1->z -= n;
    r1->w -= n;
}

void vec4_mul_scalar(id pid, float n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= n;
    r1->y *= n;
    r1->z *= n;
    r1->w *= n;
}

void vec4_div_scalar(id pid, float n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= n;
    r1->y /= n;
    r1->z /= n;
    r1->w /= n;
}

void vec4_set_scalar(id pid, float n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = n;
    r1->y = n;
    r1->z = n;
    r1->w = n;
}

void vec4_add(id pid, float x, float y, float z, float w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += x;
    r1->y += y;
    r1->z += z;
    r1->w += w;
}

void vec4_sub(id pid, float x, float y, float z, float w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= x;
    r1->y -= y;
    r1->z -= z;
    r1->w -= w;
}

void vec4_mul(id pid, float x, float y, float z, float w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= x;
    r1->y *= y;
    r1->z *= z;
    r1->w *= w;
}

void vec4_div(id pid, float x, float y, float z, float w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= x;
    r1->y /= y;
    r1->z /= z;
    r1->w /= w;
}

void vec4_set(id pid, float x, float y, float z, float w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = x;
    r1->y = y;
    r1->z = z;
    r1->w = w;
}

void vec4_get(id pid, float *x, float *y, float *z, float *w)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *x = r1->x;
    *y = r1->y;
    *z = r1->z;
    *w = r1->w;
}

void vec4_get_x(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->x;
}

void vec4_get_y(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->y;
}

void vec4_get_z(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->z;
}

void vec4_get_w(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->w;
}

void vec4_get_r(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->x;
}

void vec4_get_g(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->y;
}

void vec4_get_b(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->z;
}

void vec4_get_a(id pid, float *n)
{
    struct vec4 *r1;

    vec4_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->w;
}