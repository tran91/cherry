#include "json.h"
#include "types/vector.h"
#include "types/cmath.h"
#include "types/buffer.h"
#include "utils/utf8.h"

enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL
};

struct json_element
{
    unsigned char type;
    id any;
};
make_type(json_element);

static void json_element_init(struct json_element *p, key k)
{
    p->type = JSON_NULL;
    p->any = id_null;
}

static void json_element_clear(struct json_element *p)
{
    p->type = JSON_NULL;
    release(p->any);
    p->any = id_null;
}

void json_element_make_object(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_OBJECT) return;

    release(rje->any);
    map_new(&rje->any);
    rje->type = JSON_OBJECT;
}

void json_element_make_array(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_ARRAY) return;

    release(rje->any);
    vector_new(&rje->any);
    rje->type = JSON_ARRAY;
}

void json_element_make_string(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_STRING) return;

    release(rje->any);
    buffer_new(&rje->any);
    rje->type = JSON_STRING;
}

void json_element_make_number(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_NUMBER) return;

    release(rje->any);
    number_new(&rje->any);
    rje->type = JSON_NUMBER;
}

void json_element_make_boolean(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_BOOLEAN) return;

    release(rje->any);
    number_new(&rje->any);
    rje->type = JSON_BOOLEAN;
}

void json_element_make_null(id pid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    if (rje->type == JSON_NULL) return;

    release(rje->any);
    rje->any = id_null;
    rje->type = JSON_NULL;
}

void json_object_add(id pid, const char *name, id element)
{
    struct json_element *rje;

    /* element must be a json_element */
    json_element_fetch(element, &rje);
    assert(rje != NULL);

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_OBJECT);

    map_set(rje->any, key_chars(name), element);
}

void json_object_get(id pid, const char *name, id *eid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_OBJECT);

    map_get(rje->any, key_chars(name), eid);
}

void json_object_remove(id pid, const char *name)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_OBJECT);

    map_remove(rje->any, key_chars(name));
}

void json_array_add(id pid, id element)
{
    struct json_element *rje;

    /* element must be a json_element */
    json_element_fetch(element, &rje);
    assert(rje != NULL);

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_ARRAY);

    vector_push(rje->any, element);
}

void json_array_get(id pid, unsigned index, id *eid)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_ARRAY);

    vector_get(rje->any, index, eid);
}

void json_array_remove(id pid, unsigned index)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_ARRAY);

    vector_remove(rje->any, index);
}

void json_string_append(id pid, const char *ptr)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_STRING);

    buffer_append(rje->any, ptr, strlen(ptr));
}

void json_string_get_ptr(id pid, const char **ptr)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_STRING);

    buffer_get_ptr(rje->any, ptr);
}

void json_string_get_length(id pid, unsigned *len)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_STRING);

    buffer_get_length(rje->any, len);
}

void json_number_set(id pid, double n)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_NUMBER);

    number_set(rje->any, n);
}

void json_number_get(id pid, double *n)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_NUMBER);

    number_get(rje->any, n);
}

void json_boolean_set(id pid, unsigned char flag)
{
    struct json_element *rje;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_BOOLEAN);

    number_set(rje->any, (double)flag);
}

void json_boolean_get(id pid, unsigned char *flag)
{
    struct json_element *rje;
    double n;

    json_element_fetch(pid, &rje);
    assert(rje != NULL && rje->type == JSON_BOOLEAN);

    number_get(rje->any, &n);
    *flag = n > 0 ? 1 : 0;
}

void json_element_load_file(id pid, const char *path)
{
    id bid;
    const char *ptr;

    buffer_new(&bid);
    buffer_append_file(bid, path);
    buffer_get_ptr(bid, &ptr);
    json_element_load_string(pid, ptr);
    release(bid);
}

static void load_value(id pid, const char **ptr, unsigned char *err);
static void load_string(id pid, const char **ptr, unsigned char *err);

