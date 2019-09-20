#ifndef __utf8_h
#define __utf8_h

void utf8_width(const char *c, unsigned char *width);
void utf8_code(const char *c_ptr, unsigned *code);

#endif