#include "xml.h"
#include "types/map.h"
#include "types/vector.h"
#include "types/buffer.h"
#include "utils/utf8.h"

/*
 * xml_node
 */
struct xml_node
{
    id name;
    id value;

    id parent;
    id children;    

    id attributes;
};
struct xml_attribute
{
    id name;
    id value;

    id node;
};

make_type_detail(xml_attribute);
make_type_detail(xml_node);

static void xml_node_init(struct xml_node *p, key k)
{
    buffer_new(&p->name);
    buffer_new(&p->value);
    vector_new(&p->children);
    map_new(&p->attributes);
    p->parent = id_null;
}

static void xml_node_clear(struct xml_node *p)
{
    release(p->name);
    release(p->value);
    release(p->children);
    release(p->attributes);
}

void xml_node_get_parent(id pid, id *rid)
{
    struct xml_node *raw;
    fetch(pid, &raw);
    assert(raw != NULL);

    *rid = raw->parent;
}

void xml_node_add_node(id pid, id cid)
{
    struct xml_node *rp, *rc, *ocp;

    fetch(cid, &rc);
    assert(rc != NULL);
    if (id_equal(rc->parent, pid)) return;
    fetch(pid, &rp);
    assert(rp != NULL);

    retain(cid);
    fetch(rc->parent, &ocp);
    if (ocp) {        
        vector_remove_id(ocp->children, cid);
    }
    rc->parent = pid;
    vector_push(rp->children, cid);
    release(cid);
}

void xml_node_add_attribute(id pid, id aid)
{
    struct xml_node *rp, *ocp;
    struct xml_attribute *rc;
    const char *ptr;
    unsigned len;

    fetch(aid, &rc);
    assert(rc != NULL);
    if (id_equal(rc->node, pid)) return;
    fetch(pid, &rp);
    assert(rp != NULL);

    retain(aid);
    buffer_get_ptr(rc->name, &ptr);
    buffer_get_length(rc->name, &len);
    fetch(rc->node, &ocp);
    if (ocp) {        
        map_remove(ocp->attributes, key_mem(ptr, len));
    }
    rc->node = pid;
    map_set(rp->attributes, key_mem(ptr, len), aid);
    release(aid);
}

void xml_node_get_attribute_by_index(id pid, unsigned index, id *aid)
{
    struct xml_node *r;
    key k;

    fetch(pid, &r);
    assert(r != NULL);
    map_iterate(r->attributes, index, &k, aid);
}

void xml_node_get_attribute_by_name(id pid, const char *name, id *aid)
{
    struct xml_node *r;

    fetch(pid, &r);
    assert(r != NULL);
    map_get(r->attributes, key_chars(name), aid);
}

static void __xml_node_set_name(id pid, const char *name, const unsigned name_len)
{
    struct xml_node *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_clear(r->name);
    buffer_append(r->name, name, name_len);
}

void xml_node_set_name(id pid, const char *name)
{
    __xml_node_set_name(pid, name, strlen(name));
}

static void __xml_node_set_value(id pid, const char *value, const unsigned value_len)
{
    struct xml_node *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_clear(r->value);
    buffer_append(r->value, value, value_len);
}

void xml_node_set_value(id pid, const char *value)
{
    __xml_node_set_value(pid, value, strlen(value));
}

void xml_node_get_name(id pid, const char * const *ptr, unsigned *len)
{
    struct xml_node *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_get_ptr(r->name, ptr);
    buffer_get_length(r->name, len);
}

void xml_node_get_value(id pid, const char * const *ptr, unsigned *len)
{
    struct xml_node *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_get_ptr(r->value, ptr);
    buffer_get_length(r->value, len);
}

/*
 * attribute
 */
static void xml_attribute_init(struct xml_attribute *p, key k)
{
    buffer_new(&p->name);
    buffer_new(&p->value);
    p->node = id_null;
}

static void xml_attribute_clear(struct xml_attribute *p)
{
    release(p->name);
    release(p->value);
}

