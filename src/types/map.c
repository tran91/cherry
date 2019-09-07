#include "map.h"

static const int BLOCK_SIZE[] = {
    2, 5, 11, 23, 47, 97, 197, 397, 797, 1597,
    3203, 6421, 7919, 16069, 33391, 69313, 150151
};

static inline unsigned hash(key k, const unsigned cache_size)
{
    const void* key;
    unsigned key_len;
    switch (k.type) {
    case KEY_LITERAL:
        key = k.ptr;
        key_len = k.len;
        break;
    case KEY_ID:
        key = &k.kid;
        key_len = sizeof(k.kid);
        break;
    default:
        key_len = 0;
        break;
    }
    unsigned prime = 5381;
    const char *ptr = (const char *)key;
    unsigned i = 0;
    while (i < key_len) {
        prime = ((prime << 5) + prime) + *ptr;
        ptr++;
        i++;
    }
    return prime % cache_size;
}

struct cell
{
    struct cell *next;
    key k;
    id object;
    unsigned index;
};

struct entry 
{
    struct cell **list;
    unsigned count;
};

struct map
{
    struct entry **entries;
    char *flag;
    unsigned len;

    struct cell **ctr;
    unsigned count;
};

static void init(struct map *p, key k)
{
    p->len = 0;
    p->entries = NULL;
    p->flag = NULL;

    p->ctr = NULL;
    p->count = 0;
}

static void clear(struct map *p)
{
    int i, j;
    struct entry *e;
    struct cell *c, **n;
    if (p->len) {
        free(p->flag);

        for (i = 0; i < p->len; ++i) {
            e = p->entries[i];
            for (j = 0; j < BLOCK_SIZE[i]; ++j) {
                n = e->list + j;
                c = *n;
                while (c) {
                    *n = c->next;
                    release(c->object);
                    if (c->k.type == KEY_LITERAL) {
                        free(c->k.ptr);
                    }
                    free(c);
                    c = *n;
                }                
            }
            free(e->list);
            free(e);
        }
        free(p->entries);
        p->len = 0;
        p->entries = NULL;
        p->flag = NULL;
    }
    if (p->ctr) {
        free(p->ctr);
        p->ctr = NULL;
        p->count = 0;
    }
}

static void get(struct map *p, key k, struct cell **ret)
{
    struct entry *e;
    struct cell *c = NULL;
    int i;
    size_t h;

    for (i = 0; i < p->len; ++i) {
        e = p->entries[i];
        h = hash(k, BLOCK_SIZE[i]);
        c = e->list[h];
        while (c) {
            if (c->k.type == k.type) {
                switch (k.type) {
                case KEY_LITERAL:
                    if (c->k.len == k.len && memcmp(c->k.ptr, k.ptr, k.len) == 0) {
                        goto finish;
                    }
                    break;
                case KEY_ID:
                    if (id_equal(c->k.kid, k.kid)) {
                        goto finish;
                    }
                    break;
                }
            }
            c = c->next;
        }
    }
    c = NULL;

finish:
    *ret = c;
}

static void set(struct map *p, key k, id object)
{
    int slot, j;
    struct entry *e;
    struct cell *c, *n = NULL;
    size_t h;

    get(p, k, &n);
    if (n) {
        if (id_equal(n->object, object)) goto finish;
        retain(object);
        release(n->object);
        n->object = object;
        goto finish;
    }

    for (slot = 0; slot < p->len; ++slot) {
        if (p->flag[slot] == 0) continue;

        goto create;
    }

    p->len++;
    p->entries = realloc(p->entries, sizeof(struct entry *) * p->len);
    p->flag = realloc(p->flag, sizeof(char) * p->len);

    slot = p->len - 1;
    p->flag[slot] = 1;

    p->entries[slot] = malloc(sizeof(struct entry));
    e = p->entries[slot];

    e->count = 0;
    e->list = malloc(sizeof(struct cell *) * BLOCK_SIZE[slot]);
    for (j = 0; j < BLOCK_SIZE[slot]; ++j) {
        e->list[j] = NULL;
    }

create:
    retain(object);

    e = p->entries[slot];
    h = hash(k, BLOCK_SIZE[slot]);
    c = e->list[h];

    n = malloc(sizeof(struct cell));
    
    n->object = object;
    n->next = c;
    n->k.type = k.type;
    switch (k.type) {
    case KEY_LITERAL:
        n->k.ptr = malloc(k.len + 1);
        memcpy(n->k.ptr, k.ptr, k.len);
        n->k.ptr[k.len] = '\0'; 
        break;
    case KEY_ID:
        n->k.kid = k.kid;
        break;
    }

    e->list[h] = n;
    e->count++;
    if (e->count >= BLOCK_SIZE[slot] * 0.7f) {
        p->flag[slot] = 0;
    }

    p->ctr = realloc(p->ctr, sizeof(struct cell *) * (p->count + 1));
    p->ctr[p->count] = n;
    n->index = p->count;
    p->count++;

finish:
    ;
}

