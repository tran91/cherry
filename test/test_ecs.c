#include "ecs/ecs.h"
#include "types/cmath.h"

/* sample event */
struct event_play
{
    const char *name;
};
make_ecs_event(event_play);

static void event_play_init(struct event_play *p, key k)
{
    p->name = "listen to me";
}

static void event_play_clear(struct event_play *p)
{

}

/* sample component */
struct component_drawable
{
    const char *name;
};
make_ecs_component(component_drawable);

static void component_drawable_init(struct component_drawable *p, key k)
{
    p->name = "draw me";
}

static void component_drawable_clear(struct component_drawable *p)
{
}

/* sample system */
struct system_render
{
    id sym;
    id drawables;
};
make_ecs_system(system_render);

static void system_render_init(struct system_render *p, key k)
{
    empty_new(&p->sym);
    map_new(&p->drawables);
}

static void system_render_clear(struct system_render *p)
{
    release(p->sym);
    release(p->drawables);
}

static void system_render_check(id ctx, id sys, unsigned entity)
{
    id comp;
    id obj;
    struct system_render *rsys;

    system_render_fetch(sys, &rsys);
    assert(rsys != NULL);

    component_drawable_find(ctx, entity, &comp);
    if (id_validate(comp)) {
        /* accept entity */
        map_get(rsys->drawables, key_mem(&entity, sizeof(entity)), &obj);
        if (!id_validate(obj)) {
            map_set(rsys->drawables, key_mem(&entity, sizeof(entity)), rsys->sym);
        }
    } else {
        /* remove entity if existed */
        map_remove(rsys->drawables, key_mem(&entity, sizeof(entity)));
    }
}

static void system_render_listen(id ctx, id sys, unsigned signal)
{
    id evt;
    struct event_play *rep;
    struct system_render *rsys;

    system_render_fetch(sys, &rsys);
    assert(rsys != NULL);

    event_play_find(ctx, signal, &evt);
    if (id_validate(evt)) {
        event_play_fetch(evt, &rep);
        debug("%s: %u\n", rep->name, signal);
    }
}

static void system_render_update(id ctx, id sys, float delta)
{
    struct system_render *rsys;
    struct component_drawable *rcomp;
    key k;
    id obj, comp;
    unsigned index;
    unsigned entity;

    system_render_fetch(sys, &rsys);
    assert(rsys != NULL);

    index = 0;
    map_iterate(rsys->drawables, index, &k, &obj);
    while (id_validate(obj)) {
        entity = *(unsigned *)k.ptr;

        component_drawable_find(ctx, entity, &comp);
        component_drawable_fetch(comp, &rcomp);
        debug("%s: %u\n", rcomp->name, entity);

        index++;
        map_iterate(rsys->drawables, index, &k, &obj);
    }
}

int main(int argc, char **argv)
{
    id ctx;
    id cp, ev;
    unsigned e, s;
    int i;

    /* create context */
    ecs_context_new(&ctx);

    /* register systems */
    system_render_register(ctx);

    /* create new entity and assign some components */
    debug("\n---------------------------------------------\n");
    debug("             create 10 entities\n");
    debug("---------------------------------------------\n");
    for (i = 0; i < 10; ++i) {
        ecs_context_new_entity(ctx, &e);
        component_drawable_request(ctx, e, &cp);
    }
    ecs_context_update(ctx, 1.0f / 60);

    debug("\n---------------------------------------------\n");
    debug("          remove entity at index 4\n");
    debug("---------------------------------------------\n");
    ecs_context_remove_entity(ctx, 4);
    ecs_context_update(ctx, 1.0f / 60);

    debug("\n---------------------------------------------\n");
    debug("              create 1 entity\n");
    debug("---------------------------------------------\n");
    ecs_context_new_entity(ctx, &e);
    component_drawable_request(ctx, e, &cp);
    ecs_context_update(ctx, 1.0f / 60);

    /* send some events */
    debug("\n---------------------------------------------\n");
    debug("         add 1 signal with event_play\n");
    debug("---------------------------------------------\n");
    ecs_context_new_signal(ctx, &s);
    event_play_request(ctx, s, &ev);
    ecs_context_broadcast_signal(ctx, s);
    ecs_context_broadcast_signal(ctx, s);

    /* release context */
    release(ctx);
}