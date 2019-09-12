#include "ecs/ecs.h"
#include "types/cmath.h"

struct cp_transform
{
    id transform;
};
make_component(cp_transform);

static void cp_transform_init(struct cp_transform *p, key k)
{
    mat4_new(&p->transform);
}

static void cp_transform_clear(struct cp_transform *p)
{
    release(p->transform);
}

void cp_transform_get_transform(id pid, id t)
{
    struct cp_transform *rcpt;

    cp_transform_fetch(pid, &rcpt);
    assert(rcpt != NULL);

    mat4_set_mat4(t, rcpt->transform);
}

int main(int argc, char **argv)
{
    id ctx;
    id cp;
    unsigned e;

    ecs_context_new(&ctx);

    ecs_context_new_entity(ctx, &e);
    cp_transform_request(ctx, e, &cp);

    release(ctx);
}