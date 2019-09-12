#include "ecs.h"
#include "types/vector.h"
#include "types/buffer.h"

/*
 * entity internal
 */
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

/*
 * system_internal
 */
struct ecs_system
{
    id sid;
    void(*update)(id, id, float);
    void(*check)(id, id, unsigned);
};
make_type(ecs_system);

static void ecs_system_init(struct ecs_system *p, key k)
{
    p->sid = id_null;
    p->update = NULL;
    p->check = NULL;
}

static void ecs_system_clear(struct ecs_system *p)
{
    release(p->sid);
}

/*
 * context
 */
struct ecs_context
{
    id entities;
    id recycles;
    id systems;
};
make_type(ecs_context);

static void ecs_context_init(struct ecs_context *p, key k)
{
    vector_new(&p->entities);
    buffer_new(&p->recycles);
    map_new(&p->systems);
}

static void ecs_context_clear(struct ecs_context *p)
{
    release(p->entities);
    release(p->recycles);
    release(p->systems);
}

void ecs_context_get_system(id ctx, signed type, id *sid)
{
    struct ecs_context *rctx;
    struct ecs_system *rs;
    id s;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    map_get(rctx->systems, key_mem(&type, sizeof(type)), &s);
    ecs_system_fetch(s, &rs);
    if (rs) {
        *sid = rs->sid;
    } else {
        invalidate(sid);
    }
}

void ecs_context_add_system(id ctx, id sid, void(*update)(id, id, float), void(*check)(id, id, unsigned))
{
    struct ecs_context *rctx;
    signed tp;
    struct ecs_system *rs;
    id s;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    ecs_system_new(&s);
    ecs_system_fetch(s, &rs);
    assign(rs->sid, sid);
    rs->update = update;
    rs->check = check;

    which(sid, &tp);
    map_set(rctx->systems, key_mem(&tp, sizeof(tp)), s);
    release(s);
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

void ecs_context_check_entity(id ctx, unsigned entity)
{
    struct ecs_context *rctx;
    struct ecs_system *rs;
    unsigned index;
    id s;
    key k;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    index = 0;
    map_iterate(rctx->systems, index, &k, &s);
    while (id_validate(s)) {
        ecs_system_fetch(s, &rs);
        rs->check(ctx, rs->sid, entity);
        index++;
        map_iterate(rctx->systems, index, &k, &s);
    }
}

void ecs_context_new_entity(id ctx, unsigned *entity)
{
    struct ecs_context *rctx;
    id eid;
    unsigned len;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    buffer_get_length_with_stride(rctx->recycles, sizeof(unsigned), &len);
    if (len > 0) {
        /* find recycled entity */
        buffer_get_with_stride(rctx->recycles, sizeof(unsigned), len - 1, entity);
        buffer_cut_with_stride(rctx->recycles, sizeof(unsigned), len - 1);
        ecs_entity_new(&eid);
        vector_set(rctx->entities, *entity, eid);
        release(eid);
    } else {
        /* append new entity */
        vector_get_size(rctx->entities, entity);
        ecs_entity_new(&eid);
        vector_push(rctx->entities, eid);
        release(eid);
    }
}

void ecs_context_remove_entity(id ctx, unsigned entity)
{
    struct ecs_context *rctx;
    struct ecs_entity *re;
    id eid;
    unsigned len;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get_size(rctx->entities, &len);
    if (len <= entity) return;

    vector_get(rctx->entities, entity, &eid);
    if (id_validate(eid)) {
        ecs_entity_fetch(eid, &re);
        map_remove_all(re->components);
        ecs_context_check_entity(ctx, entity);
        vector_set(rctx->entities, entity, id_null);
        buffer_append(rctx->recycles, &entity, sizeof(entity));
    }
}

void ecs_context_update(id ctx, float delta)
{
    struct ecs_context *rctx;
    struct ecs_system *rs;
    unsigned index;
    id s;
    key k;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    index = 0;
    map_iterate(rctx->systems, index, &k, &s);
    while (id_validate(s)) {
        ecs_system_fetch(s, &rs);
        rs->update(ctx, rs->sid, delta);
        index++;
        map_iterate(rctx->systems, index, &k, &s);
    }
}