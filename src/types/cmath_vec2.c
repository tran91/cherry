#include "cmath.h"

struct vec2
{
    float x, y;
};
make_type(vec2);

static void vec2_init(struct vec2 *p, key k)
{
    p->x = p->y = 0;
}

static void vec2_clear(struct vec2 *p)
{

}

void vec2_normalize(id pid)
{
    struct vec2 *r1;
    float length;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);

    length = (float)sqrt(r1->x * r1->x + r1->y * r1->y);
    r1->x /= length;
    r1->y /= length;
}

void vec2_length(id pid, float *r)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);

    *r = (float)sqrt(r1->x * r1->x + r1->y * r1->y);
}

void vec2_distance(id pid, id cid, float *r)
{
    struct vec2 *r1, *r2;
    float x, y;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    x = r2->x - r1->x;
    y = r2->y - r1->y;
    *r = (float)sqrt(x * x + y * y);
}

void vec2_add_vec2(id pid, id cid)
{
    struct vec2 *r1, *r2;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x += r2->x;
    r1->y += r2->y;
}

void vec2_sub_vec2(id pid, id cid)
{
    struct vec2 *r1, *r2;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x -= r2->x;
    r1->y -= r2->y;
}

void vec2_mul_vec2(id pid, id cid)
{
    struct vec2 *r1, *r2;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x *= r2->x;
    r1->y *= r2->y;
}

void vec2_div_vec2(id pid, id cid)
{
    struct vec2 *r1, *r2;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x /= r2->x;
    r1->y /= r2->y;
}

void vec2_set_vec2(id pid, id cid)
{
    struct vec2 *r1, *r2;

    vec2_fetch(pid, &r1);
    vec2_fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->x = r2->x;
    r1->y = r2->y;
}

void vec2_add_scalar(id pid, float n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += n;
    r1->y += n;
}

void vec2_sub_scalar(id pid, float n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= n;
    r1->y -= n;
}

void vec2_mul_scalar(id pid, float n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= n;
    r1->y *= n;
}

void vec2_div_scalar(id pid, float n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= n;
    r1->y /= n;
}

void vec2_set_scalar(id pid, float n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = n;
    r1->y = n;
}

void vec2_add(id pid, float x, float y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x += x;
    r1->y += y;
}

void vec2_sub(id pid, float x, float y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x -= x;
    r1->y -= y;
}

void vec2_mul(id pid, float x, float y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x *= x;
    r1->y *= y;
}

void vec2_div(id pid, float x, float y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x /= x;
    r1->y /= y;
}

void vec2_set(id pid, float x, float y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    r1->x = x;
    r1->y = y;
}

void vec2_get(id pid, float *x, float *y)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    *x = r1->x;
    *y = r1->y;
}

void vec2_get_x(id pid, float *n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->x;
}

void vec2_get_y(id pid, float *n)
{
    struct vec2 *r1;

    vec2_fetch(pid, &r1);
    assert(r1 != NULL);
    *n = r1->y;
}