void __remove(struct map *p, key k)
{
    struct entry *e;
    struct cell *c, *prev;
    int i;
    size_t h;

    for (i = 0; i < p->len; ++i) {
        e = p->entries[i];
        h = hash(k, BLOCK_SIZE[i]);
        c = e->list[h];
        prev = NULL;
        while (c) {
            if (c->k.type == k.type) {
                switch (k.type) {
                case KEY_LITERAL:
                    if (c->k.len == k.len && memcmp(c->k.ptr, k.ptr, k.len) == 0) {
                        p->ctr[c->index] = p->ctr[p->count - 1];
                        p->ctr[c->index]->index = c->index;
                        p->count--;

                        if (prev) {
                            prev->next = c->next;                    
                        } else {
                            e->list[h] = c->next;
                        }
                        release(c->object);
                        free(c->k.ptr);
                        free(c);              
                        e->count--;
                        p->flag[i] = 1;

                        goto finish;
                    }
                    break;
                case KEY_ID:
                    if (id_equal(c->k.kid, k.kid)) {
                        p->ctr[c->index] = p->ctr[p->count - 1];
                        p->ctr[c->index]->index = c->index;
                        p->count--;

                        if (prev) {
                            prev->next = c->next;                    
                        } else {
                            e->list[h] = c->next;
                        }
                        release(c->object);
                        free(c);              
                        e->count--;
                        p->flag[i] = 1;

                        goto finish;
                    }
                    break;
                }
            }
            prev = c;
            c = c->next;
        }
    }

finish:
    ;
}

static signed type = -1;
static struct map *cache = NULL;
static volatile unsigned barrier = 0;

static void clean()
{
    clear(cache);
    free(cache);
    cache = NULL;
}

void map_shared(key k, id *map)
{
    struct cell *c;
    
    require((void(*)(void*, key))init, (void(*)(void*))clear, sizeof(struct map), &type);
    if (!cache) {
        atexit(clean);
        cache = malloc(sizeof(struct map));
        init(cache, key_null);
    }

    if (k.type >= 1) {
        lock(&barrier);
        get(cache, k, &c);
        if (c) {
            *map = c->object;
            retain(*map);
            unlock(&barrier);
        } else {
            create(type, map);
            set(cache, k, *map);
            unlock(&barrier);
            build(*map, k);
        }
    } else {
        create(type, map);
        build(*map, k);
    }
}

void map_new(id *map)
{
    require((void(*)(void*, key))init, (void(*)(void*))clear, sizeof(struct map), &type);
    create(type, map);
    build(*map, key_null);
}

void map_set(id map, key k, id object)
{
    struct map *raw;
    
    fetch(map, &raw);
    assert(raw != NULL);
    set(raw, k, object);
}

void map_get(id map, key k, id *object)
{
    struct map *raw;
    struct cell *c;
    
    fetch(map, &raw);
    assert(raw != NULL);
    get(raw, k, &c);
    if (c) {
        *object = c->object;
    } else {
        invalidate(object);
    }
}

void map_remove(id map, key k)
{
    struct map *raw;
    
    fetch(map, &raw);
    assert(raw != NULL);
    __remove(raw, k);
}

void map_iterate(id map, unsigned index, key *k, id *object)
{
    struct map *raw;

    fetch(map, &raw);
    assert(raw != NULL);

    if (index >= raw->count) {
        *k = key_null;
        invalidate(object);
    } else {
        *k = raw->ctr[index]->k;
        *object = raw->ctr[index]->object;
    }
}