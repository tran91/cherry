#ifndef __bind_h
#define __bind_h

#include "id.h"

type(bind);

/*
 * @pid: bind
 * 
 * example:
 *      struct your_data {
 *          int *some_thing;
 *          ...
 *      };
 *      static void your_data_init(struct your_data *p)
 *      {
 *          p->some_thing = malloc(sizeof(int));
 *          ...
 *      }
 *      static void your_data_clear(struct your_data *p)
 *      {
 *          if (p->some_thing) {
 *              free(p->some_thing);
 *              p->some_thing = NULL;
 *          }
 *          ...
 *      }
 * 
 *      ...
 * 
 *      id bid;
 *      bind_new(&bid);
 *      bind_set(bid, sizeof(struct your_data), your_data_init, your_data_clear);
 */
void bind_set(id pid, unsigned int size, void(*pinit)(void*), void(*pclear)(void*));

/*
 * @pid: bind
 * 
 * example:
 *      struct your_data d;
 *      id bid;
 *      bind_new(&bid);
 *      bind_set_raw(bid, &d, sizeof(struct your_data));
 */
void bind_set_raw(id pid, void *copy, unsigned int size);

/*
 * @pid: bind
 * 
 * example:
 *      struct your_data *str;
 *      bind_get(pid, &str);
 */
void bind_get(id pid, void *d);

#endif