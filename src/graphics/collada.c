#include "collada.h"
#include "types/vector.h"
#include "types/map.h"
#include "types/buffer.h"
#include "types/cmath.h"
#include "xml/xml.h"

struct collada_context
{
    id meshs;
    id amatures;
    id skins;
    id nodes;
};
make_type(collada_context);

static void collada_context_init(struct collada_context *p, key k)
{
    map_new(&p->meshs);
    map_new(&p->amatures);
    map_new(&p->skins);
    map_new(&p->nodes);
}

static void collada_context_clear(struct collada_context *p)
{
    release(p->meshs);
    release(p->amatures);
    release(p->skins);
    release(p->nodes);
}

struct collada_mesh
{
    id vertices;
    id normals;
    id texcoords;
    id colors;
    id indice;
    id id;
};
make_type(collada_mesh);

static void collada_mesh_init(struct collada_mesh *p, key k)
{
    buffer_new(&p->vertices);
    buffer_new(&p->normals);
    buffer_new(&p->texcoords);
    buffer_new(&p->colors);
    buffer_new(&p->indice);
    buffer_new(&p->id);
}

static void collada_mesh_clear(struct collada_mesh *p)
{
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->indice);
    release(p->id);
}

struct collada_skin
{
    id vertices;
    id normals;
    id texcoords;
    id colors;
    id bones_id;
    id weights;
    id inverse_bind_poses;
    id bone_names;
    id id;

    unsigned bones_per_vertex;
    unsigned bones_upload;

    id transform;
};
make_type(collada_skin);

static void collada_skin_init(struct collada_skin *p, key k)
{
    p->vertices = id_null;
    p->normals = id_null;
    p->texcoords = id_null;
    p->colors = id_null;
    
    vector_new(&p->inverse_bind_poses);
    buffer_new(&p->id);
    mat4_new(&p->transform);
    vector_new(&p->bone_names);
    buffer_new(&p->bones_id);
    buffer_new(&p->weights);

    p->bones_per_vertex = 0;
    p->bones_upload = 0;
}

static void collada_skin_clear(struct collada_skin *p)
{
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->bones_id);
    release(p->weights);
    release(p->inverse_bind_poses);
    release(p->bone_names);
    release(p->id);
    release(p->transform);
}

struct collada_bone
{
    id children;
    id parent;
    id transform;
    id transform_relative;
    id id;
    id sid;
    id animation_times;
    id animation_transforms;
};
make_type(collada_bone);

static void collada_bone_init(struct collada_bone *p, key k)
{
    vector_new(&p->children);
    p->parent = id_null;
    mat4_new(&p->transform);
    mat4_new(&p->transform_relative);
    buffer_new(&p->id);
    buffer_new(&p->sid);
    buffer_new(&p->animation_times);
    vector_new(&p->animation_transforms);
}

static void collada_bone_clear(struct collada_bone *p)
{
    release(p->children);
    p->parent = id_null;
    release(p->transform);
    release(p->transform_relative);
    release(p->id);
    release(p->sid);
    release(p->animation_times);
    release(p->animation_transforms);
}

static void collada_bone_add_child(id pid, id cid)
{
    struct collada_bone *p, *c, *op;

    collada_bone_fetch(pid, &p);
    collada_bone_fetch(cid, &c);
    assert(p != NULL && c != NULL);

    retain(cid);
    collada_bone_fetch(c->parent, &op);
    if (op) {
        vector_remove_id(op->children, cid);
    }
    c->parent = pid;
    vector_push(p->children, cid);
    release(cid);
}

struct collada_amature
{
    id transform;
    id bones;
    id id;
};
make_type(collada_amature);

static void collada_amature_init(struct collada_amature *p, key k)
{
    mat4_new(&p->transform);
    map_new(&p->bones);
    buffer_new(&p->id);
}

static void collada_amature_clear(struct collada_amature *p)
{
    release(p->transform);
    release(p->id);
    release(p->bones);   
}

static void collada_amature_add_bone(id pid, id bid)
{
    struct collada_amature *p;
    struct collada_bone *b;
    const char *ptr;
    unsigned len;

    collada_amature_fetch(pid, &p);
    collada_bone_fetch(bid, &b);
    assert(p != NULL && b != NULL);

    buffer_get_ptr(b->id, &ptr);
    buffer_get_length(b->id, &len);
    map_set(p->bones, key_mem(ptr, len), bid);

    buffer_get_ptr(b->sid, &ptr);
    buffer_get_length(b->sid, &len);
    map_set(p->bones, key_mem(ptr, len), bid);
}

struct collada_node
{
    id children;
    id parent;