void __xml_attribute_set_name(id pid, const char *name, const unsigned name_len)
{
    struct xml_attribute *r;
    struct xml_node *rp;
    const char *ptr;
    unsigned len;

    fetch(pid, &r);
    assert(r != NULL);

    buffer_get_ptr(r->name, &ptr);
    buffer_get_length(r->name, &len);
    if (len == name_len && strncmp(ptr, name, name_len) == 0) return;

    retain(pid);
    fetch(r->node, &rp);
    if (rp) {
        map_remove(rp->attributes, key_mem(ptr, len));
        map_set(rp->attributes, key_mem(name, name_len), pid);
    }
    buffer_clear(r->name);
    buffer_append(r->name, name, name_len);
    release(pid);
}

void xml_attribute_set_name(id pid, const char *name)
{
    __xml_attribute_set_name(pid, name, strlen(name));
}

void __xml_attribute_set_value(id pid, const char *value, const unsigned value_len)
{
    struct xml_attribute *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_clear(r->value);
    buffer_append(r->value, value, value_len);
}

void xml_attribute_set_value(id pid, const char *value)
{
    __xml_attribute_set_value(pid, value, strlen(value));
}

void xml_attribute_get_name(id pid, const char * const *ptr, unsigned *len)
{
    struct xml_attribute *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_get_ptr(r->name, ptr);
    buffer_get_length(r->name, len);
}

void xml_attribute_get_value(id pid, const char * const *ptr, unsigned *len)
{
    struct xml_attribute *r;

    fetch(pid, &r);
    assert(r != NULL);
    buffer_get_ptr(r->value, ptr);
    buffer_get_length(r->value, len);
}

/*
 * load file
 */
static const char *null = (const char[2]){'\0', '\0'};

static inline void __set_reverse(const unsigned n,
	const char **restrict l, const char * restrict r)
{
	int i;
	for(i = 0; i < n; ++i) {
		l[i] = &r[n - i - 1];
	}
}

enum {
	BLOCK_COMMENT,
	BLOCK_PROLOG,
	BLOCK_NODE,
	BLOCK_END_NODE,
	BLOCK_FULL_NODE,
	BLOCK_TEXT
};

static void __get_block(const char * str, const char **start,
	const char **end, char *type)
{
	unsigned char width;
	const char *last;
	const char *ptr = str;
	signed last_len;
	char stack_literal;
	char stack_mark;

	*start = &null[1];
	*end = &null[0];
	*type = -1;

	last = ">";
	last_len = 1;

head:
	switch (*ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			ptr++;
			goto head;
		case '<':
			break;
		case '\0':
			goto finish;
		default:
			utf8_width(ptr, &width);
			ptr += width;
			goto head;
	}

	if(ptr > str) {
		*start = str;
		*end = ptr - 1;
		*type = BLOCK_TEXT;
		goto finish;
	}

	if(strncmp(ptr, "<!--", 4) == 0) {
		*start = ptr;
		ptr += 4;
		last = "-->";
		last_len = 3;
		*type = BLOCK_COMMENT;
	} else if(strncmp(ptr, "<?xml", 5) == 0) {
		*start = ptr;
		ptr += 5;
		last = "?>";
		last_len = 2;
		*type = BLOCK_PROLOG;
	} else {
		*start = ptr;
		ptr++;
		switch (*ptr) {
			case ' ': case '\t': case '\n':
			case '\v': case '\f': case '\r':
				*type = -1;
				*start = &null[1];
				*end = &null[0];
				goto finish;
			case '/':
				*type = BLOCK_END_NODE;
				break;
			default:
				*type = BLOCK_NODE;
				break;
		}
	}

	stack_literal = 0;
	stack_mark = 0;
tail:
	switch (*type) {
		case BLOCK_COMMENT:
			if(strncmp(ptr, last, last_len) == 0) {
				*end = ptr + last_len - 1;
			} else {
				if(*ptr) {
					utf8_width(ptr, &width);
					ptr += width;
					goto tail;
				} else {
					*start = &null[1];
					*end = &null[0];
					*type = -1;
				}
			}
			break;
		default:
			if(!stack_literal) {
				switch (*ptr) {
					case '\'': case '\"':
						stack_literal = 1;
						stack_mark = *ptr;
						ptr++;
						goto tail;
					default:
						if(strncmp(ptr, last, last_len) == 0) {
							*end = ptr + last_len - 1;
							if(*type == BLOCK_NODE) {
								if(*(*end - 1) == '/') {
									*type = BLOCK_FULL_NODE;
								}
							}
						} else {
							if(*ptr) {
								utf8_width(ptr, &width);
								ptr += width;
								goto tail;
							} else {
								*start = &null[1];
								*end = &null[0];
								*type = -1;
							}
						}
						break;
				}
			} else {
				if(*ptr == stack_mark) {
					stack_literal = 0;
					ptr++;
					goto tail;
				} else {
					if(*ptr) {
						utf8_width(ptr, &width);
						ptr += width;
						goto tail;
					} else {
						*start = &null[1];
						*end = &null[0];
						*type = -1;
					}
				}
			}
			break;
	}
finish:
	;
}

