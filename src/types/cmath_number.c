#include "cmath.h"

struct number
{
    double num;
};
make_type(number);

static void init(struct number *p, key k)
{
    p->num = 0;
}

static void clear(struct number *p)
{
}

void number_add_number(id pid, id cid)
{
    struct number *r1, *r2;

    fetch(pid, &r1);
    fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->num += r2->num;
}

void number_sub_number(id pid, id cid)
{
    struct number *r1, *r2;

    fetch(pid, &r1);
    fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->num -= r2->num;
}

void number_mul_number(id pid, id cid)
{
    struct number *r1, *r2;

    fetch(pid, &r1);
    fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->num *= r2->num;
}

void number_div_number(id pid, id cid)
{
    struct number *r1, *r2;

    fetch(pid, &r1);
    fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->num /= r2->num;
}

void number_set_number(id pid, id cid)
{
    struct number *r1, *r2;

    fetch(pid, &r1);
    fetch(cid, &r2);
    assert(r1 != NULL && r2 != NULL);
    r1->num = r2->num;
}

void number_add(id pid, double n)
{
    struct number *r1;

    fetch(pid, &r1);
    assert(r1 != NULL);
    r1->num += n;
}

void number_sub(id pid, double n)
{
    struct number *r1;

    fetch(pid, &r1);
    assert(r1 != NULL);
    r1->num -= n;
}

void number_mul(id pid, double n)
{
    struct number *r1;

    fetch(pid, &r1);
    assert(r1 != NULL);
    r1->num *= n;
}

void number_div(id pid, double n)
{
    struct number *r1;

    fetch(pid, &r1);
    assert(r1 != NULL);
    r1->num /= n;
}

void number_set(id pid, double n)
{
    struct number *r1;

    fetch(pid, &r1);
    assert(r1 != NULL);
    r1->num = n;
}