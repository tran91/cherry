#ifndef __map_h
#define __map_h

#include "id.h"

/*
 * map does no retaining id inside key.
 * 
 */
void map_shared(key k, id *map);
void map_new(id *map);

void map_get_size(id map, unsigned *size);

/*
 * @map: map
 * 
 * example:
 *      id map;
 *      map_new(&map);
 * 
 *      ...
 * 
 *      id obj;
 * 
 *      ...
 * 
 *      map_set(map, key_literal("user"), obj);
 */
void map_set(id map, key k, id object);

/*
 * @map: map
 * 
 * example:
 *      id map;
 *      map_new(&map);
 * 
 *      ...
 * 
 *      id obj;
 * 
 *      ...
 * 
 *      map_get(map, key_literal("user"), &obj);
 */
void map_get(id map, key k, id *object);

/*
 * @map: map
 * 
 * example:
 *      id map;
 *      map_new(&map);
 * 
 *      ...
 * 
 *      map_remove(map, key_literal("user"));
 */
void map_remove(id map, key k);

/*
 * @map: map
 * 
 * example:
 *      id map;
 *      map_new(&map);
 * 
 *      ...
 * 
 *      unsigned index;
 *      key k;
 *      id obj;
 *      
 *      index = 0;
 *      map_iterate(map, index, &k, &obj);
 *      while (id_validate(obj)) {
 *          ...
 *          index++;
 *          map_iterate(map, index, &k, &obj);
 *      }
 */
void map_iterate(id map, unsigned index, key *k, id *object);

/*
 * @map: map
 * 
 * example:
 *      id map;
 *      map_new(&map);
 * 
 *      ...
 * 
 *      map_clear(map);
 */
void map_clear(id map);

#endif