static void update_node(id pid, const char *start, const char *end,
	const char *value_start, const char *value_end)
{
	const char *ptr_prev, *ptr;
	const char *name[2];
	const char *value[2];
	char value_mark;
	unsigned char width;
    id attr;

	ptr = start;
	ptr_prev = ptr;
	ptr++;

	__set_reverse(2, name, null);
	__set_reverse(2, value, null);

name:
	switch (*ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
		case '\0': case '/':
			name[0] = start + 1;
			name[1] = ptr - 1;
			value[0] = value_start;
			value[1] = value_end;

            if (name[1] >= name[0]) {
                __xml_node_set_name(pid, name[0], name[1] - name[0] + 1);
            }
            if (*value[0]) {
                __xml_node_set_value(pid, value[0], value[1] - value[0] + 1);
            }

			if(*ptr == '\0') {
				goto finish;
			}
			ptr_prev = ptr;
			ptr++;
			break;
		case '=':
			goto finish;
		default:
			ptr_prev = ptr;
			utf8_width(ptr, &width);
			ptr += width;
			goto name;
	}

search_attribute:
	__set_reverse(2, name, null);
	__set_reverse(2, value, null);

	switch (*ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			ptr_prev = ptr;
			ptr++;
			goto search_attribute;
		case '/': case '=': case '\0':
			goto finish;
		default:
			break;
	}

	name[0] = name[1] = ptr;
read_attribute_name:
	switch (*ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
		case '=':
			name[1] = ptr_prev;
			ptr++;
			break;
		case '/': case '\0':
			goto finish;
		default:
			ptr_prev = ptr;
			utf8_width(ptr, &width);
			ptr += width;
			goto read_attribute_name;
	}

search_value:
	switch (*ptr) {
		case '\'': case '\"':
			value[0] = value[1] = ptr + 1;
			value_mark = *ptr;
			ptr_prev = ptr;
			ptr++;
			break;
		case '/': case '=': case '\0':
			goto finish;
		default:
			ptr_prev = ptr;
			utf8_width(ptr, &width);
			ptr += width;
			goto search_value;
	}

read_value:
	switch (*ptr) {
		case '\0':
			goto finish;
		default:
			if(*ptr == value_mark) {
				value[1] = ptr - 1;

                xml_attribute_new(&attr);

                if (name[1] >= name[0]) {
                    __xml_attribute_set_name(attr, name[0], name[1] - name[0] + 1);
                }
                if (*value[0]) {
                    __xml_attribute_set_value(attr, value[0], value[1] - value[0] + 1);
                }

                xml_node_add_attribute(pid, attr);
                release(attr);

				ptr++;
				goto search_attribute;
			} else {
				ptr_prev = ptr;
				utf8_width(ptr, &width);
				ptr += width;
				goto read_value;
			}
	}

finish:
    ;
}

