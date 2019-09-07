#ifndef __file_h
#define __file_h

#include "id.h"

type(file);
void file_write(id pid, void *buf, unsigned len);
void file_read(id pid, char buf[8192], unsigned *read);
void file_seek(id pid, unsigned offset);

#endif