    id amature;

    id vertices;
    id normals;
    id texcoords;
    id colors;
    id bones_id;
    id weights;
    id inverse_bind_poses;
    id bone_names;
    id id;

    unsigned bones_per_vertex;
    unsigned bones_upload;

    id transform;
};
make_type(collada_node);

static void collada_node_init(struct collada_node *p, key k)
{
    vector_new(&p->children);
    p->parent = id_null;

    p->amature = id_null;

    p->vertices = id_null;
    p->normals = id_null;
    p->texcoords = id_null;
    p->colors = id_null;
    p->bones_id = id_null;
    p->weights = id_null;
    p->inverse_bind_poses = id_null;
    p->bone_names = id_null;
    
    buffer_new(&p->id);
    mat4_new(&p->transform);

    p->bones_per_vertex = 0;
    p->bones_upload = 0;
}

static void collada_node_clear(struct collada_node *p)
{
    release(p->children);
    release(p->amature);
    release(p->vertices);
    release(p->normals);
    release(p->texcoords);
    release(p->colors);
    release(p->bones_id);
    release(p->weights);
    release(p->inverse_bind_poses);
    release(p->bone_names);
    release(p->id);
    release(p->transform);
}

static void collada_node_add_child(id pid, id cid)
{
    struct collada_node *p, *c, *op;

    collada_node_fetch(pid, &p);
    collada_node_fetch(cid, &c);
    assert(p != NULL && c != NULL);

    retain(cid);
    collada_node_fetch(c->parent, &op);
    if (op) {
        vector_remove_id(op->children, cid);
    }
    c->parent = pid;
    vector_push(p->children, cid);
    release(cid);
}

/*
 * parser geometry
 */