static void __parse(id pid, const char *str)
{
	const char *ptr;
	const char *start, *end;
	const char *start_prev, *end_prev;
	char type, type_prev;
    id n = id_null, current = id_null;

	start = end = start_prev = end_prev = NULL;
	type = type_prev = -1;

	ptr = str;
get:
	__get_block(ptr, &start, &end, &type);
	switch (type) {
		case BLOCK_PROLOG:
			ptr = end + 1;
			goto get;
		case BLOCK_FULL_NODE:
            if(type_prev == BLOCK_NODE) {
                if (!id_validate(current)) {
                    current = pid;
                    update_node(pid, start_prev, end_prev, &null[1], &null[0]);
                } else {
                    xml_node_new(&n);
                    update_node(n, start_prev, end_prev, &null[1], &null[0]);
                    xml_node_add_node(current, n);
                    release(n);

                    current = n;
                }                
				type_prev = -1;
				start_prev = NULL;
				end_prev = NULL;
			}
            if (!id_validate(current)) {
                current = pid;
                update_node(pid, start, end, &null[1], &null[0]);
            } else {
                xml_node_new(&n);
                update_node(n, start, end, &null[1], &null[0]);
                xml_node_add_node(current, n);
                release(n);
                ptr = end + 1;
				goto get;
            }
			break;
		case BLOCK_NODE:
			if(type_prev == BLOCK_NODE) {
                if (!id_validate(current)) {
                    current = pid;
                    update_node(pid, start_prev, end_prev, &null[1], &null[0]);
                } else {
                    xml_node_new(&n);
                    update_node(n, start_prev, end_prev, &null[1], &null[0]);
                    xml_node_add_node(current, n);
                    release(n);

                    current = n;
                }
			}
			start_prev = start;
			end_prev = end;
			type_prev = type;
			ptr = end + 1;
			goto get;
		case BLOCK_TEXT:
			if(type_prev == BLOCK_NODE) {
                if (!id_validate(current)) {
                    current = pid;
                    update_node(pid, start_prev, end_prev, start, end);
                } else {
                    xml_node_new(&n);
                    update_node(n, start_prev, end_prev, start, end);
                    xml_node_add_node(current, n);
                    release(n);

                    current = n;
                }                
				type_prev = -1;
				start_prev = NULL;
				end_prev = NULL;
			}
			ptr = end + 1;
			goto get;
		case BLOCK_END_NODE:
			if(type_prev == BLOCK_NODE) {
                if (!id_validate(current)) {
                    current = pid;
                    update_node(pid, start_prev, end_prev, &null[1], &null[0]);
                } else {
                    xml_node_new(&n);
                    update_node(n, start_prev, end_prev, &null[1], &null[0]);
                    xml_node_add_node(current, n);
                    release(n);
                }                
				type_prev = -1;
				start_prev = NULL;
				end_prev = NULL;
			}
            if (id_validate(current)) {
                xml_node_get_parent(current, &current);
            }
			ptr = end + 1;
			goto get;
		case BLOCK_COMMENT:
			ptr = end + 1;
			goto get;
		default:
			break;
	}
}

void xml_node_load_file(id pid, const char *path)
{
    struct xml_node *raw, *c;
    struct xml_attribute *a;
    key k;
    id obj;
    unsigned index;
    id buf;
    const char *ptr;
    unsigned len;

    fetch(pid, &raw);
    assert(raw != NULL);
    retain(pid);

    /* clear children */
    index = 0;
    vector_get(raw->children, index, &obj);
    while (id_validate(obj)) {
        fetch(obj, &c);
        c->parent = id_null;
        index++;
        vector_get(raw->children, index, &obj);
    }
    vector_clear(raw->children);
    
    /* clear attributes */
    index = 0;
    map_iterate(raw->attributes, index, &k, &obj);
    while (id_validate(obj)) {
        fetch(obj, &a);
        a->node = id_null;
        index++;
        map_iterate(raw->attributes, index, &k, &obj);
    }
    map_clear(raw->attributes);

    /* load file */
    buffer_new(&buf);
    buffer_append_file(buf, path);
    buffer_get_ptr(buf, &ptr);
    buffer_get_length(buf, &len);
    __parse(pid, ptr);
    release(buf);

    release(pid);
}

