#ifndef __image_h
#define __image_h

#include "types/id.h"

type(image);
void image_load_file(id pid, const char *path);
void image_get_size(id pid, unsigned *width, unsigned *height);
void image_get_ptr(id pid, const unsigned char **ptr);

#endif