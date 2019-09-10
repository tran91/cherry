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
void buffer_append(id pid, const void *buf, const unsigned len);

/*
 * @pid: buffer
 * @cid: buffer
 */
void buffer_append_buffer(id pid, id cid);

/*
 * @pid: buffer
 */
void buffer_get_length(id pid, unsigned *len);

/*
 * @pid: buffer
 */
void buffer_replace(id pid, const void *search, const unsigned slen, const void *replace, const unsigned rlen);

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