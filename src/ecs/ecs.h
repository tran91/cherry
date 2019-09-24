#ifndef __ecs_h
#define __ecs_h

#include "types/id.h"

type(ecs_context);
void ecs_context_new_entity(id ctx, unsigned int *entity);
void ecs_context_update(id ctx, float delta);
void ecs_context_remove_entity(id ctx, unsigned int entity);
void ecs_context_check_entity(id ctx, unsigned int entity);

void ecs_context_new_signal(id ctx, unsigned int *signal);
void ecs_context_broadcast_signal(id ctx, unsigned int signal);

/*
 * event
 */
#define ecs_event(name) \
    struct name;\
    void name##_find(id ctx, unsigned int signal, id *pid);\
    void name##_request(id ctx, unsigned int signal, id *pid);

#define make_ecs_event(name) \
static signed name##_type = -1;\
struct name;\
static void name##_init(struct name *s, key k);\
static void name##_clear(struct name *s);\
static void name##_fetch(id pid, struct name **ptr)\
{\
    void fetch(id pid, unsigned int type, void *p);\
    fetch(pid, (unsigned int)name##_type, ptr);\
}\
void name##_find(id ctx, unsigned int signal, id *pid)\
{\
    void ecs_context_get_event(id ctx, unsigned int signal, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_event(ctx, signal, name##_type, pid);\
}\
void name##_request(id ctx, unsigned int signal, id *pid)\
{\
    void ecs_context_add_event(id ctx, unsigned int signal, id cid);\
    void ecs_context_get_event(id ctx, unsigned int signal, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_event(ctx, signal, name##_type, pid);\
    if (!id_validate(*pid)) {\
        create(name##_type, pid);\
        build(*pid, key_null);\
        ecs_context_add_event(ctx, signal, *pid);\
        release(*pid);\
    }\
}


/*
 * system
 */
#define ecs_system(name) \
    void name##_register(id ctx);

#define make_ecs_system(name) \
    static signed name##_type = -1;\
    struct name;\
    static void name##_init(struct name *s, key k);\
    static void name##_clear(struct name *s);\
    static void name##_update(id ctx, id sys, float delta);\
    static void name##_check(id ctx, id sys, unsigned int entity);\
    static void name##_listen(id ctx, id sys, unsigned int signal);\
    static void name##_fetch(id pid, struct name **ptr)\
    {\
        void fetch(id pid, unsigned int type, void *p);\
        fetch(pid, (unsigned int)name##_type, ptr);\
    }\
    void name##_register(id ctx)\
    {\
        void ecs_context_add_system(id ctx, id sid, void(*update)(id, id, float), void(*check)(id, id, unsigned int), void(*listen)(id, id, unsigned int));\
        void ecs_context_get_system(id ctx, signed type, id *sid);\
        require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
        id sid;\
        ecs_context_get_system(ctx, name##_type, &sid);\
        if (!id_validate(sid)) {\
            create(name##_type, &sid);\
            build(sid, key_null);\
            ecs_context_add_system(ctx, sid, name##_update, name##_check, name##_listen);\
            release(sid);\
        }\
    }

/*
 * component
 */
#define ecs_component(name) \
    struct name;\
    void name##_find(id ctx, unsigned int entity, id *pid);\
    void name##_request(id ctx, unsigned int entity, id *pid);

#define make_ecs_component(name) \
static signed name##_type = -1;\
struct name;\
static void name##_init(struct name *s, key k);\
static void name##_clear(struct name *s);\
static void name##_fetch(id pid, struct name **ptr)\
{\
    void fetch(id pid, unsigned int type, void *p);\
    fetch(pid, (unsigned int)name##_type, ptr);\
}\
void name##_find(id ctx, unsigned int entity, id *pid)\
{\
    void ecs_context_get_component(id ctx, unsigned int entity, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_component(ctx, entity, name##_type, pid);\
}\
void name##_request(id ctx, unsigned int entity, id *pid)\
{\
    void ecs_context_add_component(id ctx, unsigned int entity, id cid);\
    void ecs_context_get_component(id ctx, unsigned int entity, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_component(ctx, entity, name##_type, pid);\
    if (!id_validate(*pid)) {\
        create(name##_type, pid);\
        build(*pid, key_null);\
        ecs_context_add_component(ctx, entity, *pid);\
        release(*pid);\
        ecs_context_check_entity(ctx, entity);\
    }\
}

#endif