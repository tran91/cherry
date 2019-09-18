#include "vga.h"
#include "xml/xml.h"
#include "types/buffer.h"

/*
 * vga_cache
 */
struct vga_cache
{
    id texs;
    id atlases;
};
make_type(vga_cache);

static void vga_cache_init(struct vga_cache *p, key k)
{
    map_new(&p->texs);
    map_new(&p->atlases);
}

static void vga_cache_clear(struct vga_cache *p)
{
    release(p->texs);
    release(p->atlases);
}

void vga_cache_get_texture(id cid, const char *path, id *tid)
{
    struct vga_cache *rvc;

    vga_cache_fetch(cid, &rvc);
    assert(rvc != NULL);

    map_get(rvc->texs, key_chars(path), tid);
    if (!id_validate(*tid)) {
        vga_texture_new(tid);
        vga_texture_load_file(*tid, path);
        map_set(rvc->texs, key_chars(path), *tid);
        release(*tid);
    }
}

void vga_cache_get_atlas_frame(id cid, const char *atlas, const char *frame, float coords[4])
{
    struct vga_cache *rvc;
    id aid, buf, xid, query, node, attr;
    const char *ptr;
    unsigned ptr_len, index, width, height, x, y, w, h;
    float f;

    vga_cache_fetch(cid, &rvc);
    assert(rvc != NULL);

    map_get(rvc->atlases, key_chars(atlas), &aid);
    if (!id_validate(aid)) {
        map_new(&aid);
        map_set(rvc->atlases, key_chars(atlas), aid);
        release(aid);

        xml_node_new(&xid);
        xml_node_load_file(xid, atlas);

        xml_node_get_attribute_by_name(xid, "width", &attr);
        xml_attribute_get_value(attr, &ptr, &ptr_len);
        width = atoi(ptr);

        xml_node_get_attribute_by_name(xid, "height", &attr);
        xml_attribute_get_value(attr, &ptr, &ptr_len);
        height = atoi(ptr);

        xml_query_new(&query);
        xml_query_search(query, xid, "/sprite");
        index = 0;
        xml_query_get_node(query, index, &node);
        while(id_validate(node)) {

            xml_node_get_attribute_by_name(node, "x", &attr);
            xml_attribute_get_value(attr, &ptr, &ptr_len);
            x = atoi(ptr);

            xml_node_get_attribute_by_name(node, "y", &attr);
            xml_attribute_get_value(attr, &ptr, &ptr_len);
            y = atoi(ptr);

            xml_node_get_attribute_by_name(node, "w", &attr);
            xml_attribute_get_value(attr, &ptr, &ptr_len);
            w = atoi(ptr);

            xml_node_get_attribute_by_name(node, "h", &attr);
            xml_attribute_get_value(attr, &ptr, &ptr_len);
            h = atoi(ptr);

            xml_node_get_attribute_by_name(node, "n", &attr);
            xml_attribute_get_value(attr, &ptr, &ptr_len);

            buffer_new(&buf);
            map_set(aid, key_mem(ptr, ptr_len), buf);
            release(buf);

            f = x * 1.0f / width;
            buffer_append(buf, &f, sizeof(f));

            f = y * 1.0f / height;
            buffer_append(buf, &f, sizeof(f));

            f = (x + w) * 1.0f / width;
            buffer_append(buf, &f, sizeof(f));

            f = (y + h) * 1.0f / height;
            buffer_append(buf, &f, sizeof(f));

            index++;
            xml_query_get_node(query, index, &node);
        }
        release(query);
        release(xid);
    }

    map_get(aid, key_chars(frame), &buf);
    if (id_validate(buf)) {
        buffer_get_with_stride(buf, sizeof(float[4]), 0, coords);
    }
}