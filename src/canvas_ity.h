/* canvas_ity v1.00 -- ISC license
 * Copyright (c) 2022 Andrew Kensler
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * C89 port of the original C++ single-header library for rasterizing
 * immediate-mode 2D vector graphics, closely modeled on the W3C HTML5
 * 2D canvas specification.
 *
 * This is a single-header library. Define CANVAS_ITY_IMPLEMENTATION in
 * exactly one C file before including this header to get the implementation.
 */

#ifndef CANVAS_ITY_H
#define CANVAS_ITY_H

#include <stddef.h>

/* ======== PUBLIC API ENUMS ======== */

typedef enum ci_composite_operation {
    CI_SOURCE_IN = 1,
    CI_SOURCE_COPY = 2,
    CI_SOURCE_OUT = 3,
    CI_DESTINATION_IN = 4,
    CI_DESTINATION_ATOP = 7,
    CI_LIGHTER = 10,
    CI_DESTINATION_OVER = 11,
    CI_DESTINATION_OUT = 12,
    CI_SOURCE_ATOP = 13,
    CI_SOURCE_OVER = 14,
    CI_EXCLUSIVE_OR = 15
} ci_composite_operation;

typedef enum ci_cap_style {
    CI_CAP_BUTT = 0,
    CI_CAP_SQUARE = 1,
    CI_CAP_CIRCLE = 2
} ci_cap_style;

typedef enum ci_join_style {
    CI_JOIN_MITER = 0,
    CI_JOIN_BEVEL = 1,
    CI_JOIN_ROUNDED = 2
} ci_join_style;

typedef enum ci_brush_type {
    CI_FILL_STYLE = 0,
    CI_STROKE_STYLE = 1
} ci_brush_type;

typedef enum ci_repetition_style {
    CI_REPEAT = 0,
    CI_REPEAT_X = 1,
    CI_REPEAT_Y = 2,
    CI_NO_REPEAT = 3
} ci_repetition_style;

typedef enum ci_align_style {
    CI_ALIGN_LEFTWARD = 0,
    CI_ALIGN_RIGHTWARD = 1,
    CI_ALIGN_CENTER = 2,
    CI_ALIGN_START = 0,
    CI_ALIGN_ENDING = 1
} ci_align_style;

typedef enum ci_baseline_style {
    CI_BASELINE_ALPHABETIC = 0,
    CI_BASELINE_TOP = 1,
    CI_BASELINE_MIDDLE = 2,
    CI_BASELINE_BOTTOM = 3,
    CI_BASELINE_HANGING = 4,
    CI_BASELINE_IDEOGRAPHIC = 3
} ci_baseline_style;

typedef enum ci_paint_type {
    CI_PAINT_COLOR = 0,
    CI_PAINT_LINEAR = 1,
    CI_PAINT_RADIAL = 2,
    CI_PAINT_PATTERN = 3
} ci_paint_type;

/* ======== BASIC TYPES ======== */

typedef struct ci_xy { float x; float y; } ci_xy_t;
typedef struct ci_rgba { float r; float g; float b; float a; } ci_rgba_t;
typedef struct ci_affine_matrix {
    float a; float b; float c; float d; float e; float f;
} ci_affine_matrix_t;
typedef struct ci_subpath_data { size_t count; int closed; } ci_subpath_data_t;
typedef struct ci_pixel_run {
    unsigned short x; unsigned short y; float delta;
} ci_pixel_run_t;

/* ======== DYNAMIC ARRAY TYPES ======== */

typedef struct ci_float_array {
    float *data; size_t size; size_t capacity;
} ci_float_array_t;

typedef struct ci_xy_array {
    ci_xy_t *data; size_t size; size_t capacity;
} ci_xy_array_t;

typedef struct ci_rgba_array {
    ci_rgba_t *data; size_t size; size_t capacity;
} ci_rgba_array_t;

typedef struct ci_uchar_array {
    unsigned char *data; size_t size; size_t capacity;
} ci_uchar_array_t;

typedef struct ci_subpath_array {
    ci_subpath_data_t *data; size_t size; size_t capacity;
} ci_subpath_array_t;

typedef struct ci_run_array {
    ci_pixel_run_t *data; size_t size; size_t capacity;
} ci_run_array_t;

/* ======== COMPOSITE TYPES ======== */

typedef struct ci_paint_brush {
    ci_paint_type type;
    ci_rgba_array_t colors;
    ci_float_array_t stops;
    ci_xy_t start;
    ci_xy_t end;
    float start_radius;
    float end_radius;
    int width;
    int height;
    ci_repetition_style repetition;
} ci_paint_brush_t;

typedef struct ci_font_face {
    ci_uchar_array_t data;
    int cmap;
    int glyf;
    int head;
    int hhea;
    int hmtx;
    int loca;
    int maxp;
    int os_2;
    float scale;
} ci_font_face_t;

typedef struct ci_bezier_path {
    ci_xy_array_t points;
    ci_subpath_array_t subpaths;
} ci_bezier_path_t;

typedef struct ci_line_path {
    ci_xy_array_t points;
    ci_subpath_array_t subpaths;
} ci_line_path_t;

/* ======== CANVAS TYPE ======== */

typedef struct ci_canvas ci_canvas_t;

struct ci_canvas {
    /* Public fields (directly readable/writable) */
    ci_composite_operation global_composite_operation;
    float shadow_offset_x;
    float shadow_offset_y;
    ci_cap_style line_cap;
    ci_join_style line_join;
    float line_dash_offset;
    ci_align_style text_align;
    ci_baseline_style text_baseline;
    /* Internal fields */
    int size_x;
    int size_y;
    ci_affine_matrix_t forward;
    ci_affine_matrix_t inverse;
    float global_alpha;
    ci_rgba_t shadow_color;
    float shadow_blur;
    ci_float_array_t shadow;
    float line_width;
    float miter_limit;
    ci_float_array_t line_dash;
    ci_paint_brush_t fill_brush;
    ci_paint_brush_t stroke_brush;
    ci_paint_brush_t image_brush;
    ci_bezier_path_t path;
    ci_line_path_t lines;
    ci_line_path_t scratch;
    ci_run_array_t runs;
    ci_run_array_t mask;
    ci_font_face_t face;
    ci_rgba_t *bitmap;
    ci_canvas_t *saves;
};

/* ======== PUBLIC API ======== */

ci_canvas_t *ci_canvas_create(int width, int height);
void ci_canvas_destroy(ci_canvas_t *ctx);

void ci_canvas_scale(ci_canvas_t *ctx, float x, float y);
void ci_canvas_rotate(ci_canvas_t *ctx, float angle);
void ci_canvas_translate(ci_canvas_t *ctx, float x, float y);
void ci_canvas_transform(ci_canvas_t *ctx,
    float a, float b, float c, float d, float e, float f);
void ci_canvas_set_transform(ci_canvas_t *ctx,
    float a, float b, float c, float d, float e, float f);

void ci_canvas_set_global_alpha(ci_canvas_t *ctx, float alpha);

void ci_canvas_set_shadow_color(ci_canvas_t *ctx,
    float red, float green, float blue, float alpha);
void ci_canvas_set_shadow_blur(ci_canvas_t *ctx, float level);

void ci_canvas_set_line_width(ci_canvas_t *ctx, float width);
void ci_canvas_set_miter_limit(ci_canvas_t *ctx, float limit);
void ci_canvas_set_line_dash(ci_canvas_t *ctx,
    float const *segments, int count);

void ci_canvas_set_color(ci_canvas_t *ctx, ci_brush_type type,
    float red, float green, float blue, float alpha);
void ci_canvas_set_linear_gradient(ci_canvas_t *ctx, ci_brush_type type,
    float start_x, float start_y, float end_x, float end_y);
void ci_canvas_set_radial_gradient(ci_canvas_t *ctx, ci_brush_type type,
    float start_x, float start_y, float start_radius,
    float end_x, float end_y, float end_radius);
void ci_canvas_add_color_stop(ci_canvas_t *ctx, ci_brush_type type,
    float offset, float red, float green, float blue, float alpha);
void ci_canvas_set_pattern(ci_canvas_t *ctx, ci_brush_type type,
    unsigned char const *image, int width, int height,
    int stride, ci_repetition_style repetition);

void ci_canvas_begin_path(ci_canvas_t *ctx);
void ci_canvas_move_to(ci_canvas_t *ctx, float x, float y);
void ci_canvas_close_path(ci_canvas_t *ctx);
void ci_canvas_line_to(ci_canvas_t *ctx, float x, float y);
void ci_canvas_quadratic_curve_to(ci_canvas_t *ctx,
    float control_x, float control_y, float x, float y);
void ci_canvas_bezier_curve_to(ci_canvas_t *ctx,
    float control_1_x, float control_1_y,
    float control_2_x, float control_2_y, float x, float y);
void ci_canvas_arc_to(ci_canvas_t *ctx,
    float vertex_x, float vertex_y,
    float x, float y, float radius);
void ci_canvas_arc(ci_canvas_t *ctx, float x, float y, float radius,
    float start_angle, float end_angle, int counter_clockwise);
void ci_canvas_rectangle(ci_canvas_t *ctx,
    float x, float y, float width, float height);

void ci_canvas_fill(ci_canvas_t *ctx);
void ci_canvas_stroke(ci_canvas_t *ctx);
void ci_canvas_clip(ci_canvas_t *ctx);
int ci_canvas_is_point_in_path(ci_canvas_t *ctx, float x, float y);

void ci_canvas_clear_rectangle(ci_canvas_t *ctx,
    float x, float y, float width, float height);
void ci_canvas_fill_rectangle(ci_canvas_t *ctx,
    float x, float y, float width, float height);
void ci_canvas_stroke_rectangle(ci_canvas_t *ctx,
    float x, float y, float width, float height);

int ci_canvas_set_font(ci_canvas_t *ctx,
    unsigned char const *font, int bytes, float size);
void ci_canvas_fill_text(ci_canvas_t *ctx, char const *text,
    float x, float y, float maximum_width);
void ci_canvas_stroke_text(ci_canvas_t *ctx, char const *text,
    float x, float y, float maximum_width);
float ci_canvas_measure_text(ci_canvas_t *ctx, char const *text);

void ci_canvas_draw_image(ci_canvas_t *ctx,
    unsigned char const *image, int width, int height, int stride,
    float x, float y, float to_width, float to_height);

void ci_canvas_get_image_data(ci_canvas_t *ctx,
    unsigned char *image, int width, int height, int stride,
    int x, int y);
void ci_canvas_put_image_data(ci_canvas_t *ctx,
    unsigned char const *image, int width, int height, int stride,
    int x, int y);

void ci_canvas_save(ci_canvas_t *ctx);
void ci_canvas_restore(ci_canvas_t *ctx);

#endif /* CANVAS_ITY_H */

/* ======== IMPLEMENTATION ======== */

#ifdef CANVAS_ITY_IMPLEMENTATION

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ======== MATH HELPERS (C89 lacks float math functions) ======== */

#define CI_MIN(a, b) ((a) < (b) ? (a) : (b))
#define CI_MAX(a, b) ((a) > (b) ? (a) : (b))
#define CI_CLAMP(x, lo, hi) CI_MIN(CI_MAX((x), (lo)), (hi))

#define ci_sqrtf(x)  ((float)sqrt((double)(x)))
#define ci_fabsf(x)  ((float)fabs((double)(x)))
#define ci_cosf(x)   ((float)cos((double)(x)))
#define ci_sinf(x)   ((float)sin((double)(x)))
#define ci_tanf(x)   ((float)tan((double)(x)))
#define ci_acosf(x)  ((float)acos((double)(x)))
#define ci_atan2f(y, x) ((float)atan2((double)(y), (double)(x)))
#define ci_powf(b, e) ((float)pow((double)(b), (double)(e)))
#define ci_floorf(x) ((float)floor((double)(x)))
#define ci_ceilf(x)  ((float)ceil((double)(x)))
#define ci_fmodf(x, y) ((float)fmod((double)(x), (double)(y)))
#define ci_roundf(x) ((float)floor((double)(x) + 0.5))

/* ======== DYNAMIC ARRAY IMPLEMENTATION ======== */