static void __parse_geometry(struct collada_context *p, id xn_geometry)
{
    struct collada_mesh *raw_mesh;
    id mesh;
	id attr_id, attr_count, attr_semantic, attr_offset;
	id xn_float_array, xn_p, xn_triangles, xn_input;
    id query;
    id positions, texcoords, normals, colors, ps;
	int i, j;
	int positions_offset = 0, normals_offset = 0, texcoords_offset = 0, colors_offset = 0;
    const int *ps_ptr;
    unsigned ps_len;
    const float *float_ptr;
    const char *ptr;
    unsigned ptr_len;
    float max_f;
    int max_i;
    unsigned count1, count2, inputs;

    /* init local buffers */
    buffer_new(&positions);
    buffer_new(&texcoords);
    buffer_new(&normals);
    buffer_new(&colors);
    buffer_new(&ps);
    xml_query_new(&query);

    /* insert new mesh */
    collada_mesh_new(&mesh);
    collada_mesh_fetch(mesh, &raw_mesh);
    xml_node_get_attribute_by_name(xn_geometry, "id", &attr_id);
    xml_attribute_get_value(attr_id, &ptr, &ptr_len);
    buffer_append(raw_mesh->id, ptr, ptr_len);
    map_set(p->meshs, key_mem(ptr, ptr_len), mesh);
    release(mesh);

    /* find attribute float_arrays */
    xml_query_search(query, xn_geometry, "//float_array");
    i = 0;
    xml_query_get_node(query, i, &xn_float_array);
    while(id_validate(xn_float_array)) {
        xml_node_get_attribute_by_name(xn_float_array, "count", &attr_count);
        xml_node_get_attribute_by_name(xn_float_array, "id", &attr_id);
        xml_attribute_get_value(attr_count, &ptr, &ptr_len);
        count1 = atoi(ptr);

        xml_attribute_get_value(attr_id, &ptr, &ptr_len);
        if (strstr(ptr, "positions-array")) {
            buffer_reserve(positions, count1 * sizeof(float));
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);
            buffer_append_float_string(positions, ptr, &max_f);
        } else if(strstr(ptr, "normals-array")) {
            buffer_reserve(normals, count1 * sizeof(float));
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);
            buffer_append_float_string(normals, ptr, &max_f);
        } else if(strstr(ptr, "map-0-array")) {
            buffer_reserve(texcoords, count1 * sizeof(float));
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);
            buffer_append_float_string(texcoords, ptr, &max_f);
        } else if(strstr(ptr, "colors-Col-array")) {
            buffer_reserve(colors, count1 * sizeof(float));
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);
            buffer_append_float_string(colors, ptr, &max_f);
        }
        i++;
        xml_query_get_node(query, i, &xn_float_array);
    }

    xml_query_search(query, xn_geometry, "//triangles");
    xml_query_get_node(query, 0, &xn_triangles);
    xml_node_get_attribute_by_name(xn_triangles, "count", &attr_count);
    xml_attribute_get_value(attr_count, &ptr, &ptr_len);
    count1 = atoi(ptr);

    xml_query_search(query, xn_geometry, "//p");
    xml_query_get_node(query, 0, &xn_p);

    xml_query_search(query, xn_triangles, "/input");
    xml_query_get_node_length(query, &count2);

    buffer_reserve(ps, count1 * count2 * 3 * sizeof(int));
    xml_node_get_value(xn_p, &ptr, &ptr_len);
    buffer_append_int_string(ps, ptr, &max_i);

    /* find offsets */
	positions_offset = normals_offset = texcoords_offset = colors_offset = -1;
    inputs = 0;
    xml_query_get_node(query, inputs, &xn_input);
    while(id_validate(xn_input)) {
        xml_node_get_attribute_by_name(xn_input, "semantic", &attr_semantic);
        xml_node_get_attribute_by_name(xn_input, "offset", &attr_offset);
        xml_attribute_get_value(attr_semantic, &ptr, &ptr_len);
        if (strstr(ptr, "VERTEX")) {
            xml_attribute_get_value(attr_offset, &ptr, &ptr_len);
            positions_offset = atoi(ptr);
        } else if (strstr(ptr, "NORMAL")) {
            xml_attribute_get_value(attr_offset, &ptr, &ptr_len);
            normals_offset = atoi(ptr);
        } else if (strstr(ptr, "TEXCOORD")) {
            xml_attribute_get_value(attr_offset, &ptr, &ptr_len);
            texcoords_offset = atoi(ptr);
        } else if (strstr(ptr, "COLOR")) {
            xml_attribute_get_value(attr_offset, &ptr, &ptr_len);
            colors_offset = atoi(ptr);
        }
        inputs++;
        xml_query_get_node(query, inputs, &xn_input);
    }

    /* reserve mesh data memory */
    buffer_get_length(ps, &ps_len);
    buffer_get_ptr(ps, &ps_ptr);
    ps_len /= sizeof(int);

	if(positions_offset >= 0) {
        buffer_reserve(raw_mesh->vertices, (ps_len / inputs) * 3);
        buffer_reserve(raw_mesh->indice, ps_len / inputs);
	}
	if(normals_offset >= 0) {
        buffer_reserve(raw_mesh->normals, (ps_len/ inputs) * 3);
	}
	if(texcoords_offset >= 0) {
        buffer_reserve(raw_mesh->texcoords, (ps_len / inputs) * 2);
	}
    buffer_reserve(raw_mesh->colors, (ps_len / inputs) * 3);

	for(i = 0; i < ps_len; i += inputs) {
		if(positions_offset >= 0) {
			j = ps_ptr[i + positions_offset];
            buffer_append(raw_mesh->indice, &j, sizeof(j));

            buffer_get_ptr(positions, &float_ptr);
			j *= 3;
            buffer_append(raw_mesh->vertices, float_ptr + j, sizeof(float) * 3);
		}
		if(normals_offset >= 0) {
			j = ps_ptr[i + normals_offset];

            buffer_get_ptr(normals, &float_ptr);
			j *= 3;
            buffer_append(raw_mesh->normals, float_ptr + j, sizeof(float) * 3);
		}
		if(texcoords_offset >= 0) {
			j = ps_ptr[i + texcoords_offset];

            buffer_get_ptr(texcoords, &float_ptr);
			j *= 2;
            buffer_append(raw_mesh->texcoords, float_ptr + j, sizeof(float) * 2);
		}
		if(colors_offset >= 0) {
			j = ps_ptr[i + colors_offset];

            buffer_get_ptr(colors, &float_ptr);
			j *= 3;
            buffer_append(raw_mesh->colors, float_ptr + j, sizeof(float) * 3);
		} else {
            static float default_color[3] = {
                1.0f, 1.0f, 1.0f
            };
            buffer_append(raw_mesh->colors, default_color, sizeof(default_color));
        }
	}

    release(ps);
    release(positions);
    release(texcoords);
    release(normals);
    release(colors);
    release(query);
}

static void __parse_library_geometries(struct collada_context *p, id xn)
{
    id query, xn_geometry;
    unsigned index;

    xml_query_new(&query);
    xml_query_search(query, xn, "//library_geometries/geometry");
    index = 0;
    xml_query_get_node(query, index, &xn_geometry);
    while (id_validate(xn_geometry)) {
        __parse_geometry(p, xn_geometry);
        index++;
        xml_query_get_node(query, index, &xn_geometry);
    }
    release(query);
}

/*
 * parse controller
 */