/*
 * query
 */
struct xml_query
{
    id vnodes;
    id vattrs;
};
make_type_detail(xml_query);

static void xml_query_init(struct xml_query *p, key k)
{
    vector_new(&p->vnodes);
    vector_new(&p->vattrs);
}

static void xml_query_clear(struct xml_query *p)
{
    release(p->vnodes);
    release(p->vattrs);
}

void xml_query_get_node(id pid, unsigned index, id *nid)
{
    struct xml_query *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    vector_get(raw->vnodes, index, nid);
}

void xml_query_get_node_length(id pid, unsigned *len)
{
    struct xml_query *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    vector_get_size(raw->vnodes, len);
}

void xml_query_get_attribute(id pid, unsigned index, id *aid)
{
    struct xml_query *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    vector_get(raw->vattrs, index, aid);
}

void xml_query_get_attribute_length(id pid, unsigned *len)
{
    struct xml_query *raw;

    fetch(pid, &raw);
    assert(raw != NULL);
    vector_get_size(raw->vattrs, len);
}

static void __block_attribute(const char *start, const char *end,
	const char **name_begin, const char **name_end,
	const char **value_begin, const char **value_end)
{
	const char *ptr, *value[2], *name[2];
	char value_mark;
	unsigned char width;

	*name_begin = &null[1];
	*name_end = &null[0];
	*value_begin = &null[1];
	*value_end = &null[0];

	__set_reverse(2, name, null);
	__set_reverse(2, value, null);
	ptr = start;

	while(ptr <= end) {
		switch (*ptr) {
			case '@':
				name[0] = ptr + 1;
				ptr++;
				break;
			case ' ': case '\t': case '\n':
			case '\v': case '\f': case '\r': case ']':
                if (*name[0]) {
                    name[1] = ptr - 1;
                    goto assign;
                } else {
                    ptr++;
                    break;
                }
			case '=':
				if(*name[0]) {
					name[1] = ptr - 1;
					ptr++;
					if(name[1] < name[0]) {
						goto finish;
					}
					goto find_value;
				} else {
					ptr++;
					break;
				}
			default:
				utf8_width(ptr, &width);
				ptr += width;
				break;
		}
	}
	goto finish;

find_value:
	value_mark = 0;
	while(ptr <= end) {
		switch (*ptr) {
			case '\'': case '\"':
				if(*ptr == value_mark) {
					value[1] = ptr - 1;
					goto assign;
				} else {
					value_mark = *ptr;
					value[0] = ptr + 1;
				}
				ptr++;
				break;
			default:
				utf8_width(ptr, &width);
				ptr += width;
				break;
		}
	}
	goto finish;

assign:
	*name_begin = name[0];
	*name_end = name[1];
	*value_begin = value[0];
	*value_end = value[1];

finish:
	;
}

static void __block_name(const char *start, const char *end, const char **mid)
{
	const char *ptr;
	unsigned char width;

	ptr = start;
	*mid = &null[0];

	while(*ptr && ptr <= end) {
		switch (*ptr) {
			case '[':
				*mid = ptr - 1;
				goto finish;
			default:
				*mid = ptr;
				utf8_width(ptr, &width);
				ptr += width;
				break;
		}
	}
finish:
	;
}

static void __block_find(const char *str, const char **start, const char **end)
{
	const char *ptr;
	unsigned char width;

	ptr = str;
    switch (*ptr)
    {
    case '@':
        *start = ptr;
        while (*ptr) {
            *end = ptr;
            ptr++;
        }
        goto finish;
    case '.':
        *start = ptr;
        ptr++;
        goto tail;
    }
head:
	switch (*ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			ptr++;
			goto head;
		case '/':
			*start = ptr + 1;
			if(**start == '/') {
				*end = ptr;
				goto finish;
			}
			ptr++;
			break;
		default:
			*start = &null[1];
			*end = &null[0];
			goto finish;
	}
tail:
	switch (*ptr) {
		case '/': case '\0':
			*end = ptr - 1;
			goto finish;
		default:
			utf8_width(ptr, &width);
			ptr += width;
			goto tail;
	}
finish:
	;
}

