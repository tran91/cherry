#ifndef __ecs_h
#define __ecs_h

#include "types/id.h"

type(ecs_context);
void ecs_context_new_entity(id ctx, unsigned *entity);

#define component(name) \
    struct name;\
    void name##_find(id ctx, unsigned entity, id *pid);\
    void name##_request(id ctx, unsigned entity, id *pid);

#define make_component(name) \
static signed name##_type = -1;\
struct name;\
static void name##_init(struct name *s, key k);\
static void name##_clear(struct name *s);\
static void name##_fetch(id pid, struct name **ptr)\
{\
    void fetch(id pid, unsigned type, void *p);\
    fetch(pid, (unsigned)name##_type, ptr);\
}\
void name##_find(id ctx, unsigned entity, id *pid)\
{\
    void ecs_context_get_component(id ctx, unsigned entity, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_component(ctx, entity, name##_type, pid);\
}\
void name##_request(id ctx, unsigned entity, id *pid)\
{\
    void ecs_context_add_component(id ctx, unsigned entity, id cid);\
    void ecs_context_get_component(id ctx, unsigned entity, signed type, id *cid);\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    ecs_context_get_component(ctx, entity, name##_type, pid);\
    if (!id_validate(*pid)) {\
        create(name##_type, pid);\
        build(*pid, key_null);\
        ecs_context_add_component(ctx, entity, *pid);\
        release(*pid);\
    }\
}

#endif