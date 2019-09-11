#include "graphics/collada.h"

int main(int argc, char **argv)
{
    id ctx;

    collada_context_new(&ctx);
    collada_context_load_file(ctx, "inner://res/model/cube.dae");
    release(ctx);
}