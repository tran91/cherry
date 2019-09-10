#include "cmath.h"

struct vec3
{
    float x, y, z;
};
make_type(vec3);

static void vec3_init(struct vec3 *p, key k)
{
    p->x = p->y = p->z = 0;
}

static void vec3_clear(struct vec3 *p)
{
    
}

void vec3_cross_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2, tmp;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);

    tmp = *r1;
    r1->x = tmp.y * r2->z - tmp.z * r2->y;
    r1->y = tmp.z * r2->x - tmp.x * r2->z;
    r1->z = tmp.x * r2->y - tmp.y * r2->x;
}

void vec3_dot_vec3(id pid, id cid, float *r)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);

    *r = r1->x * r2->x + r1->y * r2->y + r1->z * r2->z;
}

void vec3_cross(id pid, float x, float y, float z)
{
    struct vec3 *r1, tmp;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);

    tmp = *r1;
    r1->x = tmp.y * z - tmp.z * y;
    r1->y = tmp.z * x - tmp.x * z;
    r1->z = tmp.x * y - tmp.y * x;
}

void vec3_dot(id pid, float x, float y, float z, float *r)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);

    *r = r1->x * x + r1->y * y + r1->z * z;
}

void vec3_normalize(id pid)
{
    struct vec3 *r1;
    float length;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);

    length = (float)sqrt(r1->x * r1->x + r1->y * r1->y + r1->z * r1->z);
    r1->x /= length;
    r1->y /= length;
    r1->z /= length;
}

void vec3_length(id pid, float *r)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);

    *r = (float)sqrt(r1->x * r1->x + r1->y * r1->y + r1->z * r1->z);
}

void vec3_distance(id pid, id cid, float *r)
{
    struct vec3 *r1, *r2;
    float x, y, z;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    x = r2->x - r1->x;
    y = r2->y - r1->y;
    z = r2->z - r1->z;
    *r = (float)sqrt(x * x + y * y + z * z);
}

void vec3_add_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x += r2->x;
    r1->y += r2->y;
    r1->z += r2->z;
}

void vec3_sub_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x -= r2->x;
    r1->y -= r2->y;
    r1->z -= r2->z;
}

void vec3_mul_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x *= r2->x;
    r1->y *= r2->y;
    r1->z *= r2->z;
}

void vec3_div_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x /= r2->x;
    r1->y /= r2->y;
    r1->z /= r2->z;
}

void vec3_set_vec3(id pid, id cid)
{
    struct vec3 *r1, *r2;

    vec3_fetch(pid, &r1);
    vec3_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x = r2->x;
    r1->y = r2->y;
    r1->z = r2->z;
}

void vec3_add_scalar(id pid, float n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += n;
    r1->y += n;
    r1->z += n;
}

void vec3_sub_scalar(id pid, float n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= n;
    r1->y -= n;
    r1->z -= n;
}

void vec3_mul_scalar(id pid, float n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= n;
    r1->y *= n;
    r1->z *= n;
}

void vec3_div_scalar(id pid, float n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= n;
    r1->y /= n;
    r1->z /= n;
}

void vec3_set_scalar(id pid, float n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = n;
    r1->y = n;
    r1->z = n;
}

void vec3_add(id pid, float x, float y, float z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += x;
    r1->y += y;
    r1->z += z;
}

void vec3_sub(id pid, float x, float y, float z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= x;
    r1->y -= y;
    r1->z -= z;
}

void vec3_mul(id pid, float x, float y, float z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= x;
    r1->y *= y;
    r1->z *= z;
}

void vec3_div(id pid, float x, float y, float z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= x;
    r1->y /= y;
    r1->z /= z;
}

void vec3_set(id pid, float x, float y, float z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = x;
    r1->y = y;
    r1->z = z;
}

void vec3_get(id pid, float *x, float *y, float *z)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    *x = r1->x;
    *y = r1->y;
    *z = r1->z;
}

void vec3_get_x(id pid, float *n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->x;
}

void vec3_get_y(id pid, float *n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->y;
}

void vec3_get_z(id pid, float *n)
{
    struct vec3 *r1;

    vec3_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->z;
}