static void __parse_controller(struct collada_context *p, id xn_controller)
{
    struct collada_skin *rskin;
    struct collada_mesh *rmesh;
    id skin, mesh, query;
    id weights, v_bones_id, v_weights;
    id vcount, v;
    id attr_id, attr_source, attr_count;
    id xn_skin, xn_bind_shape_matrix, xn_name_array, xn_float_array, xn_vcount, xn_v, xn_vertex_weights;
    id m;
    const char *start, *end;
    id temp;
    int i, j, vcount_max, ii, vcount_i;
    unsigned count;
    const char *ptr;
    unsigned ptr_len;
    const float *ptr_float;
    unsigned ptr_float_len;
    const int *ptr_int;
    unsigned ptr_int_len;
    const int *ptr2_int;
    unsigned ptr2_int_len;
    float max_f, f;

    buffer_new(&weights);
    buffer_new(&v_bones_id);
    buffer_new(&v_weights);
    buffer_new(&vcount);
    buffer_new(&v);
    xml_query_new(&query);

    /* find skin id */
    xml_node_get_attribute_by_name(xn_controller, "id", &attr_id);
    xml_attribute_get_value(attr_id, &ptr, &ptr_len);

    /* add skin to context */
    collada_skin_new(&skin);
    collada_skin_fetch(skin, &rskin);
    buffer_append(rskin->id, ptr, ptr_len);
    map_set(p->skins, key_mem(ptr, ptr_len), skin);
    release(skin);

    /* find mesh */
    xml_query_search(query, xn_controller, "/skin");
    xml_query_get_node(query, 0, &xn_skin);
    xml_node_get_attribute_by_name(xn_skin, "source", &attr_source);
    xml_attribute_get_value(attr_source, &ptr, &ptr_len);

    /* apply mesh's vertice, normals, texcoords, colors to skin */
    map_get(p->meshs, key_mem(ptr + 1, ptr_len - 1), &mesh);
    collada_mesh_fetch(mesh, &rmesh);
    assign(rskin->vertices, rmesh->vertices);
    assign(rskin->normals, rmesh->normals);
    assign(rskin->texcoords, rmesh->texcoords);
    assign(rskin->colors, rmesh->colors);

    /* find skin transform */
    xml_query_search(query, xn_skin, "/bind_shape_matrix");
    xml_query_get_node(query, 0, &xn_bind_shape_matrix);
    xml_node_get_value(xn_bind_shape_matrix, &ptr, &ptr_len);
    mat4_load_string(rskin->transform, ptr);
    mat4_transpose(rskin->transform);

    /* find skin bones name */
    xml_query_search(query, xn_skin, "//Name_array");
    xml_query_get_node(query, 0, &xn_name_array);
    xml_node_get_value(xn_name_array, &start, &ptr_len);
	end = start;
	while(*start) {
		if(isspace(*start)) {
			start++;
			end = start;
		} else {
			switch (*end) {
				case ' ': case '\t': case '\n':
				case '\v': case '\f': case '\r':
				case '\0':
					if(end > start) {
                        buffer_new(&temp);
                        buffer_append(temp, start, end - start);
                        vector_push(rskin->bone_names, temp);
                        release(temp);
                        temp = id_null;
					}
					start = end;
					break;
				default:
					end++;
					break;

			}
		}
	}

    /*  */
    xml_query_search(query, xn_skin, "//float_array");
    i = 0;
    xml_query_get_node(query, i, &xn_float_array);
    while (id_validate(xn_float_array)) {
        xml_node_get_attribute_by_name(xn_float_array, "id", &attr_id);
        xml_node_get_attribute_by_name(xn_float_array, "count", &attr_count);
        xml_attribute_get_value(attr_count, &ptr, &ptr_len);
        count = atoi(ptr);        
        xml_attribute_get_value(attr_id, &ptr, &ptr_len);
        if (strstr(ptr, "bind_poses-array")) {
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);
            buffer_new(&temp);
            buffer_reserve(temp, count * sizeof(float));
            buffer_append_float_string(temp, ptr, &max_f);
            buffer_get_ptr(temp, &ptr_float);
            buffer_get_length(temp, &ptr_float_len);
            ptr_float_len /= sizeof(float);
            for (j = 0; j < ptr_float_len; j += 16) {
                mat4_new(&m);
                mat4_set_array(m, ptr_float + j);
                mat4_transpose(m);
                vector_push(rskin->inverse_bind_poses, m);
                release(m);
            }
            release(temp);
        } else if(strstr(ptr, "weights-array")) {
            xml_node_get_value(xn_float_array, &ptr, &ptr_len);

            buffer_reserve(weights, count * sizeof(float));
            buffer_append_float_string(weights, ptr, &max_f);
        }
        i++;
        xml_query_get_node(query, i, &xn_float_array);
    }

    xml_query_search(query, xn_skin, "//vertex_weights");
    xml_query_get_node(query, 0, &xn_vertex_weights);
    xml_node_get_attribute_by_name(xn_vertex_weights, "count", &attr_count);
    xml_attribute_get_value(attr_count, &ptr, &ptr_len);
    count = atoi(ptr);

    buffer_reserve(vcount, count * sizeof(int));
    buffer_reserve(v, count * sizeof(int) * 2);

    xml_query_search(query, xn_vertex_weights, "//vcount");
    xml_query_get_node(query, 0, &xn_vcount);

    xml_query_search(query, xn_vertex_weights, "//v");
    xml_query_get_node(query, 0, &xn_v);

    xml_node_get_value(xn_vcount, &ptr, &ptr_len);
    buffer_append_int_string(vcount, ptr, &vcount_max);
    
    xml_node_get_value(xn_v, &ptr, &ptr_len);
    buffer_append_int_string(v, ptr, NULL);

    buffer_reserve(v_bones_id, count * vcount_max * sizeof(float));
    buffer_reserve(v_weights, count * vcount_max * sizeof(float));

    buffer_get_ptr(vcount, &ptr_int);
    buffer_get_length(vcount, &ptr_int_len);
    ptr_int_len /= sizeof(int);

    buffer_get_ptr(v, &ptr2_int);
    buffer_get_length(v, &ptr2_int_len);
    ptr2_int_len /= sizeof(int);

	for(i = 0, ii = -1; i < ptr_int_len; ++i) {
		vcount_i = ptr_int[i];

		for(j = 0; j < vcount_i; ++j) {
			ii++;
            f = (float)ptr2_int[ii];
            buffer_append(v_bones_id, &f, sizeof(float));
			ii++;
            f = (float)ptr2_int[ii];
            buffer_append(v_weights, &f, sizeof(float));
		}

		while(j < vcount_max) {
            f = 0.0f;
            buffer_append(v_bones_id, &f, sizeof(float));
            buffer_append(v_weights, &f, sizeof(float));
			j++;
		}
	}

    rskin->bones_per_vertex = vcount_max;
	if(vcount_max <= 4) {
        rskin->bones_upload = 4;
	} else if(vcount_max <= 8) {
		rskin->bones_upload = 8;
	} else if(vcount_max <= 12) {
        rskin->bones_upload = 12;
	}

    buffer_get_length(rmesh->indice, &ptr_len);
    ptr_len /= sizeof(int);
    ptr_len *= rskin->bones_upload;
    buffer_reserve(rskin->bones_id, ptr_len * sizeof(float));
    buffer_reserve(rskin->weights, ptr_len * sizeof(float));

    buffer_get_ptr(rmesh->indice, &ptr_int);
    buffer_get_length(rmesh->indice, &ptr_len);
    ptr_len /= sizeof(int);

	for(i = 0; i < ptr_int_len; ++i) {
		ii = ptr_int[i];

		for(j = 0; j < rskin->bones_per_vertex; ++j) {
            buffer_get_ptr(v_bones_id, &ptr_float);
            f = ptr_float[ii * rskin->bones_per_vertex] + j;
            buffer_append(rskin->bones_id, &f, sizeof(float));

            buffer_get_ptr(v_weights, &ptr_float);
            f = ptr_float[ii * rskin->bones_per_vertex] + j;
            buffer_append(rskin->weights, &f, sizeof(float));
		}

		while(j < rskin->bones_upload) {
            f = 0.0f;
            buffer_append(rskin->bones_id, &f, sizeof(float));
            buffer_append(rskin->weights, &f, sizeof(float));
			j++;
		}
	}

    release(query);
    release(weights);
    release(v_bones_id);
    release(v_weights);
    release(vcount);
    release(v);
}