#define CI_ARRAY_IMPL(PREFIX, TYPE)                                         \
static void PREFIX##_init(PREFIX##_t *a) {                                  \
    a->data = NULL; a->size = 0; a->capacity = 0;                          \
}                                                                           \
static void PREFIX##_free(PREFIX##_t *a) {                                  \
    free(a->data); a->data = NULL; a->size = 0; a->capacity = 0;           \
}                                                                           \
static void PREFIX##_clear(PREFIX##_t *a) {                                 \
    a->size = 0;                                                            \
}                                                                           \
static int PREFIX##_ensure(PREFIX##_t *a, size_t needed) {                  \
    size_t cap;                                                             \
    TYPE *p;                                                                \
    if (needed <= a->capacity) return 1;                                    \
    cap = a->capacity ? a->capacity : 4;                                    \
    while (cap < needed) cap *= 2;                                          \
    p = (TYPE *)realloc(a->data, cap * sizeof(TYPE));                       \
    if (!p) return 0;                                                       \
    a->data = p;                                                            \
    a->capacity = cap;                                                      \
    return 1;                                                               \
}                                                                           \
static int PREFIX##_push(PREFIX##_t *a, TYPE val) {                         \
    if (!PREFIX##_ensure(a, a->size + 1)) return 0;                         \
    a->data[a->size++] = val;                                               \
    return 1;                                                               \
}                                                                           \
static void PREFIX##_pop(PREFIX##_t *a) {                                   \
    if (a->size > 0) a->size--;                                             \
}                                                                           \
static void PREFIX##_swap(PREFIX##_t *a, PREFIX##_t *b) {                   \
    PREFIX##_t tmp;                                                         \
    tmp = *a; *a = *b; *b = tmp;                                            \
}                                                                           \
static int PREFIX##_insert(PREFIX##_t *a, size_t pos, TYPE val) {           \
    size_t i;                                                               \
    if (!PREFIX##_ensure(a, a->size + 1)) return 0;                         \
    for (i = a->size; i > pos; i--) a->data[i] = a->data[i - 1];           \
    a->data[pos] = val;                                                     \
    a->size++;                                                              \
    return 1;                                                               \
}                                                                           \
static void PREFIX##_erase(PREFIX##_t *a, size_t from, size_t to) {         \
    size_t i;                                                               \
    size_t count = to - from;                                               \
    for (i = from; i + count < a->size; i++)                                \
        a->data[i] = a->data[i + count];                                    \
    a->size -= count;                                                       \
}                                                                           \
static int PREFIX##_resize(PREFIX##_t *a, size_t n) {                       \
    size_t i;                                                               \
    if (!PREFIX##_ensure(a, n)) return 0;                                   \
    for (i = a->size; i < n; i++)                                           \
        memset(&a->data[i], 0, sizeof(TYPE));                               \
    a->size = n;                                                            \
    return 1;                                                               \
}                                                                           \
static int PREFIX##_copy(PREFIX##_t *dst, const PREFIX##_t *src) {          \
    if (src->size == 0) { dst->size = 0; return 1; }                        \
    if (!PREFIX##_ensure(dst, src->size)) return 0;                         \
    memcpy(dst->data, src->data, src->size * sizeof(TYPE));                 \
    dst->size = src->size;                                                  \
    return 1;                                                               \
}                                                                           \
static int PREFIX##_append(PREFIX##_t *a,                                   \
        const TYPE *d, size_t count) {                                      \
    size_t i;                                                               \
    if (!PREFIX##_ensure(a, a->size + count)) return 0;                     \
    for (i = 0; i < count; i++) a->data[a->size + i] = d[i];               \
    a->size += count;                                                       \
    return 1;                                                               \
}

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

CI_ARRAY_IMPL(ci_float_array, float)
CI_ARRAY_IMPL(ci_xy_array, ci_xy_t)
CI_ARRAY_IMPL(ci_rgba_array, ci_rgba_t)
CI_ARRAY_IMPL(ci_uchar_array, unsigned char)
CI_ARRAY_IMPL(ci_subpath_array, ci_subpath_data_t)
CI_ARRAY_IMPL(ci_run_array, ci_pixel_run_t)

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/* ======== CONSTRUCTOR HELPERS ======== */

static ci_xy_t ci_xy_make(float x, float y) {
    ci_xy_t r; r.x = x; r.y = y; return r;
}

static ci_rgba_t ci_rgba_make(float r, float g, float b, float a) {
    ci_rgba_t c; c.r = r; c.g = g; c.b = b; c.a = a; return c;
}

static ci_pixel_run_t ci_run_make(unsigned short x, unsigned short y,
        float delta) {
    ci_pixel_run_t r; r.x = x; r.y = y; r.delta = delta; return r;
}

static ci_subpath_data_t ci_subpath_make(size_t count, int closed) {
    ci_subpath_data_t s; s.count = count; s.closed = closed; return s;
}

/* ======== 2D VECTOR MATH ======== */

static ci_xy_t ci_xy_add(ci_xy_t a, ci_xy_t b) {
    return ci_xy_make(a.x + b.x, a.y + b.y);
}

static ci_xy_t ci_xy_sub(ci_xy_t a, ci_xy_t b) {
    return ci_xy_make(a.x - b.x, a.y - b.y);
}

static ci_xy_t ci_xy_scale(float s, ci_xy_t v) {
    return ci_xy_make(s * v.x, s * v.y);
}

static ci_xy_t ci_xy_mul(ci_xy_t a, ci_xy_t b) {
    return ci_xy_make(a.x * b.x, a.y * b.y);
}

static ci_xy_t ci_affine_mul(ci_affine_matrix_t m, ci_xy_t v) {
    return ci_xy_make(m.a * v.x + m.c * v.y + m.e,
                      m.b * v.x + m.d * v.y + m.f);
}

static float ci_dot(ci_xy_t a, ci_xy_t b) {
    return a.x * b.x + a.y * b.y;
}

static float ci_length(ci_xy_t v) {
    return ci_sqrtf(ci_dot(v, v));
}

static float ci_direction(ci_xy_t v) {
    return ci_atan2f(v.y, v.x);
}

static ci_xy_t ci_normalized(ci_xy_t v) {
    return ci_xy_scale(1.0f / CI_MAX(1.0e-6f, ci_length(v)), v);
}

static ci_xy_t ci_perpendicular(ci_xy_t v) {
    return ci_xy_make(-v.y, v.x);
}

static ci_xy_t ci_lerp_xy(ci_xy_t from, ci_xy_t to, float ratio) {
    return ci_xy_add(from, ci_xy_scale(ratio, ci_xy_sub(to, from)));
}

/* ======== RGBA COLOR MATH ======== */

static ci_rgba_t ci_rgba_add(ci_rgba_t a, ci_rgba_t b) {
    return ci_rgba_make(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

static ci_rgba_t ci_rgba_sub(ci_rgba_t a, ci_rgba_t b) {
    return ci_rgba_make(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}

static ci_rgba_t ci_rgba_scale(float s, ci_rgba_t c) {
    return ci_rgba_make(s * c.r, s * c.g, s * c.b, s * c.a);
}

/* ======== COLOR SPACE CONVERSIONS ======== */

static float ci_linearize_f(float value) {
    return value < 0.04045f ? value / 12.92f :
        ci_powf((value + 0.055f) / 1.055f, 2.4f);
}

static ci_rgba_t ci_linearized(ci_rgba_t c) {
    return ci_rgba_make(ci_linearize_f(c.r), ci_linearize_f(c.g),
                        ci_linearize_f(c.b), c.a);
}

static ci_rgba_t ci_premultiplied(ci_rgba_t c) {
    return ci_rgba_make(c.r * c.a, c.g * c.a, c.b * c.a, c.a);
}

static float ci_delinearize_f(float value) {
    return value < 0.0031308f ? 12.92f * value :
        1.055f * ci_powf(value, 1.0f / 2.4f) - 0.055f;
}

static ci_rgba_t ci_delinearized(ci_rgba_t c) {
    return ci_rgba_make(ci_delinearize_f(c.r), ci_delinearize_f(c.g),
                        ci_delinearize_f(c.b), c.a);
}

static ci_rgba_t ci_unpremultiplied(ci_rgba_t c) {
    float inv;
    if (c.a < 1.0f / 8160.0f)
        return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
    inv = 1.0f / c.a;
    return ci_rgba_make(inv * c.r, inv * c.g, inv * c.b, c.a);
}

static ci_rgba_t ci_clamped(ci_rgba_t c) {
    return ci_rgba_make(CI_CLAMP(c.r, 0.0f, 1.0f),
                        CI_CLAMP(c.g, 0.0f, 1.0f),
                        CI_CLAMP(c.b, 0.0f, 1.0f),
                        CI_CLAMP(c.a, 0.0f, 1.0f));
}

/* ======== TTF FILE PARSING HELPERS ======== */

static int ci_unsigned_8(ci_uchar_array_t *data, int index) {
    return data->data[(size_t)index];
}

static int ci_signed_8(ci_uchar_array_t *data, int index) {
    return (signed char)data->data[(size_t)index];
}

static int ci_unsigned_16(ci_uchar_array_t *data, int index) {
    size_t p = (size_t)index;
    return data->data[p] << 8 | data->data[p + 1];
}

static int ci_signed_16(ci_uchar_array_t *data, int index) {
    size_t p = (size_t)index;
    return (short)(data->data[p] << 8 | data->data[p + 1]);
}

static int ci_signed_32(ci_uchar_array_t *data, int index) {
    size_t p = (size_t)index;
    return (int)(data->data[p + 0] << 24 | data->data[p + 1] << 16 |
                 data->data[p + 2] <<  8 | data->data[p + 3] <<  0);
}

/* ======== PAINT BRUSH / FONT HELPERS ======== */

static void ci_paint_brush_init(ci_paint_brush_t *b) {
    memset(b, 0, sizeof(*b));
    ci_rgba_array_init(&b->colors);
    ci_float_array_init(&b->stops);
}

static void ci_paint_brush_free(ci_paint_brush_t *b) {
    ci_rgba_array_free(&b->colors);
    ci_float_array_free(&b->stops);
}

static void ci_paint_brush_copy(ci_paint_brush_t *dst,
        const ci_paint_brush_t *src) {
    *dst = *src;
    ci_rgba_array_init(&dst->colors);
    ci_float_array_init(&dst->stops);
    ci_rgba_array_copy(&dst->colors, &src->colors);
    ci_float_array_copy(&dst->stops, &src->stops);
}

static void ci_font_face_init(ci_font_face_t *f) {
    memset(f, 0, sizeof(*f));
    ci_uchar_array_init(&f->data);
}

static void ci_font_face_free(ci_font_face_t *f) {
    ci_uchar_array_free(&f->data);
}

static void ci_font_face_copy(ci_font_face_t *dst,
        const ci_font_face_t *src) {
    *dst = *src;
    ci_uchar_array_init(&dst->data);
    ci_uchar_array_copy(&dst->data, &src->data);
}

static void ci_line_path_init(ci_line_path_t *p) {
    ci_xy_array_init(&p->points);
    ci_subpath_array_init(&p->subpaths);
}

static void ci_line_path_free(ci_line_path_t *p) {
    ci_xy_array_free(&p->points);
    ci_subpath_array_free(&p->subpaths);
}

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
static void ci_line_path_copy(ci_line_path_t *dst,
        const ci_line_path_t *src) {
    ci_xy_array_init(&dst->points);
    ci_subpath_array_init(&dst->subpaths);
    ci_xy_array_copy(&dst->points, &src->points);
    ci_subpath_array_copy(&dst->subpaths, &src->subpaths);
}
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

static void ci_bezier_path_init(ci_bezier_path_t *p) {
    ci_xy_array_init(&p->points);
    ci_subpath_array_init(&p->subpaths);
}

static void ci_bezier_path_free(ci_bezier_path_t *p) {
    ci_xy_array_free(&p->points);
    ci_subpath_array_free(&p->subpaths);
}


/* ======== FORWARD DECLARATIONS ======== */

static void ci_add_tessellation(ci_canvas_t *ctx, ci_xy_t p1, ci_xy_t c1,
    ci_xy_t c2, ci_xy_t p2, float angular, int limit);
static void ci_add_bezier(ci_canvas_t *ctx, ci_xy_t p1, ci_xy_t c1,
    ci_xy_t c2, ci_xy_t p2, float angular);
static void ci_path_to_lines(ci_canvas_t *ctx, int stroking);
static void ci_add_glyph(ci_canvas_t *ctx, int glyph, float angular);
static int ci_character_to_glyph(ci_canvas_t *ctx, char const *text,
    int *index);
static void ci_text_to_lines(ci_canvas_t *ctx, char const *text,
    ci_xy_t position, float maximum_width, int stroking);
static void ci_dash_lines(ci_canvas_t *ctx);
static void ci_add_half_stroke(ci_canvas_t *ctx, size_t beginning,
    size_t ending, int closed);
static void ci_stroke_lines(ci_canvas_t *ctx);
static void ci_add_runs(ci_canvas_t *ctx, ci_xy_t from, ci_xy_t to);
static void ci_lines_to_runs(ci_canvas_t *ctx, ci_xy_t offset, int padding);
static ci_rgba_t ci_paint_pixel(ci_canvas_t *ctx, ci_xy_t point,
    ci_paint_brush_t const *brush);
static void ci_render_shadow(ci_canvas_t *ctx,
    ci_paint_brush_t const *brush);
static void ci_render_main(ci_canvas_t *ctx,
    ci_paint_brush_t const *brush);

/* ======== TESSELLATION ======== */

static void ci_add_tessellation(ci_canvas_t *ctx, ci_xy_t point_1,
    ci_xy_t control_1, ci_xy_t control_2, ci_xy_t point_2,
    float angular, int limit)
{
    float flatness = 0.125f * 0.125f;
    ci_xy_t edge_1 = ci_xy_sub(control_1, point_1);
    ci_xy_t edge_2 = ci_xy_sub(control_2, control_1);
    ci_xy_t edge_3 = ci_xy_sub(point_2, control_2);
    ci_xy_t segment = ci_xy_sub(point_2, point_1);
    float squared_1 = ci_dot(edge_1, edge_1);
    float squared_2 = ci_dot(edge_2, edge_2);
    float squared_3 = ci_dot(edge_3, edge_3);
    float length_squared = CI_MAX(1.0e-4f, ci_dot(segment, segment));
    float projection_1 = ci_dot(edge_1, segment) / length_squared;
    float projection_2 = ci_dot(edge_3, segment) / length_squared;
    float clamped_1 = CI_CLAMP(projection_1, 0.0f, 1.0f);
    float clamped_2 = CI_CLAMP(projection_2, 0.0f, 1.0f);
    ci_xy_t to_line_1 = ci_xy_sub(
        ci_xy_add(point_1, ci_xy_scale(clamped_1, segment)), control_1);
    ci_xy_t to_line_2 = ci_xy_sub(
        ci_xy_sub(point_2, ci_xy_scale(clamped_2, segment)), control_2);
    float cosine = 1.0f;
    ci_xy_t left_1, middle, right_2, left_2, right_1, split;
    if (angular > -1.0f) {
        if (squared_1 * squared_3 != 0.0f)
            cosine = ci_dot(edge_1, edge_3) /
                ci_sqrtf(squared_1 * squared_3);
        else if (squared_1 * squared_2 != 0.0f)
            cosine = ci_dot(edge_1, edge_2) /
                ci_sqrtf(squared_1 * squared_2);
        else if (squared_2 * squared_3 != 0.0f)
            cosine = ci_dot(edge_2, edge_3) /
                ci_sqrtf(squared_2 * squared_3);
    }
    if ((ci_dot(to_line_1, to_line_1) <= flatness &&
         ci_dot(to_line_2, to_line_2) <= flatness &&
         cosine >= angular) || !limit) {
        if (angular > -1.0f && squared_1 != 0.0f)
            ci_xy_array_push(&ctx->lines.points, control_1);
        if (angular > -1.0f && squared_2 != 0.0f)
            ci_xy_array_push(&ctx->lines.points, control_2);
        if (angular == -1.0f || squared_3 != 0.0f)
            ci_xy_array_push(&ctx->lines.points, point_2);
        return;
    }
    left_1 = ci_lerp_xy(point_1, control_1, 0.5f);
    middle = ci_lerp_xy(control_1, control_2, 0.5f);
    right_2 = ci_lerp_xy(control_2, point_2, 0.5f);
    left_2 = ci_lerp_xy(left_1, middle, 0.5f);
    right_1 = ci_lerp_xy(middle, right_2, 0.5f);
    split = ci_lerp_xy(left_2, right_1, 0.5f);
    ci_add_tessellation(ctx, point_1, left_1, left_2, split,
        angular, limit - 1);
    ci_add_tessellation(ctx, split, right_1, right_2, point_2,
        angular, limit - 1);
}

/* ======== HIGH-LEVEL BEZIER TESSELLATION ======== */

static void ci_add_bezier(ci_canvas_t *ctx, ci_xy_t point_1,
    ci_xy_t control_1, ci_xy_t control_2, ci_xy_t point_2, float angular)
{
    ci_xy_t edge_1 = ci_xy_sub(control_1, point_1);
    ci_xy_t edge_2 = ci_xy_sub(control_2, control_1);
    ci_xy_t edge_3 = ci_xy_sub(point_2, control_2);
    ci_xy_t extrema_a, extrema_b, extrema_c;
    float at[7];
    int cuts = 2;
    float determinant_1, determinant_2, determinant_3;
    float curve_a, curve_b;
    int index;
    ci_xy_t split_point_1;
    if (ci_dot(edge_1, edge_1) == 0.0f &&
        ci_dot(edge_3, edge_3) == 0.0f) {
        ci_xy_array_push(&ctx->lines.points, point_2);
        return;
    }
    at[0] = 0.0f;
    at[1] = 1.0f;
    extrema_a = ci_xy_add(ci_xy_scale(-9.0f, edge_2),
        ci_xy_scale(3.0f, ci_xy_sub(point_2, point_1)));
    extrema_b = ci_xy_sub(ci_xy_scale(6.0f,
        ci_xy_add(point_1, control_2)),
        ci_xy_scale(12.0f, control_1));
    extrema_c = ci_xy_scale(3.0f, edge_1);
    if (ci_fabsf(extrema_a.x) > 1.0e-4f) {
        float disc = extrema_b.x * extrema_b.x -
            4.0f * extrema_a.x * extrema_c.x;
        if (disc >= 0.0f) {
            float sign = extrema_b.x > 0.0f ? 1.0f : -1.0f;
            float term = -extrema_b.x - sign * ci_sqrtf(disc);
            float ext1 = term / (2.0f * extrema_a.x);
            at[cuts++] = ext1;
            at[cuts++] = extrema_c.x / (extrema_a.x * ext1);
        }
    } else if (ci_fabsf(extrema_b.x) > 1.0e-4f) {
        at[cuts++] = -extrema_c.x / extrema_b.x;
    }
    if (ci_fabsf(extrema_a.y) > 1.0e-4f) {
        float disc = extrema_b.y * extrema_b.y -
            4.0f * extrema_a.y * extrema_c.y;
        if (disc >= 0.0f) {
            float sign = extrema_b.y > 0.0f ? 1.0f : -1.0f;
            float term = -extrema_b.y - sign * ci_sqrtf(disc);
            float ext1 = term / (2.0f * extrema_a.y);
            at[cuts++] = ext1;
            at[cuts++] = extrema_c.y / (extrema_a.y * ext1);
        }
    } else if (ci_fabsf(extrema_b.y) > 1.0e-4f) {
        at[cuts++] = -extrema_c.y / extrema_b.y;
    }
    determinant_1 = ci_dot(ci_perpendicular(edge_1), edge_2);
    determinant_2 = ci_dot(ci_perpendicular(edge_1), edge_3);
    determinant_3 = ci_dot(ci_perpendicular(edge_2), edge_3);
    curve_a = determinant_1 - determinant_2 + determinant_3;
    curve_b = -2.0f * determinant_1 + determinant_2;
    if (ci_fabsf(curve_a) > 1.0e-4f &&
        ci_fabsf(curve_b) > 1.0e-4f)
        at[cuts++] = -0.5f * curve_b / curve_a;
    /* insertion sort */
    for (index = 1; index < cuts; ++index) {
        float value = at[index];
        int sorted = index - 1;
        for (; 0 <= sorted && value < at[sorted]; --sorted)
            at[sorted + 1] = at[sorted];
        at[sorted + 1] = value;
    }
    split_point_1 = point_1;
    for (index = 0; index < cuts - 1; ++index) {
        float ratio;
        ci_xy_t p1, p2, p3, p4, p5, p6, sp2, sc2, sc1;
        if (!(0.0f <= at[index] && at[index + 1] <= 1.0f &&
              at[index] != at[index + 1]))
            continue;
        ratio = at[index] / at[index + 1];
        p1 = ci_lerp_xy(point_1, control_1, at[index + 1]);
        p2 = ci_lerp_xy(control_1, control_2, at[index + 1]);
        p3 = ci_lerp_xy(control_2, point_2, at[index + 1]);
        p4 = ci_lerp_xy(p1, p2, at[index + 1]);
        p5 = ci_lerp_xy(p2, p3, at[index + 1]);
        p6 = ci_lerp_xy(p1, p4, ratio);
        sp2 = ci_lerp_xy(p4, p5, at[index + 1]);
        sc2 = ci_lerp_xy(p4, sp2, ratio);
        sc1 = ci_lerp_xy(p6, sc2, ratio);
        ci_add_tessellation(ctx, split_point_1, sc1, sc2, sp2,
            angular, 20);
        split_point_1 = sp2;
    }
}

/* ======== PATH TO LINES ======== */

static void ci_path_to_lines(ci_canvas_t *ctx, int stroking)
{
    float ratio = 0.125f / CI_MAX(0.5f * ctx->line_width, 0.125f);
    float angular = stroking ?
        (ratio - 2.0f) * ratio * 2.0f + 1.0f : -1.0f;
    size_t index = 0;
    size_t ending = 0;
    size_t subpath;
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    for (subpath = 0; subpath < ctx->path.subpaths.size; ++subpath) {
        size_t first;
        ci_xy_t point_1;
        ending += ctx->path.subpaths.data[subpath].count;
        first = ctx->lines.points.size;
        point_1 = ctx->path.points.data[index++];
        ci_xy_array_push(&ctx->lines.points, point_1);
        for (; index < ending; index += 3) {
            ci_xy_t c1 = ctx->path.points.data[index + 0];
            ci_xy_t c2 = ctx->path.points.data[index + 1];
            ci_xy_t p2 = ctx->path.points.data[index + 2];
            ci_add_bezier(ctx, point_1, c1, c2, p2, angular);
            point_1 = p2;
        }
        ci_subpath_array_push(&ctx->lines.subpaths,
            ci_subpath_make(ctx->lines.points.size - first,
                ctx->path.subpaths.data[subpath].closed));
    }
}


/* ======== GLYPH RENDERING ======== */

static void ci_add_glyph(ci_canvas_t *ctx, int glyph, float angular)
{
    int loc_format = ci_unsigned_16(&ctx->face.data,
        ctx->face.head + 50);
    int offset = ctx->face.glyf + (loc_format ?
        ci_signed_32(&ctx->face.data, ctx->face.loca + glyph * 4) :
        ci_unsigned_16(&ctx->face.data,
            ctx->face.loca + glyph * 2) * 2);
    int next = ctx->face.glyf + (loc_format ?
        ci_signed_32(&ctx->face.data,
            ctx->face.loca + glyph * 4 + 4) :
        ci_unsigned_16(&ctx->face.data,
            ctx->face.loca + glyph * 2 + 2) * 2);
    int contours, hmetrics, left_side_bearing, x_min;
    int points_count, instructions, flags_array, flags_size, x_size;
    int x_array, y_array, x, y, flags, repeated, glyph_index;
    int contour;
    if (offset == next)
        return;
    contours = ci_signed_16(&ctx->face.data, offset);
    if (contours < 0) {
        offset += 10;
        for (;;) {
            int fl = ci_unsigned_16(&ctx->face.data, offset);
            int component = ci_unsigned_16(&ctx->face.data, offset + 2);
            float e, f, a_val, b_val, c_val, d_val;
            ci_affine_matrix_t saved_fwd, saved_inv;
            if (!(fl & 2)) return;
            e = (float)(fl & 1 ?
                ci_signed_16(&ctx->face.data, offset + 4) :
                ci_signed_8(&ctx->face.data, offset + 4));
            f = (float)(fl & 1 ?
                ci_signed_16(&ctx->face.data, offset + 6) :
                ci_signed_8(&ctx->face.data, offset + 5));
            offset += fl & 1 ? 8 : 6;
            a_val = fl & 200 ? (float)ci_signed_16(
                &ctx->face.data, offset) / 16384.0f : 1.0f;
            b_val = fl & 128 ? (float)ci_signed_16(
                &ctx->face.data, offset + 2) / 16384.0f : 0.0f;
            c_val = fl & 128 ? (float)ci_signed_16(
                &ctx->face.data, offset + 4) / 16384.0f : 0.0f;
            d_val = fl & 8 ? a_val :
                fl & 64 ? (float)ci_signed_16(
                    &ctx->face.data, offset + 2) / 16384.0f :
                fl & 128 ? (float)ci_signed_16(
                    &ctx->face.data, offset + 6) / 16384.0f :
                1.0f;
            offset += fl & 8 ? 2 : fl & 64 ? 4 : fl & 128 ? 8 : 0;
            saved_fwd = ctx->forward;
            saved_inv = ctx->inverse;
            ci_canvas_transform(ctx,
                a_val, b_val, c_val, d_val, e, f);
            ci_add_glyph(ctx, component, angular);
            ctx->forward = saved_fwd;
            ctx->inverse = saved_inv;
            if (!(fl & 32)) return;
        }
    }
    hmetrics = ci_unsigned_16(&ctx->face.data, ctx->face.hhea + 34);
    left_side_bearing = glyph < hmetrics ?
        ci_signed_16(&ctx->face.data, ctx->face.hmtx + glyph * 4 + 2) :
        ci_signed_16(&ctx->face.data,
            ctx->face.hmtx + hmetrics * 2 + glyph * 2);
    x_min = ci_signed_16(&ctx->face.data, offset + 2);
    points_count = ci_unsigned_16(&ctx->face.data,
        offset + 8 + contours * 2) + 1;
    instructions = ci_unsigned_16(&ctx->face.data,
        offset + 10 + contours * 2);
    flags_array = offset + 12 + contours * 2 + instructions;
    flags_size = 0;
    x_size = 0;
    for (glyph_index = 0; glyph_index < points_count;) {
        int fl = ci_unsigned_8(&ctx->face.data,
            flags_array + flags_size++);
        int rep = fl & 8 ? ci_unsigned_8(&ctx->face.data,
            flags_array + flags_size++) + 1 : 1;
        x_size += rep * (fl & 2 ? 1 : fl & 16 ? 0 : 2);
        glyph_index += rep;
    }
    x_array = flags_array + flags_size;
    y_array = x_array + x_size;
    x = left_side_bearing - x_min;
    y = 0;
    flags = 0;
    repeated = 0;
    glyph_index = 0;
    for (contour = 0; contour < contours; ++contour) {
        int beginning = glyph_index;
        int end_idx = ci_unsigned_16(&ctx->face.data,
            offset + 10 + contour * 2);
        ci_xy_t begin_point = ci_xy_make(0.0f, 0.0f);
        int begin_on = 0;
        ci_xy_t end_point = ci_xy_make(0.0f, 0.0f);
        int end_on = 0;
        size_t first = ctx->lines.points.size;
        for (; glyph_index <= end_idx; ++glyph_index) {
            ci_xy_t point;
            int on_curve;
            if (repeated)
                --repeated;
            else {
                flags = ci_unsigned_8(&ctx->face.data, flags_array++);
                if (flags & 8)
                    repeated = ci_unsigned_8(
                        &ctx->face.data, flags_array++);
            }
            if (flags & 2)
                x += ci_unsigned_8(&ctx->face.data, x_array) *
                    (flags & 16 ? 1 : -1);
            else if (!(flags & 16))
                x += ci_signed_16(&ctx->face.data, x_array);
            if (flags & 4)
                y += ci_unsigned_8(&ctx->face.data, y_array) *
                    (flags & 32 ? 1 : -1);
            else if (!(flags & 32))
                y += ci_signed_16(&ctx->face.data, y_array);
            x_array += flags & 2 ? 1 : flags & 16 ? 0 : 2;
            y_array += flags & 4 ? 1 : flags & 32 ? 0 : 2;
            point = ci_affine_mul(ctx->forward,
                ci_xy_make((float)x, (float)y));
            on_curve = flags & 1;
            if (glyph_index == beginning) {
                begin_point = point;
                begin_on = on_curve;
                if (on_curve)
                    ci_xy_array_push(&ctx->lines.points, point);
            } else {
                ci_xy_t point_2 = on_curve ? point :
                    ci_lerp_xy(end_point, point, 0.5f);
                if (ctx->lines.points.size == first ||
                    (end_on && on_curve))
                    ci_xy_array_push(&ctx->lines.points, point_2);
                else if (!end_on || on_curve) {
                    ci_xy_t pt1 = ctx->lines.points.data[
                        ctx->lines.points.size - 1];
                    ci_xy_t ctrl1 = ci_lerp_xy(
                        pt1, end_point, 2.0f / 3.0f);
                    ci_xy_t ctrl2 = ci_lerp_xy(
                        point_2, end_point, 2.0f / 3.0f);
                    ci_add_bezier(ctx, pt1, ctrl1, ctrl2,
                        point_2, angular);
                }
            }
            end_point = point;
            end_on = on_curve;
        }
        if (begin_on ^ end_on) {
            ci_xy_t pt1 = ctx->lines.points.data[
                ctx->lines.points.size - 1];
            ci_xy_t pt2 = ctx->lines.points.data[first];
            ci_xy_t ctrl = end_on ? begin_point : end_point;
            ci_xy_t ctrl1 = ci_lerp_xy(pt1, ctrl, 2.0f / 3.0f);
            ci_xy_t ctrl2 = ci_lerp_xy(pt2, ctrl, 2.0f / 3.0f);
            ci_add_bezier(ctx, pt1, ctrl1, ctrl2, pt2, angular);
        } else if (!begin_on && !end_on) {
            ci_xy_t pt1 = ctx->lines.points.data[
                ctx->lines.points.size - 1];
            ci_xy_t sp = ci_lerp_xy(begin_point, end_point, 0.5f);
            ci_xy_t pt2 = ctx->lines.points.data[first];
            ci_add_bezier(ctx, pt1,
                ci_lerp_xy(pt1, end_point, 2.0f / 3.0f),
                ci_lerp_xy(sp, end_point, 2.0f / 3.0f),
                sp, angular);
            ci_add_bezier(ctx, sp,
                ci_lerp_xy(sp, begin_point, 2.0f / 3.0f),
                ci_lerp_xy(pt2, begin_point, 2.0f / 3.0f),
                pt2, angular);
        }
        ci_xy_array_push(&ctx->lines.points,
            ctx->lines.points.data[first]);
        ci_subpath_array_push(&ctx->lines.subpaths,
            ci_subpath_make(ctx->lines.points.size - first, 1));
    }
}

/* ======== CHARACTER TO GLYPH ======== */

static int ci_character_to_glyph(ci_canvas_t *ctx,
    char const *text, int *index)
{
    int bytes = ((text[*index] & 0x80) == 0x00 ? 1 :
                 (text[*index] & 0xe0) == 0xc0 ? 2 :
                 (text[*index] & 0xf0) == 0xe0 ? 3 :
                 (text[*index] & 0xf8) == 0xf0 ? 4 : 0);
    int masks[] = {0x0, 0x7f, 0x1f, 0x0f, 0x07};
    int codepoint = bytes ? text[*index] & masks[bytes] : 0xfffd;
    int tables, format_12, format_4, format_0, table;
    ++(*index);
    while (--bytes > 0)
        if ((text[*index] & 0xc0) == 0x80)
            codepoint = codepoint << 6 | (text[(*index)++] & 0x3f);
        else {
            codepoint = 0xfffd;
            break;
        }
    if (codepoint == '\t' || codepoint == '\v' || codepoint == '\f' ||
        codepoint == '\r' || codepoint == '\n')
        codepoint = ' ';
    tables = ci_unsigned_16(&ctx->face.data, ctx->face.cmap + 2);
    format_12 = 0;
    format_4 = 0;
    format_0 = 0;
    for (table = 0; table < tables; ++table) {
        int platform = ci_unsigned_16(&ctx->face.data,
            ctx->face.cmap + table * 8 + 4);
        int encoding = ci_unsigned_16(&ctx->face.data,
            ctx->face.cmap + table * 8 + 6);
        int off = ci_signed_32(&ctx->face.data,
            ctx->face.cmap + table * 8 + 8);
        int format = ci_unsigned_16(&ctx->face.data,
            ctx->face.cmap + off);
        if (platform == 3 && encoding == 10 && format == 12)
            format_12 = ctx->face.cmap + off;
        else if (platform == 3 && encoding == 1 && format == 4)
            format_4 = ctx->face.cmap + off;
        else if (format == 0)
            format_0 = ctx->face.cmap + off;
    }
    if (format_12) {
        int groups = ci_signed_32(&ctx->face.data, format_12 + 12);
        int group;
        for (group = 0; group < groups; ++group) {
            int s = ci_signed_32(&ctx->face.data,
                format_12 + 16 + group * 12);
            int e = ci_signed_32(&ctx->face.data,
                format_12 + 20 + group * 12);
            int g = ci_signed_32(&ctx->face.data,
                format_12 + 24 + group * 12);
            if (s <= codepoint && codepoint <= e)
                return codepoint - s + g;
        }
    } else if (format_4) {
        int segments = ci_unsigned_16(&ctx->face.data,
            format_4 + 6);
        int end_arr = format_4 + 14;
        int start_arr = end_arr + 2 + segments;
        int delta_arr = start_arr + segments;
        int range_arr = delta_arr + segments;
        int seg;
        for (seg = 0; seg < segments; seg += 2) {
            int s = ci_unsigned_16(&ctx->face.data,
                start_arr + seg);
            int e = ci_unsigned_16(&ctx->face.data,
                end_arr + seg);
            int delta = ci_signed_16(&ctx->face.data,
                delta_arr + seg);
            int range = ci_unsigned_16(&ctx->face.data,
                range_arr + seg);
            if (s <= codepoint && codepoint <= e)
                return range ?
                    ci_unsigned_16(&ctx->face.data, range_arr +
                        seg + (codepoint - s) * 2 + range) :
                    (codepoint + delta) & 0xffff;
        }
    } else if (format_0 && 0 <= codepoint && codepoint < 256) {
        return ci_unsigned_8(&ctx->face.data, format_0 + 6 + codepoint);
    }
    return 0;
}

/* ======== TEXT TO LINES ======== */

static void ci_text_to_lines(ci_canvas_t *ctx, char const *text,
    ci_xy_t position, float maximum_width, int stroking)
{
    float ratio = 0.125f / CI_MAX(0.5f * ctx->line_width, 0.125f);
    float angular = stroking ?
        (ratio - 2.0f) * ratio * 2.0f + 1.0f : -1.0f;
    float width, reduction, units_per_em, ascender, descender;
    float normalize;
    ci_xy_t scaling;
    ci_affine_matrix_t saved_fwd, saved_inv;
    int hmetrics, place, glyph_idx;
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    if (ctx->face.data.size == 0 || !text || maximum_width <= 0.0f)
        return;
    width = (maximum_width == 1.0e30f &&
        ctx->text_align == CI_ALIGN_LEFTWARD) ?
        0.0f : ci_canvas_measure_text(ctx, text);
    reduction = maximum_width / CI_MAX(maximum_width, width);
    if (ctx->text_align == CI_ALIGN_RIGHTWARD)
        position.x -= width * reduction;
    else if (ctx->text_align == CI_ALIGN_CENTER)
        position.x -= 0.5f * width * reduction;
    scaling = ci_xy_mul(ci_xy_make(ctx->face.scale, ctx->face.scale),
        ci_xy_make(reduction, 1.0f));
    units_per_em = (float)ci_unsigned_16(&ctx->face.data,
        ctx->face.head + 18);
    ascender = (float)ci_signed_16(&ctx->face.data,
        ctx->face.os_2 + 68);
    descender = (float)ci_signed_16(&ctx->face.data,
        ctx->face.os_2 + 70);
    normalize = ctx->face.scale * units_per_em /
        (ascender - descender);
    if (ctx->text_baseline == CI_BASELINE_TOP)
        position.y += ascender * normalize;
    else if (ctx->text_baseline == CI_BASELINE_MIDDLE)
        position.y += (ascender + descender) * 0.5f * normalize;
    else if (ctx->text_baseline == CI_BASELINE_BOTTOM)
        position.y += descender * normalize;
    else if (ctx->text_baseline == CI_BASELINE_HANGING)
        position.y += 0.6f * ctx->face.scale * units_per_em;
    saved_fwd = ctx->forward;
    saved_inv = ctx->inverse;
    hmetrics = ci_unsigned_16(&ctx->face.data, ctx->face.hhea + 34);
    place = 0;
    for (glyph_idx = 0; text[glyph_idx];) {
        int glyph = ci_character_to_glyph(ctx, text, &glyph_idx);
        int entry;
        ctx->forward = saved_fwd;
        ci_canvas_transform(ctx, scaling.x, 0.0f, 0.0f, -scaling.y,
            position.x + (float)place * scaling.x, position.y);
        ci_add_glyph(ctx, glyph, angular);
        entry = CI_MIN(glyph, hmetrics - 1);
        place += ci_unsigned_16(&ctx->face.data,
            ctx->face.hmtx + entry * 4);
    }
    ctx->forward = saved_fwd;
    ctx->inverse = saved_inv;
}


/* ======== DASH LINES ======== */

static void ci_dash_lines(ci_canvas_t *ctx)
{
    float total = 0.0f;
    float dash_offset;
    size_t start = 0;
    size_t ending = 0;
    size_t subpath, i;
    if (ctx->line_dash.size == 0)
        return;
    for (i = 0; i < ctx->line_dash.size; ++i)
        total += ctx->line_dash.data[i];
    ci_xy_array_swap(&ctx->lines.points, &ctx->scratch.points);
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_swap(&ctx->lines.subpaths, &ctx->scratch.subpaths);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    dash_offset = ci_fmodf(ctx->line_dash_offset, total);
    if (dash_offset < 0.0f)
        dash_offset += total;
    while (dash_offset >= ctx->line_dash.data[start]) {
        dash_offset -= ctx->line_dash.data[start];
        start = start + 1 < ctx->line_dash.size ? start + 1 : 0;
    }
    for (subpath = 0; subpath < ctx->scratch.subpaths.size; ++subpath) {
        size_t index = ending;
        size_t first, segment, merge_point, merge_subpath;
        int emit, merge_emit;
        float next_dash;
        ending += ctx->scratch.subpaths.data[subpath].count;
        first = ctx->lines.points.size;
        segment = start;
        emit = ~start & 1;
        merge_point = ctx->lines.points.size;
        merge_subpath = ctx->lines.subpaths.size;
        merge_emit = emit;
        next_dash = ctx->line_dash.data[start] - dash_offset;
        for (; index + 1 < ending; ++index) {
            ci_xy_t from = ctx->scratch.points.data[index];
            ci_xy_t to = ctx->scratch.points.data[index + 1];
            float line_len;
            if (emit)
                ci_xy_array_push(&ctx->lines.points, from);
            line_len = ci_length(ci_xy_sub(
                ci_affine_mul(ctx->inverse, to),
                ci_affine_mul(ctx->inverse, from)));
            while (next_dash < line_len) {
                ci_xy_array_push(&ctx->lines.points,
                    ci_lerp_xy(from, to, next_dash / line_len));
                if (emit) {
                    ci_subpath_array_push(&ctx->lines.subpaths,
                        ci_subpath_make(
                            ctx->lines.points.size - first, 0));
                    first = ctx->lines.points.size;
                }
                segment = segment + 1 < ctx->line_dash.size ?
                    segment + 1 : 0;
                emit = !emit;
                next_dash += ctx->line_dash.data[segment];
            }
            next_dash -= line_len;
        }
        if (emit) {
            ci_xy_array_push(&ctx->lines.points,
                ctx->scratch.points.data[index]);
            ci_subpath_array_push(&ctx->lines.subpaths,
                ci_subpath_make(ctx->lines.points.size - first, 0));
            if (ctx->scratch.subpaths.data[subpath].closed &&
                    merge_emit) {
                if (ctx->lines.subpaths.size == merge_subpath + 1) {
                    ctx->lines.subpaths.data[
                        ctx->lines.subpaths.size - 1].closed = 1;
                } else {
                    size_t cnt = ctx->lines.subpaths.data[
                        ctx->lines.subpaths.size - 1].count;
                    size_t rot_start = merge_point;
                    size_t rot_mid = ctx->lines.points.size - cnt;
                    size_t rot_end = ctx->lines.points.size;
                    /* manual rotate: move last cnt elements to
                       merge_point */
                    size_t block = rot_end - rot_mid;
                    size_t j;
                    ci_xy_t *tmp = (ci_xy_t *)malloc(
                        block * sizeof(ci_xy_t));
                    if (tmp) {
                        memcpy(tmp, ctx->lines.points.data + rot_mid,
                            block * sizeof(ci_xy_t));
                        for (j = rot_mid; j > rot_start; --j)
                            ctx->lines.points.data[j + block - 1] =
                                ctx->lines.points.data[j - 1];
                        memcpy(ctx->lines.points.data + rot_start,
                            tmp, block * sizeof(ci_xy_t));
                        free(tmp);
                    }
                    ctx->lines.subpaths.data[merge_subpath].count +=
                        cnt;
                    ci_subpath_array_pop(&ctx->lines.subpaths);
                }
            }
        }
    }
}

/* ======== HALF STROKE ======== */

static void ci_add_half_stroke(ci_canvas_t *ctx, size_t beginning,
    size_t ending, int closed)
{
    float half = ctx->line_width * 0.5f;
    float ratio = ctx->miter_limit * ctx->miter_limit * half * half;
    ci_xy_t in_direction = ci_xy_make(0.0f, 0.0f);
    float in_length = 0.0f;
    ci_xy_t point = ci_affine_mul(ctx->inverse,
        ctx->scratch.points.data[beginning]);
    size_t finish = beginning;
    size_t index = beginning;
    do {
        ci_xy_t next_pt = ci_affine_mul(ctx->inverse,
            ctx->scratch.points.data[index]);
        ci_xy_t out_direction = ci_normalized(
            ci_xy_sub(next_pt, point));
        float out_length = ci_length(ci_xy_sub(next_pt, point));
        if (in_length != 0.0f && out_length >= 1.0e-4f) {
            ci_xy_t side_in, side_out, offset_vec;
            float turn;
            int tight;
            if (closed && finish == beginning)
                finish = index;
            side_in = ci_xy_add(point,
                ci_xy_scale(half, ci_perpendicular(in_direction)));
            side_out = ci_xy_add(point,
                ci_xy_scale(half, ci_perpendicular(out_direction)));
            turn = ci_dot(ci_perpendicular(in_direction),
                out_direction);
            if (ci_fabsf(turn) < 1.0e-4f)
                turn = 0.0f;
            offset_vec = turn == 0.0f ?
                ci_xy_make(0.0f, 0.0f) :
                ci_xy_scale(half / turn,
                    ci_xy_sub(out_direction, in_direction));
            tight = (ci_dot(offset_vec, in_direction) < -in_length &&
                     ci_dot(offset_vec, out_direction) > out_length);
            if (turn > 0.0f && tight) {
                ci_xy_t tmp_xy = side_in;
                ci_xy_t tmp_dir;
                side_in = side_out;
                side_out = tmp_xy;
                tmp_dir = in_direction;
                in_direction = out_direction;
                out_direction = tmp_dir;
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_out));
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, point));
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_in));
            }
            if ((turn > 0.0f && !tight) ||
                (turn != 0.0f &&
                 ctx->line_join == CI_JOIN_MITER &&
                 ci_dot(offset_vec, offset_vec) <= ratio)) {
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward,
                        ci_xy_add(point, offset_vec)));
            } else if (ctx->line_join == CI_JOIN_ROUNDED) {
                float cos_val = ci_dot(in_direction, out_direction);
                float angle = ci_acosf(
                    CI_CLAMP(cos_val, -1.0f, 1.0f));
                float alpha = 4.0f / 3.0f *
                    ci_tanf(0.25f * angle);
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_in));
                ci_add_bezier(ctx,
                    ci_affine_mul(ctx->forward, side_in),
                    ci_affine_mul(ctx->forward, ci_xy_add(side_in,
                        ci_xy_scale(alpha * half, in_direction))),
                    ci_affine_mul(ctx->forward, ci_xy_sub(side_out,
                        ci_xy_scale(alpha * half, out_direction))),
                    ci_affine_mul(ctx->forward, side_out),
                    -1.0f);
            } else {
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_in));
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_out));
            }
            if (turn > 0.0f && tight) {
                ci_xy_t tmp_dir;
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_out));
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, point));
                ci_xy_array_push(&ctx->lines.points,
                    ci_affine_mul(ctx->forward, side_in));
                tmp_dir = in_direction;
                in_direction = out_direction;
                out_direction = tmp_dir;
            }
        }
        if (out_length >= 1.0e-4f) {
            in_direction = out_direction;
            in_length = out_length;
            point = next_pt;
        }
        index = (index == ending ? beginning :
                 ending > beginning ? index + 1 : index - 1);
    } while (index != finish);
    if (closed || in_length == 0.0f)
        return;
    {
        ci_xy_t ahead = ci_xy_scale(half, in_direction);
        ci_xy_t side = ci_perpendicular(ahead);
        if (ctx->line_cap == CI_CAP_BUTT) {
            ci_xy_array_push(&ctx->lines.points,
                ci_affine_mul(ctx->forward,
                    ci_xy_add(point, side)));
            ci_xy_array_push(&ctx->lines.points,
                ci_affine_mul(ctx->forward,
                    ci_xy_sub(point, side)));
        } else if (ctx->line_cap == CI_CAP_SQUARE) {
            ci_xy_array_push(&ctx->lines.points,
                ci_affine_mul(ctx->forward,
                    ci_xy_add(ci_xy_add(point, ahead), side)));
            ci_xy_array_push(&ctx->lines.points,
                ci_affine_mul(ctx->forward,
                    ci_xy_sub(ci_xy_add(point, ahead), side)));
        } else if (ctx->line_cap == CI_CAP_CIRCLE) {
            float alpha_cap = 0.55228475f;
            ci_xy_array_push(&ctx->lines.points,
                ci_affine_mul(ctx->forward,
                    ci_xy_add(point, side)));
            ci_add_bezier(ctx,
                ci_affine_mul(ctx->forward,
                    ci_xy_add(point, side)),
                ci_affine_mul(ctx->forward, ci_xy_add(
                    ci_xy_add(point, side),
                    ci_xy_scale(alpha_cap, ahead))),
                ci_affine_mul(ctx->forward, ci_xy_add(
                    ci_xy_add(point, ahead),
                    ci_xy_scale(alpha_cap, side))),
                ci_affine_mul(ctx->forward,
                    ci_xy_add(point, ahead)),
                -1.0f);
            ci_add_bezier(ctx,
                ci_affine_mul(ctx->forward,
                    ci_xy_add(point, ahead)),
                ci_affine_mul(ctx->forward, ci_xy_sub(
                    ci_xy_add(point, ahead),
                    ci_xy_scale(alpha_cap, side))),
                ci_affine_mul(ctx->forward, ci_xy_add(
                    ci_xy_sub(point, side),
                    ci_xy_scale(alpha_cap, ahead))),
                ci_affine_mul(ctx->forward,
                    ci_xy_sub(point, side)),
                -1.0f);
        }
    }
}

