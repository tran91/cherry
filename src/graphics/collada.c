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

struct collada_mesh
{
    id vertices;
    id normals;
    id texcoords;
    id colors;
    id indice;
    id id;
};
make_type(collada_mesh);

static void collada_mesh_init(struct collada_mesh *p, key k)
{
    buffer_new(&p->vertices);
    buffer_new(&p->normals);
    buffer_new(&p->texcoords);
    buffer_new(&p->colors);
    buffer_new(&p->indice);
    buffer_new(&p->id);
}

static void collada_mesh_clear(struct collada_mesh *p)
{
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->indice);
    release(p->id);
}

struct collada_skin
{
    id vertices;
    id normals;
    id texcoords;
    id colors;
    id bones_id;
    id weights;
    id inverse_bind_poses;
    id bone_names;
    id id;

    unsigned bones_per_vertex;
    unsigned bones_upload;

    id transform;
};
make_type(collada_skin);

static void collada_skin_init(struct collada_skin *p, key k)
{
    p->vertices = id_null;
    p->normals = id_null;
    p->texcoords = id_null;
    p->colors = id_null;
    p->bones_id = id_null;
    p->weights = id_null;
    p->inverse_bind_poses = id_null;
    p->bone_names = id_null;
    
    buffer_new(&p->id);
    mat4_new(&p->transform);

    p->bones_per_vertex = 0;
    p->bones_upload = 0;
}

static void collada_skin_clear(struct collada_skin *p)
{
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->bones_id);
    release(p->weights);
    release(p->inverse_bind_poses);
    release(p->bone_names);
    release(p->id);
    release(p->transform);
}

struct collada_bone
{
    id children;
    id parent;
    id transform;
    id transform_relative;
    id id;
    id sid;
    id animation_times;
    id animation_transforms;
};
make_type(collada_bone);

static void collada_bone_init(struct collada_bone *p, key k)
{
    vector_new(&p->children);
    p->parent = id_null;
    mat4_new(&p->transform);
    mat4_new(&p->transform_relative);
    buffer_new(&p->id);
    buffer_new(&p->sid);
    buffer_new(&p->animation_times);
    buffer_new(&p->animation_transforms);
}

static void collada_bone_clear(struct collada_bone *p)
{
    release(p->children);
    p->parent = id_null;
    release(p->transform);
    release(p->transform_relative);
    release(p->id);
    release(p->sid);
    release(p->animation_times);
    release(p->animation_transforms);
}

struct collada_amature
{
    id transform;
    id bones;
    id id;
};
make_type(collada_amature);

static void collada_amature_init(struct collada_amature *p, key k)
{
    mat4_new(&p->transform);
    map_new(&p->bones);
    buffer_new(&p->id);
}

static void collada_amature_clear(struct collada_amature *p)
{
    release(p->transform);
    release(p->id);
    release(p->bones);   
}

struct collada_node
{
    id children;
    id parent;

    id amature;

    id vertices;
    id normals;
    id texcoords;
    id colors;
    id bones_id;
    id weights;
    id inverse_bind_poses;
    id bone_names;
    id id;

    unsigned bones_per_vertex;
    unsigned bones_upload;

    id transform;
};
make_type(collada_node);

static void collada_node_init(struct collada_node *p, key k)
{
    vector_new(&p->children);
    p->parent = id_null;

    p->amature = id_null;

    p->vertices = id_null;
    p->normals = id_null;
    p->texcoords = id_null;
    p->colors = id_null;
    p->bones_id = id_null;
    p->weights = id_null;
    p->inverse_bind_poses = id_null;
    p->bone_names = id_null;
    
    buffer_new(&p->id);
    mat4_new(&p->transform);

    p->bones_per_vertex = 0;
    p->bones_upload = 0;
}

static void collada_node_clear(struct collada_node *p)
{
    release(p->children);
    release(p->amature);
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->bones_id);
    release(p->weights);
    release(p->inverse_bind_poses);
    release(p->bone_names);
    release(p->id);
    release(p->transform);
}

void dump()
{
    id test;
    collada_context_fetch(test, NULL);
    collada_mesh_fetch(test, NULL);
    collada_skin_fetch(test, NULL);
    collada_bone_fetch(test, NULL);
    collada_amature_fetch(test, NULL);
    collada_node_fetch(test, NULL);
}

void collada_context_load_file(id pid, const char *path)
{

}