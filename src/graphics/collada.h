#ifndef __collada_h
#define __collada_h

#include "types/id.h"

type(collada_context);
type(collada_node);
type(collada_mesh);
type(collada_skin);
type(collada_amature);
type(collada_bone);

void collada_context_load_file(id pid, const char *path);

#endif