static void __parse_library_controllers(struct collada_context *p, id xn)
{
    id query, xn_controller;
    unsigned index;

    xml_query_new(&query);
    xml_query_search(query, xn, "//library_controllers/controller");
    index = 0;
    xml_query_get_node(query, index, &xn_controller);
    while (id_validate(xn_controller)) {
        __parse_controller(p, xn_controller);
        index++;
        xml_query_get_node(query, index, &xn_controller);
    }
    release(query);
}

/*
 * amature
 */
static void load_bone(id bone, id amature, id xn)
{       
    struct collada_bone *rb;
	id query;
    id bone_c;
	id attr_id, attr_sid;
	id xn_matrix, xn_node;
	int i;
    const char *ptr;
    unsigned ptr_len;

    collada_bone_fetch(bone, &rb);
    assert(rb != NULL);

    xml_query_new(&query);

    /* add bone to amature */
    xml_node_get_attribute_by_name(xn, "id", &attr_id);
    xml_attribute_get_value(attr_id, &ptr, &ptr_len);
    buffer_append(rb->id, ptr, ptr_len);
    xml_node_get_attribute_by_name(xn, "sid", &attr_sid);
    xml_attribute_get_value(attr_sid, &ptr, &ptr_len);
    buffer_append(rb->sid, ptr, ptr_len);
    collada_amature_add_bone(amature, bone);

    /* load bone property */
    xml_query_search(query, xn, "/matrix");
    xml_query_get_node(query, 0, &xn_matrix);
    if (id_validate(xn_matrix)) {
        xml_node_get_value(xn_matrix, &ptr, &ptr_len);
        mat4_load_string(rb->transform, ptr);
        mat4_transpose(rb->transform);
    }

    xml_query_search(query, xn, "/node");
    i = 0;
    xml_query_get_node(query, i, &xn_node);
    while (id_validate(xn_node)) {
        collada_bone_new(&bone_c);
        load_bone(bone_c, amature, xn_node);
        collada_bone_add_child(bone, bone_c);
        release(bone_c);
        i++;
        xml_query_get_node(query, i, &xn_node);
    }

    release(query);
}