static void __fetch_attribute_name(struct xml_query *raw, const char *name, const unsigned name_len)
{
    id set, n, a, t;
	int i, j;
    const char *ptr;
    unsigned ptr_len;

    map_new(&set);
    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        map_get(set, key_id(n), &t);
        if (!id_validate(t)) {
            map_set(set, key_id(n), n);
            j = 0;
            xml_node_get_attribute_by_index(n, j, &a);
            while (id_validate(a)) {
                xml_attribute_get_name(a, &ptr, &ptr_len);
                if (ptr_len == name_len && memcmp(ptr, name, name_len) == 0) {
                    vector_push(raw->vattrs, a);
                }
                j++;
                xml_node_get_attribute_by_index(n, j, &a);
            }
        }        
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    release(set);
}

static void __fetch_all_attribute(struct xml_query *raw)
{
	id set, n, a, t;
	int i, j;

    map_new(&set);
    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        map_get(set, key_id(n), &t);
        if (!id_validate(t)) {
            map_set(set, key_id(n), n);
            j = 0;
            xml_node_get_attribute_by_index(n, j, &a);
            while (id_validate(a)) {
                vector_push(raw->vattrs, a);
                j++;
                xml_node_get_attribute_by_index(n, j, &a);
            }
        }        
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    release(set);
}

static void __keep_node_has_at_least_one_attribute(struct xml_query *raw)
{
    id n, a;
	int i;

    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        xml_node_get_attribute_by_index(n, 0, &a);
        if (!id_validate(a)) {
            vector_remove(raw->vnodes, i);
            i--;
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }
}

static void __keep_node_has_at_least_one_attribute_with_value(
	struct xml_query *raw, const char *value, const unsigned value_len)
{
	id n, a;
	int i, j;
    const char *ptr;
    unsigned ptr_len;

    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        j = 0;
        xml_node_get_attribute_by_index(n, j, &a);
        while (id_validate(a)) {
            xml_attribute_get_value(a, &ptr, &ptr_len);
            if (ptr_len == value_len && memcmp(ptr, value, value_len) == 0) {
                goto next;
            }
            j++;
            xml_node_get_attribute_by_index(n, j, &a);
        }
        vector_remove(raw->vnodes, i);
        i--;
    next:
        i++;
        vector_get(raw->vnodes, i, &n);
    }
}

static void __keep_node_has_attribute(struct xml_query *raw, const char *name, const unsigned name_len)
{
	id n, a;
	int i, j;
    const char *ptr;
    unsigned ptr_len;

    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        j = 0;
        xml_node_get_attribute_by_index(n, j, &a);
        while (id_validate(a)) {
            xml_attribute_get_name(a, &ptr, &ptr_len);
            if (ptr_len == name_len && memcmp(ptr, name, name_len) == 0) {
                goto next;
            }
            j++;
            xml_node_get_attribute_by_index(n, j, &a);
        }
        vector_remove(raw->vnodes, i);
        i--;
    next:
        i++;
        vector_get(raw->vnodes, i, &n);
    }
}

static void __keep_node_has_attribute_with_value(struct xml_query *raw,
	const char *name, const unsigned name_len, 
    const char *value, const unsigned value_len)
{
	id n, a;
	int i, j;
    const char *ptr;
    unsigned ptr_len;

    vector_clear(raw->vattrs);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        j = 0;
        xml_node_get_attribute_by_index(n, j, &a);
        while (id_validate(a)) {
            xml_attribute_get_name(a, &ptr, &ptr_len);
            if (ptr_len == name_len && memcmp(ptr, name, name_len) == 0) {
                xml_attribute_get_value(a, &ptr, &ptr_len);
                if (ptr_len == value_len && memcmp(ptr, value, value_len) == 0) {                                  
                    goto next;
                }
            }
            j++;
            xml_node_get_attribute_by_index(n, j, &a);
        }
        vector_remove(raw->vnodes, i);
        i--;
    next:
        i++;
        vector_get(raw->vnodes, i, &n);
    }
}

