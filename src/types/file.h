#ifndef __file_h
#define __file_h

#include "id.h"

type(file);
void file_open(id pid, const char *path);
void file_write(id pid, void *buf, unsigned len);
void file_read(id pid, unsigned char *buf, unsigned len, unsigned *read);
void file_seek(id pid, unsigned offset);
void file_size(const char *path, unsigned *size);

#endif