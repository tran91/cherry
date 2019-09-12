#include "ecs.h"
#include "types/vector.h"

struct ecs_entity
{
    id components;
};
make_type(ecs_entity);

static void ecs_entity_init(struct ecs_entity *p, key k)
{
    map_new(&p->components);
}

static void ecs_entity_clear(struct ecs_entity *p)
{
    release(p->components);
}

struct ecs_context
{
    id entities;
};
make_type(ecs_context);

static void ecs_context_init(struct ecs_context *p, key k)
{
    vector_new(&p->entities);
}

static void ecs_context_clear(struct ecs_context *p)
{
    release(p->entities);
}

void ecs_context_get_component(id ctx, unsigned entity, signed type, id *cid)
{
    struct ecs_context *rctx;
    struct ecs_entity *re;
    id eid;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get(rctx->entities, entity, &eid);
    if (!id_validate(eid)) return;

    ecs_entity_fetch(eid, &re);
    map_get(re->components, key_mem(&type, sizeof(type)), cid);
}

void ecs_context_add_component(id ctx, unsigned entity, id cid)
{
    struct ecs_context *rctx;
    struct ecs_entity *re;
    id eid;
    signed tp;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get(rctx->entities, entity, &eid);
    if (!id_validate(eid)) return;

    ecs_entity_fetch(eid, &re);
    which(cid, &tp);
    map_set(re->components, key_mem(&tp, sizeof(tp)), cid);
}

void ecs_context_new_entity(id ctx, unsigned *entity)
{
    struct ecs_context *rctx;
    id eid;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get_size(rctx->entities, entity);

    ecs_entity_new(&eid);
    vector_push(rctx->entities, eid);
    release(eid);
}