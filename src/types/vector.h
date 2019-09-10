#ifndef __vector_h
#define __vector_h

#include "id.h"

type(vector);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      id obj;
 * 
 *      ...
 * 
 *      vector_push(v, obj);
 */
void vector_push(id pid, id cid);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      id v2;
 *      vector_new(&v2);
 * 
 *      ...
 * 
 *      vector_push_vector(v, v2);
 */
void vector_push_vector(id pid, id cid);

void vector_set(id pid, unsigned index, id cid);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      id obj;
 *      vector_get(v, 0, &obj);
 *      if (id_validate(obj)) {
 *          ...
 *      } else {
 *          ...
 *      }
 */
void vector_get(id pid, unsigned index, id *cid);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      vector_remove(v, 0);
 */
void vector_remove(id pid, unsigned index);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      id obj;
 *      ...
 *      vector_push(v, obj);
 *      
 *      ...
 * 
 *      vector_remove_id(v, obj);
 */
void vector_remove_id(id pid, id obj);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      vector_swap(v, 0, 2);
 */
void vector_swap(id pid, unsigned idx1, unsigned idx2);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      vector_bring_to_back(v, 0);
 */
void vector_bring_to_back(id pid, unsigned index);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      vector_bring_to_front(v, 9);
 */
void vector_bring_to_front(id pid, unsigned index);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      unsigned s;
 *      vector_get_size(v, &s);
 */
void vector_get_size(id pid, unsigned *s);

/*
 * @pid: vector
 * 
 * example:
 *      id v;
 *      vector_new(&v);
 * 
 *      ...
 * 
 *      vector_clear(v);
 */
void vector_clear(id pid);

#endif