/* ======== STROKE LINES ======== */

static void ci_stroke_lines(ci_canvas_t *ctx)
{
    size_t ending = 0;
    size_t subpath;
    if (ctx->forward.a * ctx->forward.d -
        ctx->forward.b * ctx->forward.c == 0.0f)
        return;
    ci_dash_lines(ctx);
    ci_xy_array_swap(&ctx->lines.points, &ctx->scratch.points);
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_swap(&ctx->lines.subpaths,
        &ctx->scratch.subpaths);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    for (subpath = 0; subpath < ctx->scratch.subpaths.size;
         ++subpath) {
        size_t beginning = ending;
        size_t first;
        ending += ctx->scratch.subpaths.data[subpath].count;
        if (ending - beginning < 2)
            continue;
        first = ctx->lines.points.size;
        ci_add_half_stroke(ctx, beginning, ending - 1,
            ctx->scratch.subpaths.data[subpath].closed);
        if (ctx->scratch.subpaths.data[subpath].closed) {
            ci_subpath_array_push(&ctx->lines.subpaths,
                ci_subpath_make(
                    ctx->lines.points.size - first, 1));
            first = ctx->lines.points.size;
        }
        ci_add_half_stroke(ctx, ending - 1, beginning,
            ctx->scratch.subpaths.data[subpath].closed);
        ci_subpath_array_push(&ctx->lines.subpaths,
            ci_subpath_make(ctx->lines.points.size - first, 1));
    }
}