static void __parse_node_amature(struct collada_context *p, id xn_node)
{
    struct collada_amature *ra;
    id amature, bone;
    id query;
	id xn_matrix, xn_bone;
	id attr_id;
	int i;
    const char *ptr;
    unsigned ptr_len;

    xml_query_new(&query);

    collada_amature_new(&amature);
    collada_amature_fetch(amature, &ra);

    xml_node_get_attribute_by_name(xn_node, "id", &attr_id);
    xml_attribute_get_value(attr_id, &ptr, &ptr_len);
    buffer_append(ra->id, ptr, ptr_len);
    map_set(p->amatures, key_mem(ptr, ptr_len), amature);
    release(amature);

    xml_query_search(query, xn_node, "/matrix");
    xml_query_get_node(query, 0, &xn_matrix);
    if (id_validate(xn_matrix)) {
        xml_node_get_value(xn_matrix, &ptr, &ptr_len);
        mat4_load_string(ra->transform, ptr);
        mat4_transpose(ra->transform);
    }

    xml_query_search(query, xn_node, "/node");
    i = 0;
    xml_query_get_node(query, i, &xn_bone);
    while (id_validate(xn_bone)) {
        xml_node_get_attribute_by_name(xn_bone, "id", &attr_id);
        xml_attribute_get_value(attr_id, &ptr, &ptr_len);
        map_set(p->amatures, key_mem(ptr, ptr_len), amature);

        collada_bone_new(&bone);
        load_bone(bone, amature, xn_bone);
        release(bone);

        i++;
        xml_query_get_node(query, i, &xn_bone);
    }

    release(query);
}

