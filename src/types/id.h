#ifndef __id_h
#define __id_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

/* spin lock */
static inline void lock(volatile unsigned *l)
{
    while(__sync_lock_test_and_set(l, 1)) {
        while(*l);
    }
}

static inline void unlock(volatile unsigned *l)
{
    __sync_lock_release(l);
}

/* id */
typedef struct id 
{
    signed mask;
    signed index;
} id;

enum
{
    KEY_LITERAL = 1,
    KEY_ID
};

typedef struct key
{
    char type;
    union {
        struct {
            const char *ptr;
            unsigned len;
        };
        id kid;
    };
} key;

#define key_chars(p) (key){.type = KEY_LITERAL, .ptr = p, .len = strlen(p)} 
#define key_literal(p) (key){.type = KEY_LITERAL, .ptr = p, .len = sizeof(p) - 1}
#define key_id(p) (key){.type = KEY_ID, .kid = p}
#define key_null (key){.type = 0} 

#define id_null (id){-1, -1}
#define id_init (id){-1, -1}
#define id_equal(a, b) ((a).mask == (b).mask && (a).index == (b).index)
#define id_validate(a) ((a).mask >= 0 && (a).index >= 0)

void invalidate(id *pid);
void require(void(*init)(void*, key), void(*clear)(void*), unsigned size, signed *type);
void create(unsigned type, id *pid);
void build(id pid, key k);
void retain(id pid);
void release(id pid);
void fetch(id pid, void *p);

/* 
 * register type
 */
#include "map.h"

#define type(name) \
void name##_shared(key k, id *pid);\
void name##_new(id *pid);

#define make_type(name)\
static id cache = id_init;\
static void clean()\
{\
    release(cache);\
}\
static signed type = -1;\
static volatile unsigned barrier = 0;\
struct name;\
static void init(struct name *s, key k);\
static void clear(struct name *s);\
void name##_shared(key k, id *pid)\
{\
    require((void(*)(void*, key))init, (void(*)(void*))clear, sizeof(struct name), &type);\
\
    if (!id_validate(cache)) {\
        map_shared(key_null, &cache);\
        atexit(clean);\
    }\
\
    if (k.type >= 1) {\
        lock(&barrier);\
        map_get(cache, k, pid);\
        if (id_validate(*pid)) {\
            retain(*pid);\
            unlock(&barrier);\
        } else {\
            create(type, pid);\
            map_set(cache, k, *pid);\
            unlock(&barrier);\
            build(*pid, k);\
        }\
    } else {\
        create(type, pid);\
        build(*pid, k);\
    }\
}\
void name##_new(id *pid)\
{\
    require((void(*)(void*, key))init, (void(*)(void*))clear, sizeof(struct name), &type);\
    create(type, pid);\
    build(*pid, key_null);\
}

#define make_type_detail(name)\
static id name##_cache = id_init;\
static void name##_clean()\
{\
    release(name##_cache);\
}\
static signed name##_type = -1;\
static volatile unsigned name##_barrier = 0;\
struct name;\
static void name##_init(struct name *s, key k);\
static void name##_clear(struct name *s);\
void name##_shared(key k, id *pid)\
{\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
\
    if (!id_validate(name##_cache)) {\
        map_shared(key_null, &name##_cache);\
        atexit(name##_clean);\
    }\
\
    if (k.type >= 1) {\
        lock(&name##_barrier);\
        map_get(name##_cache, k, pid);\
        if (id_validate(*pid)) {\
            retain(*pid);\
            unlock(&name##_barrier);\
        } else {\
            create(name##_type, pid);\
            map_set(name##_cache, k, *pid);\
            unlock(&name##_barrier);\
            build(*pid, k);\
        }\
    } else {\
        create(name##_type, pid);\
        build(*pid, k);\
    }\
}\
void name##_new(id *pid)\
{\
    require((void(*)(void*, key))name##_init, (void(*)(void*))name##_clear, sizeof(struct name), &name##_type);\
    create(name##_type, pid);\
    build(*pid, key_null);\
}

/* platform */
#define UNKNOWN 0
#define IOS 1
#define DROID 2
#define LINUX 3
#define OSX 4
#define WEB 5
#define WINDOWS 6

#define OS LINUX

#if defined(EMSCRIPTEN)
    #undef OS
    #define OS WEB
#elif defined(__APPLE__)

    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
            #undef OS
            #define OS IOS
    #elif TARGET_OS_IPHONE

            #undef OS
            #define OS IOS

    #elif TARGET_OS_MAC
            #undef OS
            #define OS OSX
    #endif

#elif defined(ANDROID)

    #undef OS
    #define OS DROID

#elif defined(WIN32) || defined(_WIN32)

    #undef OS
    #define OS WINDOWS

#elif defined(linux)

    #undef OS
    #define OS LINUX

#endif

#if OS == DROID
    #include <android/log.h>
    #include <zip/zip.h>
#endif

#ifdef NDEBUG
    #define debug(...)
#else
    #if OS == DROID
        #define debug(...) \
            __android_log_print(ANDROID_LOG_DEBUG, \
                "app_debug",__VA_ARGS__)
    #else
        #define debug(...) printf(__VA_ARGS__)
    #endif
#endif

#endif