static void load_object(id pid, const char **ptr, unsigned char *err)
{
    const char *ctr = *ptr;
    unsigned char width;
    id name = id_null;
    id value = id_null;
    unsigned char state = 0;
    const char *str;

    json_element_make_object(pid);
    
    while (*ctr) {
        switch (state) {
        case 0:
            switch (*ctr)
            {
            case '\"':
                json_element_new(&name);
                ctr++;
                if (!(*ctr)) {
                    goto cancel;
                }
                load_string(name, &ctr, err);
                if (*err) {
                    release(name);
                    goto cancel;
                }    
                json_string_get_ptr(name, &str);
                state = 1;        
                goto next;
            case ',':
                break;
            case '}':
                ctr++;
                goto finish;
            default:
                if (!isspace(*ctr)) {                    
                    release(name);
                    goto cancel;
                }
                break;
            }
            break;
        case 1:
            switch (*ctr) {
            case ':':
                ctr++;
                if (!(*ctr)) {
                    goto cancel;
                }
                json_element_new(&value);
                load_value(value, &ctr, err);
                if (*err) {
                    release(name);
                    release(value);
                    goto cancel;
                }
                json_string_get_ptr(name, &str);
                json_object_add(pid, str, value);
                release(value);
                release(name);
                name = id_null;
                state = 0;
                goto next;
            default:
                if (!isspace(*ctr)) {
                    release(name);
                    goto cancel;
                }
                break;
            }
            break;
        }
        utf8_width(ctr, &width);
        ctr += width;
    next:
        ;
    }
cancel:
    *err = 1;

finish:
    *ptr = ctr;
}

static void load_array(id pid, const char **ptr, unsigned char *err)
{
    const char *ctr = *ptr;
    unsigned char width;
    id value = id_null;

    json_element_make_array(pid);
    
    while (*ctr) {
        if (!isspace(*ctr)) {
            switch (*ctr)
            {
            case ',':
                ctr++;
                if (!(*ctr)) {
                    goto cancel;
                }
                break;
            case ']':
                ctr++;
                goto finish;
            default:
                json_element_new(&value);
                load_value(value, &ctr, err);
                if (*err) {
                    release(value);
                    goto cancel;
                }
                json_array_add(pid, value);
                release(value);
                break;
            }            
        } else {
            utf8_width(ctr, &width);
            ctr += width;
        }   
    }
cancel:
    *err = 1;

finish:
    *ptr = ctr;
}

static void load_string(id pid, const char **ptr, unsigned char *err)
{
    struct json_element *rje;
    const char *ctr = *ptr;
    unsigned char width;
    int i;

    json_element_make_string(pid);

    while (*ctr) {
        switch (*ctr) {
            case '\\':
                ctr++;
                if (!(*ctr)) {
                    goto cancel;
                }
                switch (*ctr) {
                    case '\"': case '\\': case '/': case 'b': case 'f': case 'n': case 'r': case 't':
                        ctr++;
                        if (!(*ctr)) {
                            goto cancel;
                        }
                        goto next;
                    case 'u':
                        for (i = 0; i < 5; ++i) {
                            ctr++;
                            if (!(*ctr)) {
                                goto cancel;
                            }
                        }
                        goto next;
                    default:
                        goto cancel;
                }
                break;
            case '\"':
                json_element_fetch(pid, &rje);
                buffer_append(rje->any, *ptr, ctr - (*ptr));
                ctr++;
                goto finish;
            default:
                if (iscntrl(*ctr)) {
                    goto cancel;
                }
                break;
        }
        utf8_width(ctr, &width);
        ctr += width;
    next:
        ;
    }
cancel:
    *err = 1;

finish:
    *ptr = ctr;
}

static void load_number(id pid, const char **ptr, unsigned char *err)
{
    const char *ctr = *ptr;
    unsigned char width;
    char *end;
    char buf[256];
    double f;

    json_element_make_number(pid);

    while (*ctr) {
        switch (*ctr)
        {
        case ',': case '}': case ']':
            strncpy(buf, *ptr, ctr - (*ptr));
            f = strtof(buf, &end);
            if (buf == end) {
                goto cancel;
            }
            json_number_set(pid, f);
            goto finish;
        default:
            if (isspace(*ctr)) {
                strncpy(buf, *ptr, ctr - (*ptr));
                f = strtof(buf, &end);
                if (buf == end) {
                    goto cancel;
                }
                json_number_set(pid, f);
                goto finish;
            }
            break;
        }
        utf8_width(ctr, &width);
        ctr += width;
    }
cancel:
    *err = 1;

finish:
    *ptr = ctr;
}

