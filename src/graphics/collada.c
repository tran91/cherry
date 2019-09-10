#include "collada.h"
#include "types/vector.h"
#include "types/map.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "xml/xml.h"

struct collada_context
{
    id meshs;
    id amatures;
    id skins;
    id nodes;
};
make_type(collada_context);

static void collada_context_init(struct collada_context *p, key k)
{
    map_new(&p->meshs);
    map_new(&p->amatures);
    map_new(&p->skins);
    map_new(&p->nodes);
}

static void collada_context_clear(struct collada_context *p)
{
    release(p->meshs);
    release(p->amatures);
    release(p->skins);
    release(p->nodes);
}

void collada_context_load_file(id pid, const char *path)
{

}