static void __parse_node_visual(id node, struct collada_context *p, id xn_node)
{
    struct collada_node *rnode;
    struct collada_mesh *rmesh;
    struct collada_skin *rskin;
    id node_c;
    id mesh = id_null, skin = id_null, amature = id_null;
	id query;
	id xn_instance, xn_skeleton, xn_matrix, xn_child;
	id attr_id, attr_url;
	int i;
    const char *ptr;
    unsigned ptr_len;

    xml_query_new(&query);
    collada_node_fetch(node, &rnode);
    assert(rnode != NULL);

    xml_node_get_attribute_by_name(xn_node, "id", &attr_id);
    xml_attribute_get_value(attr_id, &ptr, &ptr_len);
    buffer_append(rnode->id, ptr, ptr_len);
    map_set(p->nodes, key_mem(ptr, ptr_len), node);

    xml_query_search(query, xn_node, "/instance_controller");
    xml_query_get_node(query, 0, &xn_instance);
    if (id_validate(xn_instance)) {
        xml_node_get_attribute_by_name(xn_instance, "url", &attr_url);
        xml_attribute_get_value(attr_url, &ptr, &ptr_len);
        map_get(p->skins, key_mem(ptr + 1, ptr_len - 1), &skin);

        xml_query_search(query, xn_instance, "/skeleton");
        xml_query_get_node(query, 0, &xn_skeleton);
        xml_node_get_value(xn_skeleton, &ptr, &ptr_len);
        map_get(p->amatures, key_mem(ptr + 1, ptr_len - 1), &amature);
    } else {
        xml_query_search(query, xn_node, "/instance_geometry");
        xml_query_get_node(query, 0, &xn_instance);
        if (id_validate(xn_instance)) {
            xml_node_get_attribute_by_name(xn_instance, "url", &attr_url);
            xml_attribute_get_value(attr_url, &ptr, &ptr_len);
            map_get(p->meshs, key_mem(ptr + 1, ptr_len - 1), &mesh);
        }
    }

	if(id_validate(mesh)) {
        collada_mesh_fetch(mesh, &rmesh);

        assign(rnode->vertices, rmesh->vertices);
        assign(rnode->normals, rmesh->normals);
        assign(rnode->texcoords, rmesh->texcoords);
        assign(rnode->colors, rmesh->colors);

        xml_query_search(query, xn_node, "/matrix");
        xml_query_get_node(query, 0, &xn_matrix);
        if (id_validate(xn_matrix)) {
            xml_node_get_value(xn_matrix, &ptr, &ptr_len);
            mat4_load_string(rnode->transform, ptr);
            mat4_transpose(rnode->transform);
        }
	} else if(id_validate(skin) && id_validate(amature)) {
        collada_skin_fetch(skin, &rskin);
        
        assign(rnode->vertices, rskin->vertices);
        assign(rnode->normals, rskin->normals);
        assign(rnode->texcoords, rskin->texcoords);
        assign(rnode->colors, rskin->colors);
        assign(rnode->bones_id, rskin->bones_id);
        assign(rnode->weights, rskin->weights);
        assign(rnode->inverse_bind_poses, rskin->inverse_bind_poses);
        assign(rnode->bone_names, rskin->bone_names);
        rnode->bones_upload = rskin->bones_upload;
        rnode->bones_per_vertex = rskin->bones_per_vertex;

        assign(rnode->amature, amature);
	}

    xml_query_search(query, xn_node, "/node");
    i = 0;
    xml_query_get_node(query, i, &xn_child);
    while (id_validate(xn_child)) {
        collada_node_new(&node_c);
        __parse_node_visual(node_c, p, xn_child);
        collada_node_add_child(node, node_c);
        release(node_c);
        i++;
        xml_query_get_node(query, i, &xn_child);
    }

    release(query);
}

static void __parse_visual_scene(struct collada_context *p, id xn_visual_scene)
{
	id xn_node, query, sub_query, node;
	int i;
    unsigned len;

    xml_query_new(&query);
    xml_query_new(&sub_query);

    xml_query_search(query, xn_visual_scene, "/node");
    i = 0;
    xml_query_get_node(query, i, &xn_node);
    while (id_validate(xn_node)) {
        xml_query_search(sub_query, xn_node, "/node[@type=\"JOINT\"]");
        xml_query_get_node_length(sub_query, &len);
        if (len > 0) {
            __parse_node_amature(p, xn_node);
        } else {
            collada_node_new(&node);
            __parse_node_visual(node, p, xn_node);
            release(node);
        }
        i++;
        xml_query_get_node(query, i, &xn_node);
    }
 
    release(query);
    release(sub_query);
}

static void __parse_library_visual_scenes(struct collada_context *p, id xn)
{
	id query, xn_visual_scene;
	int i;

    xml_query_new(&query);

    xml_query_search(query, xn, "//library_visual_scenes/visual_scene");
    i = 0;
    xml_query_get_node(query, i, &xn_visual_scene);
    while (id_validate(xn_visual_scene)) {
        __parse_visual_scene(p, xn_visual_scene);
        i++;
        xml_query_get_node(query, i, &xn_visual_scene);
    }

	release(query);
}

/*
 * animation
 */
static void __animation_get_bone(struct collada_context *p,
	const char *bkey, const unsigned bkey_len, id *bone)
{
    struct collada_amature *ramature;
    id amature;
    unsigned index;
    key k;

    index = 0;
    map_iterate(p->amatures, index, &k, &amature);
    while(id_validate(amature)) {
        collada_amature_fetch(amature, &ramature);
        map_get(ramature->bones, key_mem(bkey, bkey_len), bone);
        if (id_validate(*bone)) {
            return;
        }
        index++;
        map_iterate(p->amatures, index, &k, &amature);
    }
}

