#ifndef __file_h
#define __file_h

#include "id.h"

type(file);
void file_open(id pid, const char *path);
void file_write(id pid, const void *buf, unsigned int len);
void file_read(id pid, unsigned char *buf, unsigned int len, unsigned int *read);
void file_seek(id pid, unsigned int offset);
void file_size(const char *path, unsigned int *size);

#endif