static void load_value(id pid, const char **ptr, unsigned char *err)
{
    const char *ctr = *ptr;
    unsigned char width;

    while(*ctr) {
        switch (*ctr)
        {
        case '{':
            ctr++;
            if (!(*ctr)) goto cancel;
            load_object(pid, &ctr, err);
            goto finish;
        case '[':
            ctr++;
            if (!(*ctr)) goto cancel;
            load_array(pid, &ctr, err);
            goto finish;
        case '\"':
            ctr++;
            if (!(*ctr)) goto cancel;
            load_string(pid, &ctr, err);
            goto finish;
        default:
            if (!isspace(*ctr)) {
                if (strncmp(ctr, "true", 4) == 0) {
                    json_element_make_boolean(pid);
                    json_boolean_set(pid, 1);
                    ctr += 4;
                    goto finish;
                } else if (strncmp(ctr, "false", 5) == 0) {
                    json_element_make_boolean(pid);
                    json_boolean_set(pid, 0);
                    ctr += 5;
                    goto finish;
                } else if (strncmp(ctr, "null", 4) == 0) {
                    json_element_make_null(pid);
                    ctr += 4;
                    goto finish;
                } else {
                    json_element_make_number(pid);
                    load_number(pid, &ctr, err);
                    goto finish;
                }
            }
            break;
        }
        utf8_width(ctr, &width);
        ctr += width;
    }
cancel:
    *err = 1;

finish:
    *ptr = ctr;
}

void json_element_load_string(id pid, const char *ptr)
{
    struct json_element *rje;
    unsigned char err;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    release(rje->any);
    rje->type = JSON_NULL;
    rje->any = id_null;

    const char *ctr = ptr;
    load_value(pid, &ctr, &err);
    if (err) {
        json_element_make_null(pid);
    }
}

static void __dump(id pid, unsigned stack)
{
    struct json_element *rje;
    key k;
    id obj;
    unsigned index, index2;
    const char *str;
    double f;

    json_element_fetch(pid, &rje);
    assert(rje != NULL);

    switch (rje->type)
    {
    case JSON_OBJECT:
        debug("{\n");
        stack++;        
        index = 0;
        map_iterate(rje->any, index, &k, &obj);
        while (id_validate(obj)) {
            if (index > 0) {
                debug(",\n");
            }
            for (index2 = 0; index2 < stack; ++index2) {
                debug("\t");
            }
            debug("%s : ", k.ptr);
            __dump(obj, stack);
            index++;
            map_iterate(rje->any, index, &k, &obj);
        }
        stack--;
        debug("\n");
        for (index = 0; index < stack; ++index) {
            debug("\t");
        }
        debug("}");
        break;
    case JSON_ARRAY:
        debug("[\n");
        stack++;        
        index = 0;
        vector_get(rje->any, index, &obj);
        while (id_validate(obj)) {
            if (index > 0) {
                debug(",\n");
            }
            for (index2 = 0; index2 < stack; ++index2) {
                debug("\t");
            }
            __dump(obj, stack);
            index++;
            vector_get(rje->any, index, &obj);
        }
        stack--;
        debug("\n");
        for (index = 0; index < stack; ++index) {
            debug("\t");
        }
        debug("]");
        break;
    case JSON_STRING:
        buffer_get_ptr(rje->any, &str);
        debug("\"%s\"", str);
        break;
    case JSON_NUMBER:
        number_get(rje->any, &f);
        debug("%f", f);
        break;
    case JSON_BOOLEAN:
        number_get(rje->any, &f);
        if (f > 0) {
            debug("true");
        } else {
            debug("false");
        }
        break;
    case JSON_NULL:
        debug("null");
        break;
    }
}

void json_element_dump(id pid)
{
    __dump(pid, 0);
    debug("\n");
}