/* ======== SCAN CONVERSION ======== */

static void ci_add_runs(ci_canvas_t *ctx, ci_xy_t from, ci_xy_t to)
{
    float sign, y_step;
    ci_xy_t now, pixel, corner, slope, next_x, next_y;
    if (ci_fabsf(to.y - from.y) < 2.0e-5f)
        return;
    sign = to.y > from.y ? 1.0f : -1.0f;
    if (from.x > to.x) {
        ci_xy_t tmp = from; from = to; to = tmp;
    }
    now = from;
    pixel = ci_xy_make(ci_floorf(now.x), ci_floorf(now.y));
    corner = ci_xy_add(pixel, ci_xy_make(1.0f,
        to.y > from.y ? 1.0f : 0.0f));
    slope = ci_xy_make((to.x - from.x) / (to.y - from.y),
                       (to.y - from.y) / (to.x - from.x));
    next_x = (to.x - from.x < 2.0e-5f) ? to :
        ci_xy_make(corner.x, now.y + (corner.x - now.x) * slope.y);
    next_y = ci_xy_make(now.x + (corner.y - now.y) * slope.x,
        corner.y);
    if ((from.y < to.y && to.y < next_y.y) ||
        (from.y > to.y && to.y > next_y.y))
        next_y = to;
    y_step = to.y > from.y ? 1.0f : -1.0f;
    do {
        float carry = 0.0f;
        while (next_x.x < next_y.x) {
            float strip = CI_CLAMP(
                (next_x.y - now.y) * y_step, 0.0f, 1.0f);
            float mid = (next_x.x + now.x) * 0.5f;
            float area = (mid - pixel.x) * strip;
            ci_run_array_push(&ctx->runs,
                ci_run_make((unsigned short)pixel.x,
                    (unsigned short)pixel.y,
                    (carry + strip - area) * sign));
            carry = area;
            now = next_x;
            next_x.x += 1.0f;
            next_x.y = (next_x.x - from.x) * slope.y + from.y;
            pixel.x += 1.0f;
        }
        {
            float strip = CI_CLAMP(
                (next_y.y - now.y) * y_step, 0.0f, 1.0f);
            float mid = (next_y.x + now.x) * 0.5f;
            float area = (mid - pixel.x) * strip;
            ci_run_array_push(&ctx->runs,
                ci_run_make((unsigned short)pixel.x,
                    (unsigned short)pixel.y,
                    (carry + strip - area) * sign));
            ci_run_array_push(&ctx->runs,
                ci_run_make((unsigned short)(pixel.x + 1.0f),
                    (unsigned short)pixel.y, area * sign));
        }
        now = next_y;
        next_y.y += y_step;
        next_y.x = (next_y.y - from.y) * slope.x + from.x;
        pixel.y += y_step;
        if ((from.y < to.y && to.y < next_y.y) ||
            (from.y > to.y && to.y > next_y.y))
            next_y = to;
    } while (now.y != to.y);
}

