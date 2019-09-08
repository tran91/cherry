#include "image.h"
#include "types/file.h"
#include "types/buffer.h"
#include "jpeg/jpeglib.h"
#include "png/png.h"
#include "png/pngstruct.h"

struct image
{
    unsigned char *ptr;
    unsigned width;
    unsigned height;
    unsigned channels;
};
make_type(image);

static void init(struct image *p, key k)
{
    p->ptr = NULL;
    p->width = 0;
    p->height = 0;
    p->channels = 0;
}

static void clear(struct image *p)
{
    if (p->ptr) {
        free(p->ptr);
        p->ptr = NULL;
        p->width = 0;
        p->height = 0;
        p->channels = 0;
    }
}

/*
 * JPEG
 */
const static JOCTET EOI_BUFFER[1] = { JPEG_EOI };

struct jpeg_source {
    struct jpeg_source_mgr pub;
    const JOCTET *data;
    size_t       len;
};

static void __jpeg_init_source(j_decompress_ptr cinfo) {

}

static boolean __jpeg_fill_input_buffer(j_decompress_ptr cinfo) {
    struct jpeg_source* src = (struct jpeg_source*)cinfo->src;
    src->pub.next_input_byte = EOI_BUFFER;
    src->pub.bytes_in_buffer = 1;
    return TRUE;
}
static void __jpeg_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
    struct jpeg_source* src = (struct jpeg_source*)cinfo->src;
    if (src->pub.bytes_in_buffer < num_bytes) {
        src->pub.next_input_byte = EOI_BUFFER;
        src->pub.bytes_in_buffer = 1;
    } else {
        src->pub.next_input_byte += num_bytes;
        src->pub.bytes_in_buffer -= num_bytes;
    }
}
static void __jpeg_term_source(j_decompress_ptr cinfo) {

}

static void __jpeg_set_source_mgr(j_decompress_ptr cinfo,
    const char* data, size_t len) {
    struct jpeg_source* src;
    if (cinfo->src == 0) {
        cinfo->src = (struct jpeg_source_mgr *)
        (*cinfo->mem->alloc_small)
        ((j_common_ptr) cinfo, JPOOL_PERMANENT,
            sizeof(struct jpeg_source));
    }
    src = (struct jpeg_source*) cinfo->src;
    src->pub.init_source = __jpeg_init_source;
    src->pub.fill_input_buffer = __jpeg_fill_input_buffer;
    src->pub.skip_input_data = __jpeg_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = __jpeg_term_source;

    src->data = data;
    src->len = len;
    src->pub.bytes_in_buffer = len;
    src->pub.next_input_byte = src->data;
}

static void __load_jpeg(struct image *p, const char *path)
{
    id buf;
    struct jpeg_decompress_struct cInfo;
    struct jpeg_error_mgr jError;
    char* pTexUint;
    int yy;
    const char *buf_ptr;
    unsigned buf_len;
    
    buffer_new(&buf);
    buffer_append_file(buf, path);
    buffer_get_ptr(buf, &buf_ptr);
    buffer_get_length(buf, &buf_len);

    cInfo.err = jpeg_std_error(&jError);

    jpeg_create_decompress(&cInfo);
    __jpeg_set_source_mgr(&cInfo, buf_ptr, buf_len);

    jpeg_read_header(&cInfo, TRUE);
    jpeg_start_decompress(&cInfo);

    p->width = cInfo.output_width;
    p->height = cInfo.output_height;

    pTexUint = calloc(3, p->width * p->height);
    JSAMPROW buffer[1];
    buffer[0] = pTexUint + p->width * 3 * (p->height - 1);

    yy = 0;
    while(cInfo.output_scanline < cInfo.output_height) {
        if(yy >= cInfo.output_height)
            break;

        jpeg_read_scanlines(&cInfo, buffer, 1);

        yy++;
        if(yy < cInfo.output_height) {
            buffer[0] -= p->width * 3;
        }
    }

    jpeg_finish_decompress(&cInfo);
    jpeg_destroy_decompress(&cInfo);

    p->ptr = (unsigned char *)pTexUint;
    p->channels = 3;

    release(buf);
}

static void read_png_chunk(png_structp png_ptr, png_bytep data, png_size_t length)
{
    unsigned rv;
    file_read(*(id *)png_ptr->io_ptr, data, length, &rv);
}

/*
 * PNG
 */
static void load_png(struct image *p, const char *path)
{
    id fid;
    png_byte * volatile image_data = NULL;
    png_bytep * volatile row_pointers = NULL;
    png_byte header[8];
    unsigned rv;
    int bit_depth, color_type;
    int rowbytes;

    file_new(&fid);
    file_open(fid, path);
    file_read(fid, header, 8, &rv);
    png_structp png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        goto error;
    }
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        goto error;
    }
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        goto error;
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        goto error;
    }
    png_set_read_fn(png_ptr, &fid, read_png_chunk);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &p->width, &p->height, &bit_depth, &color_type, NULL, NULL, NULL);
    if(bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }
    if(color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    image_data = malloc(sizeof(png_byte) * rowbytes * p->height);
    if (!image_data) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        goto error;
    }
    row_pointers = malloc(sizeof(png_bytep) * p->height);
    if (!row_pointers) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        goto error;
    }
    for (int i = 0; i < p->height; ++i) {
        row_pointers[p->height - 1 - i] = image_data + i * rowbytes;
    }
    png_read_image(png_ptr, row_pointers);
    p->ptr      = image_data;
    p->channels = 4;
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(row_pointers);
    release(fid);
    return;

error:
    if(row_pointers) {
        free(row_pointers);
    }
    if(image_data) {
        free(image_data);
    }
    release(fid);
}

void image_load_file(id pid, const char *path)
{
    struct image *raw;
    id fid;
    unsigned rv;
    int test;
    png_byte header[8];

    fetch(pid, &raw);
    assert(raw != NULL);
    clear(raw);
    file_new(&fid);
    file_open(fid, path);
    file_read(fid, header, 8, &rv);
    test = !png_sig_cmp(header, 0, 8);
    release(fid);
    if (!test) {
        __load_jpeg(raw, path);
    } else {
        load_png(raw, path);
    }
}

void image_get_size(id pid, unsigned *width, unsigned *height)
{
    struct image *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *width = raw->width;
    *height = raw->height;
}

void image_get_ptr(id pid, const unsigned char **ptr)
{
    struct image *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *ptr = raw->ptr;
}

void image_get_number_channels(id pid, unsigned *count)
{
    struct image *raw;

    fetch(pid, &raw);
    assert(raw != NULL);

    *count = raw->channels;
}