#include "ecs.h"
#include "types/vector.h"
#include "types/buffer.h"

/*
 * signal internal
 */
struct ecs_signal
{
    id events;
};
make_type(ecs_signal);

static void ecs_signal_init(struct ecs_signal *p, key k)
{
    map_new(&p->events);
}

static void ecs_signal_clear(struct ecs_signal *p)
{
    release(p->events);
}

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
    void(*listen)(id, id, unsigned);
};
make_type(ecs_system);

static void ecs_system_init(struct ecs_system *p, key k)
{
    p->sid = id_null;
    p->update = NULL;
    p->check = NULL;
    p->listen = NULL;
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
    struct {
        id indexes;
        id recycled;
    } entities;

    struct {
        id indexes;
        id recycled;
    } signals;
    
    id systems;
};
make_type(ecs_context);

static void ecs_context_init(struct ecs_context *p, key k)
{
    vector_new(&p->entities.indexes);
    buffer_new(&p->entities.recycled);
    vector_new(&p->signals.indexes);
    buffer_new(&p->signals.recycled);
    map_new(&p->systems);
}

static void ecs_context_clear(struct ecs_context *p)
{
    release(p->entities.indexes);
    release(p->entities.recycled);
    release(p->signals.indexes);
    release(p->signals.recycled);
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

void ecs_context_add_system(id ctx, id sid, void(*update)(id, id, float), void(*check)(id, id, unsigned), void(*listen)(id, id, unsigned))
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
    rs->listen = listen;

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

    vector_get(rctx->entities.indexes, entity, &eid);
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

    vector_get(rctx->entities.indexes, entity, &eid);
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

    buffer_get_length_with_stride(rctx->entities.recycled, sizeof(unsigned), &len);
    if (len > 0) {
        /* find recycled entity */
        buffer_get_with_stride(rctx->entities.recycled, sizeof(unsigned), len - 1, entity);
        buffer_cut_with_stride(rctx->entities.recycled, sizeof(unsigned), len - 1);
        ecs_entity_new(&eid);
        vector_set(rctx->entities.indexes, *entity, eid);
        release(eid);
    } else {
        /* append new entity */
        vector_get_size(rctx->entities.indexes, entity);
        ecs_entity_new(&eid);
        vector_push(rctx->entities.indexes, eid);
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

    vector_get_size(rctx->entities.indexes, &len);
    if (len <= entity) return;

    vector_get(rctx->entities.indexes, entity, &eid);
    if (id_validate(eid)) {
        ecs_entity_fetch(eid, &re);
        map_remove_all(re->components);
        ecs_context_check_entity(ctx, entity);
        vector_set(rctx->entities.indexes, entity, id_null);
        buffer_append(rctx->entities.recycled, &entity, sizeof(entity));
    }
}

void ecs_context_new_signal(id ctx, unsigned *signal)
{
    struct ecs_context *rctx;
    id sid;
    unsigned len;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    buffer_get_length_with_stride(rctx->signals.recycled, sizeof(unsigned), &len);
    if (len > 0) {
        /* find recycled signal */
        buffer_get_with_stride(rctx->signals.recycled, sizeof(unsigned), len - 1, signal);
        buffer_cut_with_stride(rctx->signals.recycled, sizeof(unsigned), len - 1);
        ecs_signal_new(&sid);
        vector_set(rctx->signals.indexes, *signal, sid);
        release(sid);
    } else {
        /* append new signal */
        vector_get_size(rctx->signals.indexes, signal);
        ecs_signal_new(&sid);
        vector_push(rctx->signals.indexes, sid);
        release(sid);
    }
}

void ecs_context_add_event(id ctx, unsigned signal, id eid)
{
    struct ecs_context *rctx;
    struct ecs_signal *rs;
    id sid;
    signed tp;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get(rctx->signals.indexes, signal, &sid);
    if (!id_validate(sid)) return;

    ecs_signal_fetch(sid, &rs);
    which(eid, &tp);
    map_set(rs->events, key_mem(&tp, sizeof(tp)), eid);
}

void ecs_context_get_event(id ctx, unsigned signal, signed type, id *eid)
{
    struct ecs_context *rctx;
    struct ecs_signal *rs;
    id sid;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    vector_get(rctx->signals.indexes, signal, &sid);
    if (!id_validate(sid)) return;

    ecs_signal_fetch(sid, &rs);
    map_get(rs->events, key_mem(&type, sizeof(type)), eid);
}

void ecs_context_broadcast_signal(id ctx, unsigned signal)
{
    struct ecs_context *rctx;
    struct ecs_system *rs;
    unsigned index, len;
    id s;
    key k;

    ecs_context_fetch(ctx, &rctx);
    assert(rctx != NULL);

    index = 0;
    map_iterate(rctx->systems, index, &k, &s);
    while (id_validate(s)) {
        ecs_system_fetch(s, &rs);
        rs->listen(ctx, rs->sid, signal);
        index++;
        map_iterate(rctx->systems, index, &k, &s);
    }

    vector_get_size(rctx->signals.indexes, &len);
    if (len <= signal) return;

    vector_get(rctx->signals.indexes, signal, &s);
    if (id_validate(s)) {
        vector_set(rctx->signals.indexes, signal, id_null);
        buffer_append(rctx->signals.recycled, &signal, sizeof(signal));
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