static int ci_run_compare(const void *a, const void *b)
{
    const ci_pixel_run_t *left = (const ci_pixel_run_t *)a;
    const ci_pixel_run_t *right = (const ci_pixel_run_t *)b;
    if (left->y != right->y) return left->y < right->y ? -1 : 1;
    if (left->x != right->x) return left->x < right->x ? -1 : 1;
    if (ci_fabsf(left->delta) < ci_fabsf(right->delta)) return -1;
    if (ci_fabsf(left->delta) > ci_fabsf(right->delta)) return 1;
    return 0;
}

static void ci_lines_to_runs(ci_canvas_t *ctx, ci_xy_t offset,
    int padding)
{
    float width = (float)(ctx->size_x + padding);
    float height = (float)(ctx->size_y + padding);
    size_t ending = 0;
    size_t subpath, to;
    ci_run_array_clear(&ctx->runs);
    for (subpath = 0; subpath < ctx->lines.subpaths.size; ++subpath) {
        size_t beginning = ending;
        size_t index, last;
        int edge;
        ending += ctx->lines.subpaths.data[subpath].count;
        ci_xy_array_clear(&ctx->scratch.points);
        for (index = beginning; index < ending; ++index)
            ci_xy_array_push(&ctx->scratch.points,
                ci_xy_add(offset, ctx->lines.points.data[index]));
        for (edge = 0; edge < 4; ++edge) {
            ci_xy_t normal = ci_xy_make(
                edge == 0 ? 1.0f : edge == 2 ? -1.0f : 0.0f,
                edge == 1 ? 1.0f : edge == 3 ? -1.0f : 0.0f);
            float place = edge == 2 ? width :
                edge == 3 ? height : 0.0f;
            size_t first = ctx->scratch.points.size;
            for (index = 0; index < first; ++index) {
                ci_xy_t from_pt = ctx->scratch.points.data[
                    (index ? index : first) - 1];
                ci_xy_t to_pt = ctx->scratch.points.data[index];
                float from_side = ci_dot(from_pt, normal) + place;
                float to_side = ci_dot(to_pt, normal) + place;
                if (from_side * to_side < 0.0f)
                    ci_xy_array_push(&ctx->scratch.points,
                        ci_lerp_xy(from_pt, to_pt,
                            from_side / (from_side - to_side)));
                if (to_side >= 0.0f)
                    ci_xy_array_push(&ctx->scratch.points, to_pt);
            }
            ci_xy_array_erase(&ctx->scratch.points, 0, first);
        }
        last = ctx->scratch.points.size;
        for (index = 0; index < last; ++index) {
            ci_xy_t from_pt = ctx->scratch.points.data[
                (index ? index : last) - 1];
            ci_xy_t to_pt = ctx->scratch.points.data[index];
            ci_add_runs(ctx,
                ci_xy_make(CI_CLAMP(from_pt.x, 0.0f, width),
                           CI_CLAMP(from_pt.y, 0.0f, height)),
                ci_xy_make(CI_CLAMP(to_pt.x, 0.0f, width),
                           CI_CLAMP(to_pt.y, 0.0f, height)));
        }
    }
    if (ctx->runs.size == 0)
        return;
    qsort(ctx->runs.data, ctx->runs.size, sizeof(ci_pixel_run_t),
        ci_run_compare);
    to = 0;
    {
        size_t from;
        for (from = 1; from < ctx->runs.size; ++from)
            if (ctx->runs.data[from].x == ctx->runs.data[to].x &&
                ctx->runs.data[from].y == ctx->runs.data[to].y)
                ctx->runs.data[to].delta +=
                    ctx->runs.data[from].delta;
            else if (ctx->runs.data[from].delta != 0.0f)
                ctx->runs.data[++to] = ctx->runs.data[from];
    }
    ctx->runs.size = to + 1;
}


/* ======== PAINT PIXEL ======== */

static ci_rgba_t ci_paint_pixel(ci_canvas_t *ctx, ci_xy_t point,
    ci_paint_brush_t const *brush)
{
    if (brush->colors.size == 0)
        return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
    if (brush->type == CI_PAINT_COLOR)
        return brush->colors.data[0];
    point = ci_affine_mul(ctx->inverse, point);
    if (brush->type == CI_PAINT_PATTERN) {
        float w = (float)brush->width;
        float h = (float)brush->height;
        float scale_x, scale_y, reciprocal_x, reciprocal_y;
        int left, top_val, right, bottom_val, pattern_y;
        ci_rgba_t total_color;
        float total_weight;
        int is_image = (brush == &ctx->image_brush);
        if (((brush->repetition & 2) &&
             (point.x < 0.0f || w <= point.x)) ||
            ((brush->repetition & 1) &&
             (point.y < 0.0f || h <= point.y)))
            return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
        scale_x = ci_fabsf(ctx->inverse.a) +
            ci_fabsf(ctx->inverse.c);
        scale_y = ci_fabsf(ctx->inverse.b) +
            ci_fabsf(ctx->inverse.d);
        scale_x = CI_CLAMP(scale_x, 1.0f, w * 0.25f);
        scale_y = CI_CLAMP(scale_y, 1.0f, h * 0.25f);
        reciprocal_x = 1.0f / scale_x;
        reciprocal_y = 1.0f / scale_y;
        point = ci_xy_sub(point, ci_xy_make(0.5f, 0.5f));
        left = (int)ci_ceilf(point.x - scale_x * 2.0f);
        top_val = (int)ci_ceilf(point.y - scale_y * 2.0f);
        right = (int)ci_ceilf(point.x + scale_x * 2.0f);
        bottom_val = (int)ci_ceilf(point.y + scale_y * 2.0f);
        total_color = ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
        total_weight = 0.0f;
        for (pattern_y = top_val; pattern_y < bottom_val;
             ++pattern_y) {
            float yf = ci_fabsf(reciprocal_y *
                ((float)pattern_y - point.y));
            float weight_y = yf < 1.0f ?
                (1.5f * yf - 2.5f) * yf * yf + 1.0f :
                ((-0.5f * yf + 2.5f) * yf - 4.0f) * yf + 2.0f;
            int wrapped_y = pattern_y % brush->height;
            int pattern_x;
            if (wrapped_y < 0)
                wrapped_y += brush->height;
            if (is_image)
                wrapped_y = CI_CLAMP(pattern_y, 0,
                    brush->height - 1);
            for (pattern_x = left; pattern_x < right; ++pattern_x) {
                float xf = ci_fabsf(reciprocal_x *
                    ((float)pattern_x - point.x));
                float weight_x = xf < 1.0f ?
                    (1.5f * xf - 2.5f) * xf * xf + 1.0f :
                    ((-0.5f * xf + 2.5f) * xf - 4.0f) * xf + 2.0f;
                int wrapped_x = pattern_x % brush->width;
                float weight;
                size_t idx;
                if (wrapped_x < 0)
                    wrapped_x += brush->width;
                if (is_image)
                    wrapped_x = CI_CLAMP(pattern_x, 0,
                        brush->width - 1);
                weight = weight_x * weight_y;
                idx = (size_t)(wrapped_y * brush->width + wrapped_x);
                total_color = ci_rgba_add(total_color,
                    ci_rgba_scale(weight,
                        brush->colors.data[idx]));
                total_weight += weight;
            }
        }
        return ci_rgba_scale(1.0f / total_weight, total_color);
    }
    /* gradient */
    {
        float grad_offset;
        ci_xy_t relative = ci_xy_sub(point, brush->start);
        ci_xy_t line = ci_xy_sub(brush->end, brush->start);
        float gradient = ci_dot(relative, line);
        float span = ci_dot(line, line);
        size_t idx;
        float mix;
        ci_rgba_t delta;
        if (brush->type == CI_PAINT_LINEAR) {
            if (span == 0.0f)
                return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
            grad_offset = gradient / span;
        } else {
            float initial = brush->start_radius;
            float change = brush->end_radius - initial;
            float a_coeff = span - change * change;
            float b_coeff = -2.0f * (gradient + initial * change);
            float c_coeff = ci_dot(relative, relative) -
                initial * initial;
            float disc = b_coeff * b_coeff -
                4.0f * a_coeff * c_coeff;
            float root, recip, off1, off2, r1, r2;
            if (disc < 0.0f || (span == 0.0f && change == 0.0f))
                return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
            root = ci_sqrtf(disc);
            recip = 1.0f / (2.0f * a_coeff);
            off1 = (-b_coeff - root) * recip;
            off2 = (-b_coeff + root) * recip;
            r1 = initial + change * off1;
            r2 = initial + change * off2;
            if (r2 >= 0.0f)
                grad_offset = off2;
            else if (r1 >= 0.0f)
                grad_offset = off1;
            else
                return ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
        }
        /* binary search (upper_bound) */
        {
            size_t lo = 0, hi = brush->stops.size;
            while (lo < hi) {
                size_t mid = lo + (hi - lo) / 2;
                if (brush->stops.data[mid] <= grad_offset)
                    lo = mid + 1;
                else
                    hi = mid;
            }
            idx = lo;
        }
        if (idx == 0)
            return ci_premultiplied(brush->colors.data[0]);
        if (idx == brush->stops.size)
            return ci_premultiplied(
                brush->colors.data[brush->stops.size - 1]);
        mix = (grad_offset - brush->stops.data[idx - 1]) /
              (brush->stops.data[idx] - brush->stops.data[idx - 1]);
        delta = ci_rgba_sub(brush->colors.data[idx],
            brush->colors.data[idx - 1]);
        return ci_premultiplied(ci_rgba_add(
            brush->colors.data[idx - 1],
            ci_rgba_scale(mix, delta)));
    }
}

/* ======== RENDER SHADOW ======== */

