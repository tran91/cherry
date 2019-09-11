#include "graphics/collada.h"

int main(int argc, char **argv)
{
    id ctx, node;
    const float *fptr;
    unsigned flen;

    collada_context_new(&ctx);
    collada_context_load_file(ctx, "inner://res/model/cube.dae");

    collada_context_get_node(ctx, "Cube", &node);
    if (id_validate(node)) {
        collada_node_get_vertice(node, &fptr, &flen);
        debug("vertice: %u\n", flen);

        collada_node_get_normals(node, &fptr, &flen);
        debug("normals: %u\n", flen);

        collada_node_get_texcoords(node, &fptr, &flen);
        debug("texcoords: %u\n", flen);

        collada_node_get_colors(node, &fptr, &flen);
        debug("colors: %u\n", flen);
    }

    release(ctx);
}