static void __move_to_parent(struct xml_query *raw)
{
    id set, n, p, t;
	int i;

    vector_clear(raw->vattrs);
    map_new(&set);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        xml_node_get_parent(n, &p);
        if (!id_validate(p)) {
            goto erase;
        }
        map_get(set, key_id(p), &t);
        if (!id_validate(t)) {
            map_set(set, key_id(p), p);
            vector_set(raw->vnodes, i, p);
            goto next;
        }        
    erase:
        vector_remove(raw->vnodes, i);
        i--;
    next:
        i++;
        vector_get(raw->vnodes, i, &n);
    }
    release(set);
}

static void __get_all_children_has_name(struct xml_query *raw, const char *name, const unsigned name_len)
{
    id set, v, n, c, t;
	int i, j;
    const char *ptr;
    unsigned ptr_len;
    struct xml_node *rn;

    vector_clear(raw->vattrs);
    map_new(&set);
    vector_new(&v);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        fetch(n, &rn);        
        j = 0;
        vector_get(rn->children, j, &c);
        while(id_validate(c)) {
            map_get(set, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(set, key_id(c), c);
                xml_node_get_name(c, &ptr, &ptr_len);
                if (ptr_len == name_len && memcmp(ptr, name, name_len) == 0) {
                    vector_push(v, c);
                }
            }
            j++;
            vector_get(rn->children, j, &c);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    vector_push_vector(raw->vnodes, v);
    release(set);
    release(v);
}

static void __get_all_children(struct xml_query *raw)
{
	id set, v, n, c, t;
	int i, j;
    struct xml_node *rn;

    vector_clear(raw->vattrs);
    map_new(&set);
    vector_new(&v);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        fetch(n, &rn);
        j = 0;
        vector_get(rn->children, j, &c);
        while(id_validate(c)) {
            map_get(set, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(set, key_id(c), c);
                vector_push(v, c);
            }
            j++;
            vector_get(rn->children, j, &c);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    vector_push_vector(raw->vnodes, v);
    release(set);
    release(v);
}

static void __get_all_children_has_name_recursive(struct xml_query *raw, const char *name, const unsigned name_len)
{
    id set, check, v, n, c, t;
	int i, j;
    struct xml_node *rn;
    const char *ptr;
    unsigned ptr_len;

    vector_clear(raw->vattrs);
    map_new(&set);
    map_new(&check);
    vector_new(&v);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        map_get(check, key_id(n), &t);
        if (!id_validate(t)) {
            map_set(check, key_id(n), n);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        fetch(n, &rn);
        j = 0;
        vector_get(rn->children, j, &c);
        while(id_validate(c)) {
            map_get(set, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(set, key_id(c), c);
                xml_node_get_name(c, &ptr, &ptr_len);
                if (ptr_len == name_len && memcmp(ptr, name, name_len) == 0) {
                    vector_push(v, c);
                }                 
            }
            map_get(check, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(check, key_id(c), c);
                vector_push(raw->vnodes, c);
            }
            j++;
            vector_get(rn->children, j, &c);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    vector_push_vector(raw->vnodes, v);
    release(set);
    release(check);
    release(v);
}

static void __get_all_children_recursive(struct xml_query *raw)
{
	id set, check, v, n, c, t;
	int i, j;
    struct xml_node *rn;

    vector_clear(raw->vattrs);
    map_new(&set);
    map_new(&check);
    vector_new(&v);

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        map_get(check, key_id(n), &t);
        if (!id_validate(t)) {
            map_set(check, key_id(n), n);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    i = 0;
    vector_get(raw->vnodes, i, &n);
    while (id_validate(n)) {
        fetch(n, &rn);
        j = 0;
        vector_get(rn->children, j, &c);
        while(id_validate(c)) {
            map_get(set, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(set, key_id(c), c);
                vector_push(v, c);              
            }
            map_get(check, key_id(c), &t);
            if (!id_validate(t)) {
                map_set(check, key_id(c), c);
                vector_push(raw->vnodes, c);
            }
            j++;
            vector_get(rn->children, j, &c);
        }
        i++;
        vector_get(raw->vnodes, i, &n);
    }

    vector_clear(raw->vnodes);
    vector_push_vector(raw->vnodes, v);
    release(set);
    release(check);
    release(v);
}

void xml_query_search(id pid, id nid, const char *command)
{
    struct xml_query *raw;
    const char *start, *end, *mid, *ptr;
	const char *name[2], *value[2];
	int sub;
	unsigned char recursive;
    unsigned len;
    id n;

    fetch(pid, &raw);
    assert(raw != NULL);
    vector_clear(raw->vnodes);
    vector_clear(raw->vattrs);
	ptr = command;
	recursive = 0;
    vector_push(raw->vnodes, nid);
block:
	__block_find(ptr, &start, &end);
	sub = end - start;
    
	switch (sub) {
		case -1:
			if(*start == '/' && *end == '/') {
				recursive = 1;
				ptr = end + 1;
				goto block;
			} else {
                vector_get_size(raw->vnodes, &len);
                if (len == 1) {
                    vector_get(raw->vnodes, 0, &n);
                    if (id_equal(n, nid)) {
                        vector_clear(raw->vnodes);
                    }
                }
				goto finish;
			}
		case 0:
			switch (*start) {
				case '*':
					if(recursive) {
						__get_all_children_recursive(raw);
					} else {
						__get_all_children(raw);
					}
					recursive = 0;
					ptr = end + 1;
					goto block;
				case '.':
					recursive = 0;
					ptr = end + 1;
					goto block;
				case '/':
					recursive = 1;

					ptr = end;
					goto block;
				case '@':
                    vector_clear(raw->vnodes);
                    vector_clear(raw->vattrs);
					goto finish;
				default:
					goto detail_case;
			}
		case 1:
			if(memcmp(start, "..", 2) == 0) {
				recursive = 0;
                __move_to_parent(raw);

				ptr = end + 1;
				goto block;
			} else if(memcmp(start, "@*", 2) == 0) {
				if(recursive) {
                    __get_all_children_recursive(raw);
				}
                __fetch_all_attribute(raw);
				goto finish;
			} else {
				goto detail_case;
			}
		default:
			goto detail_case;
	}

detail_case:
	if(*start == '@') {
		if(*(end + 1) != '\0') {
            xml_query_clear(raw);
		} else {
			if(recursive) {
                __get_all_children_recursive(raw);
			}
            __fetch_attribute_name(raw, start + 1, end - start);
			goto finish;
		}
	} else {
		__block_name(start, end, &mid);
		if(*mid) {
			if(mid == start && *mid == '*') {
				if(recursive) {
                    __get_all_children_recursive(raw);
				} else {
                    __get_all_children(raw);
				}
			} else {
				if(recursive) {
                    __get_all_children_has_name_recursive(raw,
						start, mid - start + 1);
				} else {
                    __get_all_children_has_name(raw,
						start, mid - start + 1);
				}

			}

			mid++;
		filter_attribute:
			__block_attribute(mid, end, &name[0], &name[1],
				&value[0], &value[1]);
			if(*name[0]) {
				if(name[1] == name[0] && *name[0] == '*') {
					if(value[0] <= value[1]) {
                        __keep_node_has_at_least_one_attribute_with_value(raw,
							value[0], value[1] - value[0] + 1);
					} else {
                        __keep_node_has_at_least_one_attribute(raw);
					}
				} else {
					if(value[0] <= value[1]) {
                        __keep_node_has_attribute_with_value(
							raw,
							name[0], name[1] - name[0] + 1,
							value[0], value[1] - value[0] + 1
						);
					} else {
                        __keep_node_has_attribute(raw,
							name[0], name[1] - name[0] + 1);
					}
				}
				mid = value[1] + 2;
				goto filter_attribute;
			}
		}
		recursive = 0;
	}
	ptr = end + 1;
	goto block;

finish:
	;
}