static void ci_render_shadow(ci_canvas_t *ctx,
    ci_paint_brush_t const *brush)
{
    float sigma_squared;
    size_t radius;
    int border;
    ci_xy_t offset;
    int left_val, right_val, top_val, bottom_val;
    size_t w, h, working;
    float alpha_val, divisor, weight_1, weight_2;
    size_t y_idx;
    int operation, x_var, y_var;
    float sum;
    size_t idx;
    if (ctx->shadow_color.a == 0.0f ||
        (ctx->shadow_blur == 0.0f &&
         ctx->shadow_offset_x == 0.0f &&
         ctx->shadow_offset_y == 0.0f))
        return;
    sigma_squared = 0.25f * ctx->shadow_blur * ctx->shadow_blur;
    radius = (size_t)(0.5f * ci_sqrtf(
        4.0f * sigma_squared + 1.0f) - 0.5f);
    border = 3 * ((int)radius + 1);
    offset = ci_xy_make((float)border + ctx->shadow_offset_x,
                        (float)border + ctx->shadow_offset_y);
    ci_lines_to_runs(ctx, offset, 2 * border);
    left_val = ctx->size_x + 2 * border;
    right_val = 0;
    top_val = ctx->size_y + 2 * border;
    bottom_val = 0;
    for (idx = 0; idx < ctx->runs.size; ++idx) {
        left_val = CI_MIN(left_val, (int)ctx->runs.data[idx].x);
        right_val = CI_MAX(right_val, (int)ctx->runs.data[idx].x);
        top_val = CI_MIN(top_val, (int)ctx->runs.data[idx].y);
        bottom_val = CI_MAX(bottom_val, (int)ctx->runs.data[idx].y);
    }
    left_val = CI_MAX(left_val - border, 0);
    right_val = CI_MIN(right_val + border,
        ctx->size_x + 2 * border) + 1;
    top_val = CI_MAX(top_val - border, 0);
    bottom_val = CI_MIN(bottom_val + border,
        ctx->size_y + 2 * border);
    w = (size_t)CI_MAX(right_val - left_val, 0);
    h = (size_t)CI_MAX(bottom_val - top_val, 0);
    working = w * h;
    ci_float_array_clear(&ctx->shadow);
    ci_float_array_resize(&ctx->shadow,
        working + CI_MAX(w, h));
    /* rasterize alpha */
    {
        int rx = -1, ry = -1;
        float rsum = 0.0f;
        for (idx = 0; idx < ctx->runs.size; ++idx) {
            ci_pixel_run_t nxt = ctx->runs.data[idx];
            float coverage = CI_MIN(ci_fabsf(rsum), 1.0f);
            int rto = nxt.y == ry ? nxt.x : rx + 1;
            if (coverage >= 1.0f / 8160.0f)
                for (; rx < rto; ++rx)
                    ctx->shadow.data[
                        (size_t)(ry - top_val) * w +
                        (size_t)(rx - left_val)] = coverage *
                        ci_paint_pixel(ctx,
                            ci_xy_sub(ci_xy_make(
                                (float)rx + 0.5f,
                                (float)ry + 0.5f), offset),
                            brush).a;
            if (nxt.y != ry) rsum = 0.0f;
            rx = nxt.x;
            ry = nxt.y;
            rsum += nxt.delta;
        }
    }
    /* box blur */
    alpha_val = (float)(2 * radius + 1) *
        ((float)(radius * (radius + 1)) - sigma_squared) /
        (2.0f * sigma_squared -
         (float)(6 * (radius + 1) * (radius + 1)));
    divisor = 2.0f * (alpha_val + (float)radius) + 1.0f;
    weight_1 = alpha_val / divisor;
    weight_2 = (1.0f - alpha_val) / divisor;
    for (y_idx = 0; y_idx < h; ++y_idx) {
        int pass;
        for (pass = 0; pass < 3; ++pass) {
            size_t x_idx;
            float running;
            for (x_idx = 0; x_idx < w; ++x_idx)
                ctx->shadow.data[working + x_idx] =
                    ctx->shadow.data[y_idx * w + x_idx];
            running = weight_1 *
                ctx->shadow.data[working + radius + 1];
            for (x_idx = 0; x_idx <= radius; ++x_idx)
                running += (weight_1 + weight_2) *
                    ctx->shadow.data[working + x_idx];
            ctx->shadow.data[y_idx * w] = running;
            for (x_idx = 1; x_idx < w; ++x_idx) {
                if (x_idx >= radius + 1)
                    running -= weight_2 *
                        ctx->shadow.data[
                            working + x_idx - radius - 1];
                if (x_idx >= radius + 2)
                    running -= weight_1 *
                        ctx->shadow.data[
                            working + x_idx - radius - 2];
                if (x_idx + radius < w)
                    running += weight_2 *
                        ctx->shadow.data[
                            working + x_idx + radius];
                if (x_idx + radius + 1 < w)
                    running += weight_1 *
                        ctx->shadow.data[
                            working + x_idx + radius + 1];
                ctx->shadow.data[y_idx * w + x_idx] = running;
            }
        }
    }
    {
        size_t x_idx;
        for (x_idx = 0; x_idx < w; ++x_idx) {
            int pass;
            for (pass = 0; pass < 3; ++pass) {
                float running;
                for (y_idx = 0; y_idx < h; ++y_idx)
                    ctx->shadow.data[working + y_idx] =
                        ctx->shadow.data[y_idx * w + x_idx];
                running = weight_1 *
                    ctx->shadow.data[working + radius + 1];
                for (y_idx = 0; y_idx <= radius; ++y_idx)
                    running += (weight_1 + weight_2) *
                        ctx->shadow.data[working + y_idx];
                ctx->shadow.data[x_idx] = running;
                for (y_idx = 1; y_idx < h; ++y_idx) {
                    if (y_idx >= radius + 1)
                        running -= weight_2 *
                            ctx->shadow.data[
                                working + y_idx - radius - 1];
                    if (y_idx >= radius + 2)
                        running -= weight_1 *
                            ctx->shadow.data[
                                working + y_idx - radius - 2];
                    if (y_idx + radius < h)
                        running += weight_2 *
                            ctx->shadow.data[
                                working + y_idx + radius];
                    if (y_idx + radius + 1 < h)
                        running += weight_1 *
                            ctx->shadow.data[
                                working + y_idx + radius + 1];
                    ctx->shadow.data[y_idx * w + x_idx] = running;
                }
            }
        }
    }
    /* composite shadow */
    operation = (int)ctx->global_composite_operation;
    x_var = -1;
    y_var = -1;
    sum = 0.0f;
    for (idx = 0; idx < ctx->mask.size; ++idx) {
        ci_pixel_run_t nxt = ctx->mask.data[idx];
        float visibility = CI_MIN(ci_fabsf(sum), 1.0f);
        int to_x = CI_MIN(
            nxt.y == y_var ? nxt.x : x_var + 1,
            right_val - border);
        if (visibility >= 1.0f / 8160.0f &&
            top_val <= y_var + border &&
            y_var + border < bottom_val)
            for (; x_var < to_x; ++x_var) {
                ci_rgba_t *back = &ctx->bitmap[
                    y_var * ctx->size_x + x_var];
                ci_rgba_t fore = ci_rgba_scale(
                    ctx->global_alpha *
                    ctx->shadow.data[
                        (size_t)(y_var + border - top_val) * w +
                        (size_t)(x_var + border - left_val)],
                    ctx->shadow_color);
                float mix_fore = operation & 1 ? back->a : 0.0f;
                float mix_back = operation & 4 ? fore.a : 0.0f;
                ci_rgba_t blend;
                if (operation & 2) mix_fore = 1.0f - mix_fore;
                if (operation & 8) mix_back = 1.0f - mix_back;
                blend = ci_rgba_add(ci_rgba_scale(mix_fore, fore),
                    ci_rgba_scale(mix_back, *back));
                blend.a = CI_MIN(blend.a, 1.0f);
                *back = ci_rgba_add(ci_rgba_scale(visibility, blend),
                    ci_rgba_scale(1.0f - visibility, *back));
            }
        if (nxt.y != y_var) sum = 0.0f;
        x_var = CI_MAX((int)nxt.x, left_val - border);
        y_var = nxt.y;
        sum += nxt.delta;
    }
}

/* ======== RENDER MAIN ======== */

static void ci_render_main(ci_canvas_t *ctx,
    ci_paint_brush_t const *brush)
{
    int operation;
    int x_var = -1, y_var = -1;
    float path_sum = 0.0f, clip_sum = 0.0f;
    size_t path_index = 0, clip_index = 0;
    if (ctx->forward.a * ctx->forward.d -
        ctx->forward.b * ctx->forward.c == 0.0f)
        return;
    ci_render_shadow(ctx, brush);
    ci_lines_to_runs(ctx, ci_xy_make(0.0f, 0.0f), 0);
    operation = (int)ctx->global_composite_operation;
    while (clip_index < ctx->mask.size) {
        int which = (path_index < ctx->runs.size &&
            ci_run_compare(&ctx->runs.data[path_index],
                &ctx->mask.data[clip_index]) < 0);
        ci_pixel_run_t nxt = which ?
            ctx->runs.data[path_index] :
            ctx->mask.data[clip_index];
        float coverage = CI_MIN(ci_fabsf(path_sum), 1.0f);
        float visibility = CI_MIN(ci_fabsf(clip_sum), 1.0f);
        int to_x = nxt.y == y_var ? nxt.x : x_var + 1;
        if ((coverage >= 1.0f / 8160.0f || ~operation & 8) &&
            visibility >= 1.0f / 8160.0f)
            for (; x_var < to_x; ++x_var) {
                ci_rgba_t *back = &ctx->bitmap[
                    y_var * ctx->size_x + x_var];
                ci_rgba_t fore = ci_rgba_scale(
                    coverage * ctx->global_alpha,
                    ci_paint_pixel(ctx,
                        ci_xy_make((float)x_var + 0.5f,
                                   (float)y_var + 0.5f), brush));
                float mix_fore = operation & 1 ? back->a : 0.0f;
                float mix_back = operation & 4 ? fore.a : 0.0f;
                ci_rgba_t blend;
                if (operation & 2) mix_fore = 1.0f - mix_fore;
                if (operation & 8) mix_back = 1.0f - mix_back;
                blend = ci_rgba_add(ci_rgba_scale(mix_fore, fore),
                    ci_rgba_scale(mix_back, *back));
                blend.a = CI_MIN(blend.a, 1.0f);
                *back = ci_rgba_add(ci_rgba_scale(visibility, blend),
                    ci_rgba_scale(1.0f - visibility, *back));
            }
        x_var = nxt.x;
        if (nxt.y != y_var) {
            y_var = nxt.y;
            path_sum = 0.0f;
            clip_sum = 0.0f;
        }
        if (which)
            path_sum += ctx->runs.data[path_index++].delta;
        else
            clip_sum += ctx->mask.data[clip_index++].delta;
    }
}


/* ======== CANVAS LIFECYCLE ======== */

static void ci_canvas_free_internals(ci_canvas_t *ctx)
{
    ci_float_array_free(&ctx->shadow);
    ci_float_array_free(&ctx->line_dash);
    ci_paint_brush_free(&ctx->fill_brush);
    ci_paint_brush_free(&ctx->stroke_brush);
    ci_paint_brush_free(&ctx->image_brush);
    ci_bezier_path_free(&ctx->path);
    ci_line_path_free(&ctx->lines);
    ci_line_path_free(&ctx->scratch);
    ci_run_array_free(&ctx->runs);
    ci_run_array_free(&ctx->mask);
    ci_font_face_free(&ctx->face);
}

ci_canvas_t *ci_canvas_create(int width, int height)
{
    ci_canvas_t *ctx;
    ci_affine_matrix_t identity;
    unsigned short y;
    if (width < 1 || width > 32768 || height < 1 || height > 32768)
        return NULL;
    ctx = (ci_canvas_t *)calloc(1, sizeof(ci_canvas_t));
    if (!ctx) return NULL;
    ctx->size_x = width;
    ctx->size_y = height;
    ctx->global_composite_operation = CI_SOURCE_OVER;
    ctx->shadow_offset_x = 0.0f;
    ctx->shadow_offset_y = 0.0f;
    ctx->line_cap = CI_CAP_BUTT;
    ctx->line_join = CI_JOIN_MITER;
    ctx->line_dash_offset = 0.0f;
    ctx->text_align = CI_ALIGN_START;
    ctx->text_baseline = CI_BASELINE_ALPHABETIC;
    identity.a = 1.0f; identity.b = 0.0f;
    identity.c = 0.0f; identity.d = 1.0f;
    identity.e = 0.0f; identity.f = 0.0f;
    ctx->forward = identity;
    ctx->inverse = identity;
    ctx->global_alpha = 1.0f;
    ctx->shadow_blur = 0.0f;
    ctx->line_width = 1.0f;
    ctx->miter_limit = 10.0f;
    ci_float_array_init(&ctx->shadow);
    ci_float_array_init(&ctx->line_dash);
    ci_paint_brush_init(&ctx->fill_brush);
    ci_paint_brush_init(&ctx->stroke_brush);
    ci_paint_brush_init(&ctx->image_brush);
    ci_bezier_path_init(&ctx->path);
    ci_line_path_init(&ctx->lines);
    ci_line_path_init(&ctx->scratch);
    ci_run_array_init(&ctx->runs);
    ci_run_array_init(&ctx->mask);
    ci_font_face_init(&ctx->face);
    ctx->bitmap = (ci_rgba_t *)calloc(
        (size_t)(width * height), sizeof(ci_rgba_t));
    if (!ctx->bitmap) {
        ci_canvas_free_internals(ctx);
        free(ctx);
        return NULL;
    }
    ctx->saves = NULL;
    ci_canvas_set_color(ctx, CI_FILL_STYLE,
        0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE,
        0.0f, 0.0f, 0.0f, 1.0f);
    for (y = 0; y < ctx->size_y; ++y) {
        ci_run_array_push(&ctx->mask,
            ci_run_make(0, y, 1.0f));
        ci_run_array_push(&ctx->mask,
            ci_run_make((unsigned short)ctx->size_x, y, -1.0f));
    }
    return ctx;
}

void ci_canvas_destroy(ci_canvas_t *ctx)
{
    ci_canvas_t *head;
    if (!ctx) return;
    free(ctx->bitmap);
    while ((head = ctx->saves) != NULL) {
        ctx->saves = head->saves;
        head->saves = NULL;
        free(head->bitmap);
        ci_canvas_free_internals(head);
        free(head);
    }
    ci_canvas_free_internals(ctx);
    free(ctx);
}

/* ======== TRANSFORMS ======== */

void ci_canvas_set_transform(ci_canvas_t *ctx,
    float a, float b, float c, float d, float e, float f)
{
    float det = a * d - b * c;
    float sc = det != 0.0f ? 1.0f / det : 0.0f;
    ci_affine_matrix_t fwd, inv;
    fwd.a = a; fwd.b = b; fwd.c = c;
    fwd.d = d; fwd.e = e; fwd.f = f;
    inv.a = sc * d;  inv.b = sc * -b;
    inv.c = sc * -c; inv.d = sc * a;
    inv.e = sc * (c * f - d * e);
    inv.f = sc * (b * e - a * f);
    ctx->forward = fwd;
    ctx->inverse = inv;
}

void ci_canvas_transform(ci_canvas_t *ctx,
    float a, float b, float c, float d, float e, float f)
{
    ci_canvas_set_transform(ctx,
        ctx->forward.a * a + ctx->forward.c * b,
        ctx->forward.b * a + ctx->forward.d * b,
        ctx->forward.a * c + ctx->forward.c * d,
        ctx->forward.b * c + ctx->forward.d * d,
        ctx->forward.a * e + ctx->forward.c * f + ctx->forward.e,
        ctx->forward.b * e + ctx->forward.d * f + ctx->forward.f);
}

void ci_canvas_scale(ci_canvas_t *ctx, float x, float y)
{
    ci_canvas_transform(ctx, x, 0.0f, 0.0f, y, 0.0f, 0.0f);
}

void ci_canvas_rotate(ci_canvas_t *ctx, float angle)
{
    float c = ci_cosf(angle);
    float s = ci_sinf(angle);
    ci_canvas_transform(ctx, c, s, -s, c, 0.0f, 0.0f);
}

void ci_canvas_translate(ci_canvas_t *ctx, float x, float y)
{
    ci_canvas_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, x, y);
}

/* ======== COMPOSITING ======== */

void ci_canvas_set_global_alpha(ci_canvas_t *ctx, float alpha)
{
    if (0.0f <= alpha && alpha <= 1.0f)
        ctx->global_alpha = alpha;
}

/* ======== SHADOWS ======== */

void ci_canvas_set_shadow_color(ci_canvas_t *ctx,
    float red, float green, float blue, float alpha)
{
    ctx->shadow_color = ci_premultiplied(ci_linearized(ci_clamped(
        ci_rgba_make(red, green, blue, alpha))));
}

void ci_canvas_set_shadow_blur(ci_canvas_t *ctx, float level)
{
    if (0.0f <= level)
        ctx->shadow_blur = level;
}

/* ======== LINE STYLES ======== */

void ci_canvas_set_line_width(ci_canvas_t *ctx, float width)
{
    if (0.0f < width)
        ctx->line_width = width;
}

void ci_canvas_set_miter_limit(ci_canvas_t *ctx, float limit)
{
    if (0.0f < limit)
        ctx->miter_limit = limit;
}

void ci_canvas_set_line_dash(ci_canvas_t *ctx,
    float const *segments, int count)
{
    int index;
    for (index = 0; index < count; ++index)
        if (segments && segments[index] < 0.0f)
            return;
    ci_float_array_clear(&ctx->line_dash);
    if (!segments) return;
    for (index = 0; index < count; ++index)
        ci_float_array_push(&ctx->line_dash, segments[index]);
    if (count & 1)
        for (index = 0; index < count; ++index)
            ci_float_array_push(&ctx->line_dash, segments[index]);
}

/* ======== FILL AND STROKE STYLES ======== */

void ci_canvas_set_color(ci_canvas_t *ctx, ci_brush_type type,
    float red, float green, float blue, float alpha)
{
    ci_paint_brush_t *br = type == CI_FILL_STYLE ?
        &ctx->fill_brush : &ctx->stroke_brush;
    br->type = CI_PAINT_COLOR;
    ci_rgba_array_clear(&br->colors);
    ci_rgba_array_push(&br->colors,
        ci_premultiplied(ci_linearized(ci_clamped(
            ci_rgba_make(red, green, blue, alpha)))));
}