static void __parse_animation(struct collada_context *p, id xn)
{
    struct collada_bone *rbone;
    id query;
	id xn_float_array;
	id attr_count, attr_target;
	unsigned count;
	const char *start, *end;
    const char *ptr;
    unsigned ptr_len;
    const float *float_ptr;
	id bone;
	id fa;
    id m;
	unsigned i;

    xml_query_new(&query);

    xml_query_search(query, xn, "//channel/@target");
    xml_query_get_attribute(query, 0, &attr_target);
    xml_attribute_get_value(attr_target, &start, &i);
    end = strchr(start, '/');
    __animation_get_bone(p, start, end - start, &bone);
    if (!id_validate(bone)) goto finish;

    collada_bone_fetch(bone, &rbone);

    xml_query_search(query, xn, "//param[@name=\"TRANSFORM\"]/../../../float_array");
    xml_query_get_node(query, 0, &xn_float_array);
    xml_node_get_attribute_by_name(xn_float_array, "count", &attr_count);
    xml_attribute_get_value(attr_count, &ptr, &ptr_len);
    count = atoi(ptr);

    buffer_new(&fa);
    buffer_reserve(fa, count * sizeof(float));
    xml_node_get_value(xn_float_array, &ptr, &ptr_len);
    buffer_append_float_string(fa, ptr, NULL);
    buffer_get_ptr(fa, &float_ptr);
    buffer_get_length_with_stride(fa, sizeof(float), &ptr_len);
    for (i = 0; i < ptr_len; i += 16) {
        mat4_new(&m);
        mat4_set_array(m, float_ptr + i);
        mat4_transpose(m);
        vector_push(rbone->animation_transforms, m);
        release(m);
    }
    release(fa);

    xml_query_search(query, xn, "//param[@name=\"TIME\"]/../../../float_array");
    xml_query_get_node(query, 0, &xn_float_array);
    xml_node_get_attribute_by_name(xn_float_array, "count", &attr_count);
    xml_attribute_get_value(attr_count, &ptr, &ptr_len);
    count = atoi(ptr);
    buffer_reserve(rbone->animation_times, count * sizeof(float));
    xml_node_get_value(xn_float_array, &ptr, &ptr_len);
    buffer_append_float_string(rbone->animation_times, ptr, NULL);
finish:
	release(query);
}

static void __parse_library_animations(struct collada_context *p, id xn)
{
    id query, node;
	int i;

    xml_query_new(&query);

    xml_query_search(query, xn, "//library_animations/animation");
    i = 0;
    xml_query_get_node(query, i, &node);
    while (id_validate(node)) {
        __parse_animation(p, node);
        i++;
        xml_query_get_node(query, i, &node);
    }

	release(query);
}


void collada_context_load_file(id pid, const char *path)
{
    struct collada_context *raw;
    id root;

    collada_context_fetch(pid, &raw);
    assert(raw != NULL);
    xml_node_new(&root);
    xml_node_load_file(root, path);
    __parse_library_geometries(raw, root);
    __parse_library_controllers(raw, root);
    __parse_library_visual_scenes(raw, root);
    __parse_library_animations(raw, root);
    release(root);
}

void collada_context_get_node(id pid, const char *name, id *nid)
{
    struct collada_context *rctx;

    collada_context_fetch(pid, &rctx);
    assert(rctx != NULL);

    map_get(rctx->nodes, key_chars(name), nid);
}

void collada_node_get_vertice(id pid, const float **ptr, unsigned *len)
{
    struct collada_node *rn;

    collada_node_fetch(pid, &rn);
    assert(rn != NULL);

    buffer_get_ptr(rn->vertices, ptr);
    buffer_get_length(rn->vertices, len);
}

void collada_node_get_normals(id pid, const float **ptr, unsigned *len)
{
    struct collada_node *rn;

    collada_node_fetch(pid, &rn);
    assert(rn != NULL);

    buffer_get_ptr(rn->normals, ptr);
    buffer_get_length(rn->normals, len);
}

void collada_node_get_texcoords(id pid, const float **ptr, unsigned *len)
{
    struct collada_node *rn;

    collada_node_fetch(pid, &rn);
    assert(rn != NULL);

    if (id_validate(rn->texcoords)) {
        buffer_get_ptr(rn->texcoords, ptr);
        buffer_get_length(rn->texcoords, len);
    } else {
        *ptr = NULL;
        *len = 0;
    }
}

void collada_node_get_colors(id pid, const float **ptr, unsigned *len)
{
    struct collada_node *rn;

    collada_node_fetch(pid, &rn);
    assert(rn != NULL);

    if (id_validate(rn->colors)) {
        buffer_get_ptr(rn->colors, ptr);
        buffer_get_length(rn->colors, len);
    } else {
        *ptr = NULL;
        *len = 0;
    }
}

void collada_node_get_transform(id pid, id *mid)
{
    struct collada_node *rn;

    collada_node_fetch(pid, &rn);
    assert(rn != NULL);

    *mid = rn->transform;
}