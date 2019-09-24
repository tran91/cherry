#ifndef __buffer_h
#define __buffer_h

#include "id.h"

type(buffer);

/*
 * @pid: buffer
 */
void buffer_erase(id pid);

/*
 * @pid: buffer
 */
void buffer_append_file(id pid, const char *path);

/*
 * @pid: buffer
 */
void buffer_append(id pid, const void *buf, const unsigned int len);

/*
 * @pid: buffer
 * @cid: buffer
 */
void buffer_append_buffer(id pid, id cid);

void buffer_append_float_string(id pid, const char *ptr, float *max);
void buffer_append_int_string(id pid, const char *ptr, int *max);

/*
 * @pid: buffer
 */
void buffer_reserve(id pid, unsigned int len);

/*
 * @pid: buffer
 */
void buffer_get_length(id pid, unsigned int *len);
void buffer_get_length_with_stride(id pid, unsigned int stride, unsigned int *len);

void buffer_cut_with_stride(id pid, unsigned int stride, unsigned int index);
void buffer_get_with_stride(id pid, unsigned int stride, unsigned int index, void *mem);

/*
 * @pid: buffer
 */
void buffer_replace(id pid, const void *search, const unsigned int slen, const void *replace, const unsigned int rlen);

/*
 * @pid: buffer
 * 
 * example:
 *      id buf;
 *      buffer_new(&buf);
 *      
 *      ...
 * 
 *      const char *str;
 *      buffer_get(pid, &str);
 *      if (str) {
 *          ...
 *      } else {
 *          ...
 *      }
 */
void buffer_get_ptr(id pid, const void *ptr);

#endif