void ci_canvas_set_linear_gradient(ci_canvas_t *ctx,
    ci_brush_type type, float start_x, float start_y,
    float end_x, float end_y)
{
    ci_paint_brush_t *br = type == CI_FILL_STYLE ?
        &ctx->fill_brush : &ctx->stroke_brush;
    br->type = CI_PAINT_LINEAR;
    ci_rgba_array_clear(&br->colors);
    ci_float_array_clear(&br->stops);
    br->start = ci_xy_make(start_x, start_y);
    br->end = ci_xy_make(end_x, end_y);
}

void ci_canvas_set_radial_gradient(ci_canvas_t *ctx,
    ci_brush_type type, float start_x, float start_y,
    float start_radius, float end_x, float end_y,
    float end_radius)
{
    ci_paint_brush_t *br;
    if (start_radius < 0.0f || end_radius < 0.0f)
        return;
    br = type == CI_FILL_STYLE ?
        &ctx->fill_brush : &ctx->stroke_brush;
    br->type = CI_PAINT_RADIAL;
    ci_rgba_array_clear(&br->colors);
    ci_float_array_clear(&br->stops);
    br->start = ci_xy_make(start_x, start_y);
    br->end = ci_xy_make(end_x, end_y);
    br->start_radius = start_radius;
    br->end_radius = end_radius;
}

void ci_canvas_add_color_stop(ci_canvas_t *ctx, ci_brush_type type,
    float offset, float red, float green, float blue, float alpha)
{
    ci_paint_brush_t *br = type == CI_FILL_STYLE ?
        &ctx->fill_brush : &ctx->stroke_brush;
    ci_rgba_t color;
    /* binary search for insertion point */
    size_t lo = 0, hi = br->stops.size;
    size_t idx;
    if ((br->type != CI_PAINT_LINEAR &&
         br->type != CI_PAINT_RADIAL) ||
        offset < 0.0f || 1.0f < offset)
        return;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        if (br->stops.data[mid] <= offset)
            lo = mid + 1;
        else
            hi = mid;
    }
    idx = lo;
    color = ci_linearized(ci_clamped(
        ci_rgba_make(red, green, blue, alpha)));
    ci_rgba_array_insert(&br->colors, idx, color);
    ci_float_array_insert(&br->stops, idx, offset);
}

void ci_canvas_set_pattern(ci_canvas_t *ctx, ci_brush_type type,
    unsigned char const *image, int width, int height,
    int stride, ci_repetition_style repetition)
{
    ci_paint_brush_t *br;
    int y, x;
    if (!image || width <= 0 || height <= 0)
        return;
    br = type == CI_FILL_STYLE ?
        &ctx->fill_brush : &ctx->stroke_brush;
    br->type = CI_PAINT_PATTERN;
    ci_rgba_array_clear(&br->colors);
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x) {
            int idx = y * stride + x * 4;
            ci_rgba_t color = ci_rgba_make(
                image[idx + 0] / 255.0f,
                image[idx + 1] / 255.0f,
                image[idx + 2] / 255.0f,
                image[idx + 3] / 255.0f);
            ci_rgba_array_push(&br->colors,
                ci_premultiplied(ci_linearized(color)));
        }
    br->width = width;
    br->height = height;
    br->repetition = repetition;
}


/* ---- Path building ---- */

void ci_canvas_begin_path(ci_canvas_t *ctx) {
    ci_xy_array_clear(&ctx->path.points);
    ci_subpath_array_clear(&ctx->path.subpaths);
}

void ci_canvas_move_to(ci_canvas_t *ctx, float x, float y) {
    ci_xy_t pt;
    ci_subpath_data_t sp;
    if (ctx->path.subpaths.size > 0 &&
        ctx->path.subpaths.data[ctx->path.subpaths.size - 1].count == 1) {
        ctx->path.points.data[ctx->path.points.size - 1] =
            ci_affine_mul(ctx->forward, ci_xy_make(x, y));
        return;
    }
    pt = ci_affine_mul(ctx->forward, ci_xy_make(x, y));
    sp = ci_subpath_make(1, 0);
    ci_xy_array_push(&ctx->path.points, pt);
    ci_subpath_array_push(&ctx->path.subpaths, sp);
}

void ci_canvas_close_path(ci_canvas_t *ctx) {
    ci_xy_t first;
    ci_affine_matrix_t saved_fwd;
    ci_affine_matrix_t identity;
    size_t back_count;
    if (ctx->path.subpaths.size == 0)
        return;
    back_count =
        ctx->path.subpaths.data[ctx->path.subpaths.size - 1].count;
    first = ctx->path.points.data[ctx->path.points.size - back_count];
    saved_fwd = ctx->forward;
    identity.a = 1.0f; identity.b = 0.0f;
    identity.c = 0.0f; identity.d = 1.0f;
    identity.e = 0.0f; identity.f = 0.0f;
    ctx->forward = identity;
    ci_canvas_line_to(ctx, first.x, first.y);
    ctx->path.subpaths.data[ctx->path.subpaths.size - 1].closed = 1;
    ci_canvas_move_to(ctx, first.x, first.y);
    ctx->forward = saved_fwd;
}

void ci_canvas_line_to(ci_canvas_t *ctx, float x, float y) {
    ci_xy_t point_1, point_2, diff;
    if (ctx->path.subpaths.size == 0) {
        ci_canvas_move_to(ctx, x, y);
        return;
    }
    point_1 = ctx->path.points.data[ctx->path.points.size - 1];
    point_2 = ci_affine_mul(ctx->forward, ci_xy_make(x, y));
    diff = ci_xy_sub(point_2, point_1);
    if (ci_dot(diff, diff) == 0.0f)
        return;
    ci_xy_array_push(&ctx->path.points, point_1);
    ci_xy_array_push(&ctx->path.points, point_2);
    ci_xy_array_push(&ctx->path.points, point_2);
    ctx->path.subpaths.data[ctx->path.subpaths.size - 1].count += 3;
}

void ci_canvas_quadratic_curve_to(ci_canvas_t *ctx,
    float cx, float cy, float x, float y)
{
    ci_xy_t point_1, control, point_2, control_1, control_2;
    if (ctx->path.subpaths.size == 0)
        ci_canvas_move_to(ctx, cx, cy);
    point_1 = ctx->path.points.data[ctx->path.points.size - 1];
    control = ci_affine_mul(ctx->forward, ci_xy_make(cx, cy));
    point_2 = ci_affine_mul(ctx->forward, ci_xy_make(x, y));
    control_1 = ci_lerp_xy(point_1, control, 2.0f / 3.0f);
    control_2 = ci_lerp_xy(point_2, control, 2.0f / 3.0f);
    ci_xy_array_push(&ctx->path.points, control_1);
    ci_xy_array_push(&ctx->path.points, control_2);
    ci_xy_array_push(&ctx->path.points, point_2);
    ctx->path.subpaths.data[ctx->path.subpaths.size - 1].count += 3;
}

void ci_canvas_bezier_curve_to(ci_canvas_t *ctx,
    float c1x, float c1y, float c2x, float c2y,
    float x, float y)
{
    ci_xy_t ctrl_1, ctrl_2, point_2;
    if (ctx->path.subpaths.size == 0)
        ci_canvas_move_to(ctx, c1x, c1y);
    ctrl_1 = ci_affine_mul(ctx->forward, ci_xy_make(c1x, c1y));
    ctrl_2 = ci_affine_mul(ctx->forward, ci_xy_make(c2x, c2y));
    point_2 = ci_affine_mul(ctx->forward, ci_xy_make(x, y));
    ci_xy_array_push(&ctx->path.points, ctrl_1);
    ci_xy_array_push(&ctx->path.points, ctrl_2);
    ci_xy_array_push(&ctx->path.points, point_2);
    ctx->path.subpaths.data[ctx->path.subpaths.size - 1].count += 3;
}

void ci_canvas_arc_to(ci_canvas_t *ctx,
    float vx, float vy, float x, float y, float radius)
{
    ci_xy_t point_1, vertex, point_2, edge_1, edge_2, offset, center;
    float sine, angle_1, angle_2;
    int reverse;
    static float const epsilon = 1.0e-4f;
    float det = ctx->forward.a * ctx->forward.d -
                ctx->forward.b * ctx->forward.c;
    if (radius < 0.0f || det == 0.0f)
        return;
    if (ctx->path.subpaths.size == 0)
        ci_canvas_move_to(ctx, vx, vy);
    point_1 = ci_affine_mul(ctx->inverse,
        ctx->path.points.data[ctx->path.points.size - 1]);
    vertex = ci_xy_make(vx, vy);
    point_2 = ci_xy_make(x, y);
    edge_1 = ci_normalized(ci_xy_sub(point_1, vertex));
    edge_2 = ci_normalized(ci_xy_sub(point_2, vertex));
    sine = (float)fabs((double)ci_dot(ci_perpendicular(edge_1), edge_2));
    if (sine < epsilon) {
        ci_canvas_line_to(ctx, vx, vy);
        return;
    }
    offset = ci_xy_scale(radius / sine, ci_xy_add(edge_1, edge_2));
    center = ci_xy_add(vertex, offset);
    angle_1 = ci_direction(ci_xy_sub(
        ci_xy_scale(ci_dot(offset, edge_1), edge_1), offset));
    angle_2 = ci_direction(ci_xy_sub(
        ci_xy_scale(ci_dot(offset, edge_2), edge_2), offset));
    reverse = (int)ci_floorf((angle_2 - angle_1) / 3.14159265f) & 1;
    ci_canvas_arc(ctx, center.x, center.y, radius,
        angle_1, angle_2, reverse);
}

void ci_canvas_arc(ci_canvas_t *ctx,
    float x, float y, float radius,
    float start_angle, float end_angle,
    int counter_clockwise)
{
    static float const tau = 6.28318531f;
    float winding = counter_clockwise ? -1.0f : 1.0f;
    float from = ci_fmodf(start_angle, tau);
    float span = ci_fmodf(end_angle, tau) - from;
    ci_xy_t centered_1;
    int steps, step;
    float segment, alpha;
    if (radius < 0.0f)
        return;
    if ((end_angle - start_angle) * winding >= tau)
        span = tau * winding;
    else if (span * winding < 0.0f)
        span += tau * winding;
    centered_1 = ci_xy_scale(radius,
        ci_xy_make(ci_cosf(from), ci_sinf(from)));
    ci_canvas_line_to(ctx, x + centered_1.x, y + centered_1.y);
    if (span == 0.0f)
        return;
    steps = (int)CI_MAX(1.0f,
        ci_roundf(16.0f / tau * span * winding));
    segment = span / (float)steps;
    alpha = 4.0f / 3.0f * ci_tanf(0.25f * segment);
    for (step = 0; step < steps; ++step) {
        float angle = from + (float)(step + 1) * segment;
        ci_xy_t centered_2 = ci_xy_scale(radius,
            ci_xy_make(ci_cosf(angle), ci_sinf(angle)));
        ci_xy_t pt1 = ci_xy_add(ci_xy_make(x, y), centered_1);
        ci_xy_t pt2 = ci_xy_add(ci_xy_make(x, y), centered_2);
        ci_xy_t c1 = ci_xy_add(pt1,
            ci_xy_scale(alpha, ci_perpendicular(centered_1)));
        ci_xy_t c2 = ci_xy_sub(pt2,
            ci_xy_scale(alpha, ci_perpendicular(centered_2)));
        ci_canvas_bezier_curve_to(ctx,
            c1.x, c1.y, c2.x, c2.y, pt2.x, pt2.y);
        centered_1 = centered_2;
    }
}

void ci_canvas_rectangle(ci_canvas_t *ctx,
    float x, float y, float w, float h)
{
    ci_canvas_move_to(ctx, x, y);
    ci_canvas_line_to(ctx, x + w, y);
    ci_canvas_line_to(ctx, x + w, y + h);
    ci_canvas_line_to(ctx, x, y + h);
    ci_canvas_close_path(ctx);
}

/* ---- Drawing paths ---- */

void ci_canvas_fill(ci_canvas_t *ctx) {
    ci_path_to_lines(ctx, 0);
    ci_render_main(ctx, &ctx->fill_brush);
}

void ci_canvas_stroke(ci_canvas_t *ctx) {
    ci_path_to_lines(ctx, 1);
    ci_stroke_lines(ctx);
    ci_render_main(ctx, &ctx->stroke_brush);
}

void ci_canvas_clip(ci_canvas_t *ctx) {
    ci_xy_t origin;
    size_t part;
    int y;
    float last, sum_1, sum_2;
    size_t index_1, index_2;
    origin = ci_xy_make(0.0f, 0.0f);
    ci_path_to_lines(ctx, 0);
    ci_lines_to_runs(ctx, origin, 0);
    part = ctx->runs.size;
    ci_run_array_append(&ctx->runs,
        ctx->mask.data, ctx->mask.size);
    ci_run_array_clear(&ctx->mask);
    y = -1;
    last = 0.0f;
    sum_1 = 0.0f;
    sum_2 = 0.0f;
    index_1 = 0;
    index_2 = part;
    while (index_1 < part && index_2 < ctx->runs.size) {
        int which = ci_run_compare(
            &ctx->runs.data[index_1],
            &ctx->runs.data[index_2]) < 0;
        ci_pixel_run_t next = which ?
            ctx->runs.data[index_1] :
            ctx->runs.data[index_2];
        float visibility;
        if (next.y != y) {
            y = next.y;
            last = 0.0f;
            sum_1 = 0.0f;
            sum_2 = 0.0f;
        }
        if (which)
            sum_1 += ctx->runs.data[index_1++].delta;
        else
            sum_2 += ctx->runs.data[index_2++].delta;
        visibility = CI_MIN((float)fabs((double)sum_1), 1.0f) *
                     CI_MIN((float)fabs((double)sum_2), 1.0f);
        if (visibility == last)
            continue;
        if (ctx->mask.size > 0 &&
            ctx->mask.data[ctx->mask.size - 1].x == next.x &&
            ctx->mask.data[ctx->mask.size - 1].y == next.y) {
            ctx->mask.data[ctx->mask.size - 1].delta +=
                visibility - last;
        } else {
            ci_pixel_run_t piece =
                ci_run_make(next.x, next.y, visibility - last);
            ci_run_array_push(&ctx->mask, piece);
        }
        last = visibility;
    }
}

int ci_canvas_is_point_in_path(ci_canvas_t *ctx,
    float x, float y)
{
    int winding;
    size_t subpath, beginning, ending, index;
    ci_path_to_lines(ctx, 0);
    winding = 0;
    subpath = 0;
    beginning = 0;
    ending = 0;
    for (index = 0; index < ctx->lines.points.size; ++index) {
        ci_xy_t from, to, diff;
        float side;
        while (index >= ending) {
            beginning = ending;
            ending += ctx->lines.subpaths.data[subpath++].count;
        }
        from = ctx->lines.points.data[index];
        to = ctx->lines.points.data[
            index + 1 < ending ? index + 1 : beginning];
        if ((from.y < y && y <= to.y) ||
            (to.y < y && y <= from.y)) {
            diff = ci_xy_sub(to, from);
            side = ci_dot(ci_perpendicular(diff),
                ci_xy_sub(ci_xy_make(x, y), from));
            if (side == 0.0f)
                return 1;
            winding += side > 0.0f ? 1 : -1;
        } else if (from.y == y && y == to.y &&
                   ((from.x <= x && x <= to.x) ||
                    (to.x <= x && x <= from.x))) {
            return 1;
        }
    }
    return winding != 0;
}

