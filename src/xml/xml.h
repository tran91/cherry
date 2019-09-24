#ifndef __xml_h
#define __xml_h

#include "types/id.h"

type(xml_node);
void xml_node_add_node(id pid, id cid);
void xml_node_get_parent(id pid, id *rid);
void xml_node_add_attribute(id pid, id aid);
void xml_node_get_attribute_by_index(id pid, unsigned int index, id *aid);
void xml_node_get_attribute_by_name(id pid, const char *name, id *aid);
void xml_node_set_name(id pid, const char *name);
void xml_node_set_value(id pid, const char *value);
void xml_node_get_name(id pid, const char * const *ptr, unsigned int *len);
void xml_node_get_value(id pid, const char * const *ptr, unsigned int *len);
void xml_node_load_file(id pid, const char *path);

type(xml_attribute);
void xml_attribute_set_name(id pid, const char *name);
void xml_attribute_set_value(id pid, const char *value);
void xml_attribute_get_name(id pid, const char * const *ptr, unsigned int *len);
void xml_attribute_get_value(id pid, const char * const *ptr, unsigned int *len);

type(xml_query);
void xml_query_search(id pid, id nid, const char *command);
void xml_query_get_node(id pid, unsigned int index, id *nid);
void xml_query_get_node_length(id pid, unsigned int *len);
void xml_query_get_attribute(id pid, unsigned int index, id *aid);
void xml_query_get_attribute_length(id pid, unsigned int *len);

#endif