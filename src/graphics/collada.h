#ifndef __collada_h
#define __collada_h

#include "types/id.h"

type(collada_context);
type(collada_node);
type(collada_mesh);
type(collada_skin);
type(collada_amature);
type(collada_bone);

/*
 * @pid: collada_context
 * 
 * example:
 *      id ctx;
 *      collada_context_new(&ctx);
 *      collada_context_load_file(ctx, "inner://res/model/cube.dae");
 */
void collada_context_load_file(id pid, const char *path);

/*
 * @pid: collada_context
 * @nid: collada_node
 * 
 * example:
 *      id ctx;
 *      id node;
 * 
 *      collada_context_get_node(ctx, "Cube", &node);
 */
void collada_context_get_node(id pid, const char *name, id *nid);

/*
 * @pid: collada_node
 *
 * example:
 *      id node;
 *      const char *fptr;
 *      unsigned int flen;
 * 
 *      collada_node_get_vertice(node, &fptr, &flen);
 */
void collada_node_get_vertice(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 *
 * example:
 *      id node;
 *      const char *fptr;
 *      unsigned int flen;
 * 
 *      collada_node_get_normals(node, &fptr, &flen);
 */
void collada_node_get_normals(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 * 
 * example:
 *      id node;
 *      const char *fptr;
 *      unsigned int flen;
 * 
 *      collada_node_get_texcoords(node, &fptr, &flen);
 */
void collada_node_get_texcoords(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 * 
 * example:
 *      id node;
 *      const char *fptr;
 *      unsigned int flen;
 * 
 *      collada_node_get_colors(node, &fptr, &flen);
 */
void collada_node_get_colors(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 * @nid: mat4
 * 
 * example:
 *      id node;
 *      id m;
 *      
 *      collada_node_get_transform(node, m);
 */
void collada_node_get_transform(id pid, id mid);

/*
 * @pid: collada_node
 */
void collada_node_get_bones_id(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 */
void collada_node_get_weights(id pid, const float **ptr, unsigned int *len);

/*
 * @pid: collada_node
 */
void collada_node_get_bones_per_vertex(id pid, unsigned int *v);

/*
 * @pid: collada_node
 */
void collada_node_get_bones_upload(id pid, unsigned int *v);

/*
 * @pid: collada_node
 * @vid: vector
 */
void collada_node_get_inverse_bind_poses(id pid, id *vid);

void collada_node_get_aabb(id pid, float *min_x, float *min_y, float *min_z, float *max_x, float *max_y, float *max_z);

#endif