/* ---- Drawing rectangles ---- */

void ci_canvas_clear_rectangle(ci_canvas_t *ctx,
    float x, float y, float w, float h)
{
    ci_composite_operation saved_op = ctx->global_composite_operation;
    float saved_alpha = ctx->global_alpha;
    float saved_shadow_a = ctx->shadow_color.a;
    ci_paint_type saved_type = ctx->fill_brush.type;
    ctx->global_composite_operation = CI_DESTINATION_OUT;
    ctx->global_alpha = 1.0f;
    ctx->shadow_color.a = 0.0f;
    ctx->fill_brush.type = CI_PAINT_COLOR;
    ci_canvas_fill_rectangle(ctx, x, y, w, h);
    ctx->fill_brush.type = saved_type;
    ctx->shadow_color.a = saved_shadow_a;
    ctx->global_alpha = saved_alpha;
    ctx->global_composite_operation = saved_op;
}

void ci_canvas_fill_rectangle(ci_canvas_t *ctx,
    float x, float y, float w, float h)
{
    ci_subpath_data_t entry;
    if (w == 0.0f || h == 0.0f)
        return;
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward, ci_xy_make(x, y)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward, ci_xy_make(x + w, y)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward, ci_xy_make(x + w, y + h)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward, ci_xy_make(x, y + h)));
    entry = ci_subpath_make(4, 1);
    ci_subpath_array_push(&ctx->lines.subpaths, entry);
    ci_render_main(ctx, &ctx->fill_brush);
}

void ci_canvas_stroke_rectangle(ci_canvas_t *ctx,
    float x, float y, float w, float h)
{
    ci_subpath_data_t entry;
    if (w == 0.0f && h == 0.0f)
        return;
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    if (w == 0.0f || h == 0.0f) {
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward, ci_xy_make(x, y)));
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward,
                ci_xy_make(x + w, y + h)));
        entry = ci_subpath_make(2, 0);
        ci_subpath_array_push(&ctx->lines.subpaths, entry);
    } else {
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward, ci_xy_make(x, y)));
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward, ci_xy_make(x + w, y)));
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward,
                ci_xy_make(x + w, y + h)));
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward, ci_xy_make(x, y + h)));
        ci_xy_array_push(&ctx->lines.points,
            ci_affine_mul(ctx->forward, ci_xy_make(x, y)));
        entry = ci_subpath_make(5, 1);
        ci_subpath_array_push(&ctx->lines.subpaths, entry);
    }
    ci_stroke_lines(ctx);
    ci_render_main(ctx, &ctx->stroke_brush);
}

/* ---- Text ---- */

int ci_canvas_set_font(ci_canvas_t *ctx,
    unsigned char const *font, int bytes, float size)
{
    if (font && bytes) {
        int version, tables, index;
        ci_uchar_array_clear(&ctx->face.data);
        ctx->face.cmap = 0;
        ctx->face.glyf = 0;
        ctx->face.head = 0;
        ctx->face.hhea = 0;
        ctx->face.hmtx = 0;
        ctx->face.loca = 0;
        ctx->face.maxp = 0;
        ctx->face.os_2 = 0;
        if (bytes < 6)
            return 0;
        version = ((int)font[0] << 24 | (int)font[1] << 16 |
                   (int)font[2] << 8  | (int)font[3]);
        tables = (int)font[4] << 8 | (int)font[5];
        if ((version != 0x00010000 && version != 0x74727565) ||
            bytes < tables * 16 + 12)
            return 0;
        ci_uchar_array_append(&ctx->face.data,
            font, (size_t)(tables * 16 + 12));
        for (index = 0; index < tables; ++index) {
            int tag = ci_signed_32(&ctx->face.data,
                index * 16 + 12);
            int offset = ci_signed_32(&ctx->face.data,
                index * 16 + 20);
            int span = ci_signed_32(&ctx->face.data,
                index * 16 + 24);
            int place;
            if (bytes < offset + span) {
                ci_uchar_array_clear(&ctx->face.data);
                return 0;
            }
            place = (int)ctx->face.data.size;
            if (tag == 0x636d6170)
                ctx->face.cmap = place;
            else if (tag == 0x676c7966)
                ctx->face.glyf = place;
            else if (tag == 0x68656164)
                ctx->face.head = place;
            else if (tag == 0x68686561)
                ctx->face.hhea = place;
            else if (tag == 0x686d7478)
                ctx->face.hmtx = place;
            else if (tag == 0x6c6f6361)
                ctx->face.loca = place;
            else if (tag == 0x6d617870)
                ctx->face.maxp = place;
            else if (tag == 0x4f532f32)
                ctx->face.os_2 = place;
            else
                continue;
            ci_uchar_array_append(&ctx->face.data,
                font + offset, (size_t)span);
        }
        if (!ctx->face.cmap || !ctx->face.glyf ||
            !ctx->face.head || !ctx->face.hhea ||
            !ctx->face.hmtx || !ctx->face.loca ||
            !ctx->face.maxp || !ctx->face.os_2) {
            ci_uchar_array_clear(&ctx->face.data);
            return 0;
        }
    }
    if (ctx->face.data.size == 0)
        return 0;
    {
        int units = ci_unsigned_16(&ctx->face.data,
            ctx->face.head + 18);
        ctx->face.scale = size / (float)units;
    }
    return 1;
}

void ci_canvas_fill_text(ci_canvas_t *ctx,
    char const *text, float x, float y, float max_width)
{
    ci_text_to_lines(ctx, text, ci_xy_make(x, y), max_width, 0);
    ci_render_main(ctx, &ctx->fill_brush);
}

void ci_canvas_stroke_text(ci_canvas_t *ctx,
    char const *text, float x, float y, float max_width)
{
    ci_text_to_lines(ctx, text, ci_xy_make(x, y), max_width, 1);
    ci_stroke_lines(ctx);
    ci_render_main(ctx, &ctx->stroke_brush);
}

float ci_canvas_measure_text(ci_canvas_t *ctx,
    char const *text)
{
    int hmetrics, width, index;
    if (ctx->face.data.size == 0 || !text)
        return 0.0f;
    hmetrics = ci_unsigned_16(&ctx->face.data,
        ctx->face.hhea + 34);
    width = 0;
    for (index = 0; text[index]; ) {
        int glyph = ci_character_to_glyph(ctx, text, &index);
        int entry = glyph < hmetrics - 1 ? glyph : hmetrics - 1;
        width += ci_unsigned_16(&ctx->face.data,
            ctx->face.hmtx + entry * 4);
    }
    return (float)width * ctx->face.scale;
}

/* ---- Images ---- */

void ci_canvas_draw_image(ci_canvas_t *ctx,
    unsigned char const *image,
    int width, int height, int stride,
    float x, float y, float to_width, float to_height)
{
    ci_paint_brush_t temp;
    ci_subpath_data_t entry;
    ci_affine_matrix_t saved_fwd, saved_inv;
    if (!image || width <= 0 || height <= 0 ||
        to_width == 0.0f || to_height == 0.0f)
        return;
    /* swap fill_brush and image_brush */
    temp = ctx->fill_brush;
    ctx->fill_brush = ctx->image_brush;
    ctx->image_brush = temp;
    ci_canvas_set_pattern(ctx, CI_FILL_STYLE,
        image, width, height, stride, CI_REPEAT);
    /* swap back */
    temp = ctx->fill_brush;
    ctx->fill_brush = ctx->image_brush;
    ctx->image_brush = temp;
    ci_xy_array_clear(&ctx->lines.points);
    ci_subpath_array_clear(&ctx->lines.subpaths);
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward, ci_xy_make(x, y)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward,
            ci_xy_make(x + to_width, y)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward,
            ci_xy_make(x + to_width, y + to_height)));
    ci_xy_array_push(&ctx->lines.points,
        ci_affine_mul(ctx->forward,
            ci_xy_make(x, y + to_height)));
    entry = ci_subpath_make(4, 1);
    ci_subpath_array_push(&ctx->lines.subpaths, entry);
    saved_fwd = ctx->forward;
    saved_inv = ctx->inverse;
    ci_canvas_translate(ctx,
        x + CI_MIN(0.0f, to_width),
        y + CI_MIN(0.0f, to_height));
    ci_canvas_scale(ctx,
        (float)fabs((double)to_width) / (float)width,
        (float)fabs((double)to_height) / (float)height);
    ci_render_main(ctx, &ctx->image_brush);
    ctx->forward = saved_fwd;
    ctx->inverse = saved_inv;
}

void ci_canvas_get_image_data(ci_canvas_t *ctx,
    unsigned char *image,
    int width, int height, int stride,
    int x, int y)
{
    static float const bayer[4][4] = {
        { 0.03125f, 0.53125f, 0.15625f, 0.65625f },
        { 0.78125f, 0.28125f, 0.90625f, 0.40625f },
        { 0.21875f, 0.71875f, 0.09375f, 0.59375f },
        { 0.96875f, 0.46875f, 0.84375f, 0.34375f }
    };
    int img_y, img_x;
    if (!image)
        return;
    for (img_y = 0; img_y < height; ++img_y)
        for (img_x = 0; img_x < width; ++img_x) {
            int idx = img_y * stride + img_x * 4;
            int cx = x + img_x;
            int cy = y + img_y;
            ci_rgba_t color = ci_rgba_make(0.0f, 0.0f, 0.0f, 0.0f);
            ci_rgba_t threshold_color, delin;
            float th;
            if (0 <= cx && cx < ctx->size_x &&
                0 <= cy && cy < ctx->size_y)
                color = ctx->bitmap[cy * ctx->size_x + cx];
            th = bayer[cy & 3][cx & 3];
            threshold_color =
                ci_rgba_make(th, th, th, th);
            delin = ci_delinearized(
                ci_clamped(ci_unpremultiplied(color)));
            color = ci_rgba_add(threshold_color,
                ci_rgba_scale(255.0f, delin));
            image[idx + 0] = (unsigned char)color.r;
            image[idx + 1] = (unsigned char)color.g;
            image[idx + 2] = (unsigned char)color.b;
            image[idx + 3] = (unsigned char)color.a;
        }
}

void ci_canvas_put_image_data(ci_canvas_t *ctx,
    unsigned char const *image,
    int width, int height, int stride,
    int x, int y)
{
    int img_y, img_x;
    if (!image)
        return;
    for (img_y = 0; img_y < height; ++img_y)
        for (img_x = 0; img_x < width; ++img_x) {
            int idx = img_y * stride + img_x * 4;
            int cx = x + img_x;
            int cy = y + img_y;
            ci_rgba_t color;
            if (cx < 0 || ctx->size_x <= cx ||
                cy < 0 || ctx->size_y <= cy)
                continue;
            color = ci_rgba_make(
                image[idx + 0] / 255.0f,
                image[idx + 1] / 255.0f,
                image[idx + 2] / 255.0f,
                image[idx + 3] / 255.0f);
            ctx->bitmap[cy * ctx->size_x + cx] =
                ci_premultiplied(ci_linearized(color));
        }
}

/* ---- State stack ---- */

void ci_canvas_save(ci_canvas_t *ctx) {
    ci_canvas_t *state =
        (ci_canvas_t *)calloc(1, sizeof(ci_canvas_t));
    if (!state)
        return;
    state->global_composite_operation =
        ctx->global_composite_operation;
    state->shadow_offset_x = ctx->shadow_offset_x;
    state->shadow_offset_y = ctx->shadow_offset_y;
    state->line_cap = ctx->line_cap;
    state->line_join = ctx->line_join;
    state->line_dash_offset = ctx->line_dash_offset;
    state->text_align = ctx->text_align;
    state->text_baseline = ctx->text_baseline;
    state->forward = ctx->forward;
    state->inverse = ctx->inverse;
    state->global_alpha = ctx->global_alpha;
    state->shadow_color = ctx->shadow_color;
    state->shadow_blur = ctx->shadow_blur;
    state->line_width = ctx->line_width;
    state->miter_limit = ctx->miter_limit;
    ci_float_array_init(&state->line_dash);
    ci_float_array_copy(&state->line_dash, &ctx->line_dash);
    ci_paint_brush_init(&state->fill_brush);
    ci_paint_brush_copy(&state->fill_brush, &ctx->fill_brush);
    ci_paint_brush_init(&state->stroke_brush);
    ci_paint_brush_copy(&state->stroke_brush, &ctx->stroke_brush);
    ci_run_array_init(&state->mask);
    ci_run_array_copy(&state->mask, &ctx->mask);
    ci_font_face_init(&state->face);
    ci_font_face_copy(&state->face, &ctx->face);
    state->saves = ctx->saves;
    /* null out arrays that save state does not own */
    state->bitmap = NULL;
    state->size_x = 0;
    state->size_y = 0;
    ci_float_array_init(&state->shadow);
    ci_xy_array_init(&state->path.points);
    ci_subpath_array_init(&state->path.subpaths);
    ci_line_path_init(&state->lines);
    ci_line_path_init(&state->scratch);
    ci_run_array_init(&state->runs);
    ci_paint_brush_init(&state->image_brush);
    ctx->saves = state;
}

void ci_canvas_restore(ci_canvas_t *ctx) {
    ci_canvas_t *state;
    if (!ctx->saves)
        return;
    state = ctx->saves;
    ctx->global_composite_operation =
        state->global_composite_operation;
    ctx->shadow_offset_x = state->shadow_offset_x;
    ctx->shadow_offset_y = state->shadow_offset_y;
    ctx->line_cap = state->line_cap;
    ctx->line_join = state->line_join;
    ctx->line_dash_offset = state->line_dash_offset;
    ctx->text_align = state->text_align;
    ctx->text_baseline = state->text_baseline;
    ctx->forward = state->forward;
    ctx->inverse = state->inverse;
    ctx->global_alpha = state->global_alpha;
    ctx->shadow_color = state->shadow_color;
    ctx->shadow_blur = state->shadow_blur;
    ctx->line_width = state->line_width;
    ctx->miter_limit = state->miter_limit;
    /* swap dynamic arrays — transfer ownership */
    ci_float_array_free(&ctx->line_dash);
    ctx->line_dash = state->line_dash;
    ci_float_array_init(&state->line_dash);
    ci_paint_brush_free(&ctx->fill_brush);
    ctx->fill_brush = state->fill_brush;
    ci_paint_brush_init(&state->fill_brush);
    ci_paint_brush_free(&ctx->stroke_brush);
    ctx->stroke_brush = state->stroke_brush;
    ci_paint_brush_init(&state->stroke_brush);
    ci_run_array_free(&ctx->mask);
    ctx->mask = state->mask;
    ci_run_array_init(&state->mask);
    ci_font_face_free(&ctx->face);
    ctx->face = state->face;
    ci_font_face_init(&state->face);
    ctx->saves = state->saves;
    state->saves = NULL;
    /* free the save state shell */
    ci_xy_array_free(&state->path.points);
    ci_subpath_array_free(&state->path.subpaths);
    ci_xy_array_free(&state->lines.points);
    ci_subpath_array_free(&state->lines.subpaths);
    ci_run_array_free(&state->runs);
    ci_paint_brush_free(&state->image_brush);
    free(state);
}

#endif /* CANVAS_ITY_IMPLEMENTATION */
