#ifndef __json_h
#define __json_h

#include "types/id.h"

type(json_element);

/*
 * @pid: json_element
 */
void json_element_load_file(id pid, const char *path);
void json_element_load_string(id pid, const char *ptr);
void json_element_make_object(id pid);
void json_element_make_array(id pid);
void json_element_make_string(id pid);
void json_element_make_number(id pid);
void json_element_make_boolean(id pid);
void json_element_make_null(id pid);
void json_element_dump(id pid);
void json_element_save_file(id pid, const char *path);

void json_element_is_object(id pid, unsigned char *ret);
void json_element_is_array(id pid, unsigned char *ret);
void json_element_is_string(id pid, unsigned char *ret);
void json_element_is_number(id pid, unsigned char *ret);
void json_element_is_boolean(id pid, unsigned char *ret);
void json_element_is_null(id pid, unsigned char *ret);

void json_object_add(id pid, const char *name, id element);
void json_object_get(id pid, const char *name, id *eid);
void json_object_remove(id pid, const char *name);

void json_array_add(id pid, id element);
void json_array_get(id pid, unsigned int index, id *eid);
void json_array_remove(id pid, unsigned int index);

void json_string_append(id pid, const char *ptr);
void json_string_get_ptr(id pid, const char **ptr);
void json_string_get_length(id pid, unsigned int *len);

void json_number_set(id pid, double n);
void json_number_get(id pid, double *n);

void json_boolean_set(id pid, unsigned char flag);
void json_boolean_get(id pid, unsigned char *flag);

#endif