#include "ecs/ecs.h"
#include "types/cmath.h"

/* component transform */
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

/* system */
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
        debug("%s\n", rcomp->name);

        index++;
        map_iterate(rsys->drawables, index, &k, &obj);
    }
}


int main(int argc, char **argv)
{
    id ctx;
    id cp;
    unsigned e;
    int i;

    /* create context */
    ecs_context_new(&ctx);

    /* register systems */
    system_render_register(ctx);

    /* create new entity and assign some components */
    for (i = 0; i < 10; ++i) {
        ecs_context_new_entity(ctx, &e);
        component_drawable_request(ctx, e, &cp);
    }

    /* update context one time */
    ecs_context_update(ctx, 1.0f / 60);

    /* release context */
    release(ctx);
}