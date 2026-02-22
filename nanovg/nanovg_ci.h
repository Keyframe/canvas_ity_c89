/*
 * nanovg_ci.h -- NanoVG backend for canvas_ity
 *
 * Copyright (c) 2024 Dominik Susmel
 * ISC license (same as canvas_ity)
 *
 * This single-header file bridges NanoVG's rendering callbacks to canvas_ity's
 * 2D vector path API.  Usage:
 *
 *   #define NANOVG_CI_IMPLEMENTATION
 *   #include "nanovg_ci.h"
 *
 * Public API:
 *   NVGcontext *nvgCreateCI(int width, int height, int flags);
 *   void        nvgDeleteCI(NVGcontext *ctx);
 *   void        nvgciReadPixels(NVGcontext *ctx, unsigned char *dst,
 *                               int width, int height, int stride);
 *
 * Flags:
 *   NVG_ANTIALIAS  -- hint (canvas_ity always antialiases)
 *   NVG_DEBUG      -- reserved
 */

#ifndef NANOVG_CI_H
#define NANOVG_CI_H

#include "nanovg.h"

#ifdef __cplusplus
extern "C" {
#endif

enum NVGcreateFlags {
    NVG_ANTIALIAS       = 1 << 0,
    NVG_STENCIL_STROKES = 1 << 1,  /* ignored -- canvas_ity uses coverage */
    NVG_DEBUG           = 1 << 2
};

NVGcontext *nvgCreateCI(int width, int height, int flags);
void        nvgDeleteCI(NVGcontext *ctx);
void        nvgciReadPixels(NVGcontext *ctx, unsigned char *dst,
                            int width, int height, int stride);

#ifdef __cplusplus
}
#endif

#endif /* NANOVG_CI_H */

/* ======== IMPLEMENTATION ======== */

#ifdef NANOVG_CI_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* canvas_ity must be included with CANVAS_ITY_IMPLEMENTATION before this file,
   or at least the header portion must be visible. */

/* ---- texture store ---- */

typedef struct NVGCItexture {
    int id;
    unsigned char *data;
    int width, height;
    int type;  /* NVG_TEXTURE_ALPHA or NVG_TEXTURE_RGBA */
    int flags;
} NVGCItexture;

typedef struct NVGCIcontext {
    ci_canvas_t *canvas;
    int width, height;
    float device_pixel_ratio;
    NVGCItexture *textures;
    int ntextures;
    int texture_cap;
    int next_texture_id;
} NVGCIcontext;

/* ---- helpers ---- */

static NVGCItexture *nvgci__findTexture(NVGCIcontext *ci, int id)
{
    int i;
    for (i = 0; i < ci->ntextures; i++) {
        if (ci->textures[i].id == id)
            return &ci->textures[i];
    }
    return NULL;
}

static NVGCItexture *nvgci__allocTexture(NVGCIcontext *ci)
{
    NVGCItexture *tex = NULL;
    int i;

    /* find a free slot */
    for (i = 0; i < ci->ntextures; i++) {
        if (ci->textures[i].id == 0) {
            tex = &ci->textures[i];
            break;
        }
    }
    if (tex == NULL) {
        /* grow array */
        if (ci->ntextures + 1 > ci->texture_cap) {
            int new_cap = ci->texture_cap == 0 ? 4 : ci->texture_cap * 2;
            NVGCItexture *new_arr = (NVGCItexture *)realloc(
                ci->textures, (size_t)new_cap * sizeof(NVGCItexture));
            if (!new_arr) return NULL;
            memset(new_arr + ci->texture_cap, 0,
                   (size_t)(new_cap - ci->texture_cap) * sizeof(NVGCItexture));
            ci->textures = new_arr;
            ci->texture_cap = new_cap;
        }
        tex = &ci->textures[ci->ntextures++];
    }
    memset(tex, 0, sizeof(*tex));
    tex->id = ++ci->next_texture_id;
    return tex;
}

/* SDF rounded rect distance (NanoVG paint formula) */
static float nvgci__sdroundrect(float px, float py,
                                float ex, float ey, float r)
{
    float dx = (float)fabs((double)px) - ex + r;
    float dy = (float)fabs((double)py) - ey + r;
    float ax = dx > 0.0f ? dx : 0.0f;
    float ay = dy > 0.0f ? dy : 0.0f;
    float outside = (float)sqrt((double)(ax * ax + ay * ay)) - r;
    float bx = dx > dy ? dx : dy;
    float inside = bx < 0.0f ? bx : 0.0f;
    return outside + inside;
}

static float nvgci__clampf(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* ---- paint application ---- */

static void nvgci__applyPaint(NVGCIcontext *ci, NVGpaint *paint,
                              NVGcompositeOperationState compositeOp,
                              const float *bounds)
{
    float ir, ig, ib, ia;
    float or_, og, ob, oa;

    (void)compositeOp;

    ir = paint->innerColor.u.col.r;
    ig = paint->innerColor.u.col.g;
    ib = paint->innerColor.u.col.b;
    ia = paint->innerColor.u.col.a;
    or_ = paint->outerColor.u.col.r;
    og = paint->outerColor.u.col.g;
    ob = paint->outerColor.u.col.b;
    oa = paint->outerColor.u.col.a;

    /* Determine paint type */
    if (paint->image != 0) {
        /* Image paint */
        NVGCItexture *tex = nvgci__findTexture(ci, paint->image);
        if (tex && tex->data) {
            ci_canvas_set_pattern(ci->canvas, CI_FILL_STYLE,
                tex->data, tex->width, tex->height,
                tex->width * 4, CI_REPEAT);
        } else {
            ci_canvas_set_color(ci->canvas, CI_FILL_STYLE,
                ir, ig, ib, ia);
        }
    } else if ((float)fabs((double)(ir - or_)) < 1e-5f &&
               (float)fabs((double)(ig - og)) < 1e-5f &&
               (float)fabs((double)(ib - ob)) < 1e-5f &&
               (float)fabs((double)(ia - oa)) < 1e-5f) {
        /* Solid color fast path */
        ci_canvas_set_color(ci->canvas, CI_FILL_STYLE,
            ir, ig, ib, ia);
    } else {
        /* Gradient: pre-render SDF paint to pattern buffer.
           ci_paint_pixel uses ctx->inverse at render time, so the pattern
           must be at canvas origin with identity transform during fill. */
        int bw = ci->width;
        int bh = ci->height;
        unsigned char *buf;
        float invxf[6];
        int has_inv;
        int px, py;

        /* Use path bounds to limit the buffer when available */
        if (bounds) {
            int bx0 = (int)bounds[0];
            int by0_ = (int)bounds[1];
            int bx1 = (int)(bounds[2] + 1.0f);
            int by1 = (int)(bounds[3] + 1.0f);
            if (bx0 < 0) bx0 = 0;
            if (by0_ < 0) by0_ = 0;
            if (bx1 > ci->width)  bx1 = ci->width;
            if (by1 > ci->height) by1 = ci->height;
            (void)bx0; (void)by0_; (void)bx1; (void)by1;
        }

        buf = (unsigned char *)malloc((size_t)(bw * bh * 4));
        if (buf) {
            has_inv = nvgTransformInverse(invxf, paint->xform);
            if (!has_inv) {
                nvgTransformIdentity(invxf);
            }

            for (py = 0; py < bh; py++) {
                for (px = 0; px < bw; px++) {
                    float sx = (float)px + 0.5f;
                    float sy = (float)py + 0.5f;
                    float lx, ly, d, t;
                    float cr, cg, cb, ca;
                    int idx = (py * bw + px) * 4;

                    lx = sx * invxf[0] + sy * invxf[2] + invxf[4];
                    ly = sx * invxf[1] + sy * invxf[3] + invxf[5];

                    d = nvgci__sdroundrect(lx, ly,
                            paint->extent[0], paint->extent[1],
                            paint->radius);
                    t = nvgci__clampf(
                            (d + paint->feather * 0.5f) / paint->feather,
                            0.0f, 1.0f);

                    cr = ir + (or_ - ir) * t;
                    cg = ig + (og - ig) * t;
                    cb = ib + (ob - ib) * t;
                    ca = ia + (oa - ia) * t;

                    buf[idx + 0] = (unsigned char)(nvgci__clampf(cr, 0.0f, 1.0f) * 255.0f + 0.5f);
                    buf[idx + 1] = (unsigned char)(nvgci__clampf(cg, 0.0f, 1.0f) * 255.0f + 0.5f);
                    buf[idx + 2] = (unsigned char)(nvgci__clampf(cb, 0.0f, 1.0f) * 255.0f + 0.5f);
                    buf[idx + 3] = (unsigned char)(nvgci__clampf(ca, 0.0f, 1.0f) * 255.0f + 0.5f);
                }
            }
            /* Pattern at canvas origin — identity transform will be set
               by the caller (renderFill/renderStroke) before fill. */
            ci_canvas_set_pattern(ci->canvas, CI_FILL_STYLE,
                buf, bw, bh, bw * 4, CI_NO_REPEAT);
            free(buf);
        }
    }
}

/* ---- NVGparams callbacks ---- */

static int nvgci__renderCreate(void *uptr)
{
    (void)uptr;
    return 1; /* canvas already created in nvgCreateCI */
}

static int nvgci__renderCreateTexture(void *uptr, int type,
    int w, int h, int imageFlags, const unsigned char *data)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    NVGCItexture *tex = nvgci__allocTexture(ci);
    size_t nbytes;
    if (!tex) return 0;

    tex->width = w;
    tex->height = h;
    tex->type = type;
    tex->flags = imageFlags;

    /* Always store as RGBA internally */
    nbytes = (size_t)(w * h * 4);
    tex->data = (unsigned char *)malloc(nbytes);
    if (!tex->data) { tex->id = 0; return 0; }

    if (data) {
        if (type == NVG_TEXTURE_RGBA) {
            memcpy(tex->data, data, nbytes);
        } else {
            /* Alpha-only: expand to RGBA (white + alpha) */
            int i;
            for (i = 0; i < w * h; i++) {
                tex->data[i * 4 + 0] = 255;
                tex->data[i * 4 + 1] = 255;
                tex->data[i * 4 + 2] = 255;
                tex->data[i * 4 + 3] = data[i];
            }
        }
    } else {
        memset(tex->data, 0, nbytes);
    }
    return tex->id;
}

static int nvgci__renderDeleteTexture(void *uptr, int image)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    NVGCItexture *tex = nvgci__findTexture(ci, image);
    if (!tex) return 0;
    free(tex->data);
    tex->data = NULL;
    tex->id = 0;
    return 1;
}

static int nvgci__renderUpdateTexture(void *uptr, int image,
    int x, int y, int w, int h, const unsigned char *data)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    NVGCItexture *tex = nvgci__findTexture(ci, image);
    int row;
    if (!tex) return 0;

    if (tex->type == NVG_TEXTURE_RGBA) {
        for (row = 0; row < h; row++) {
            int dy = y + row;
            if (dy < 0 || dy >= tex->height) continue;
            memcpy(tex->data + (dy * tex->width + x) * 4,
                   data + (dy * tex->width + x) * 4,
                   (size_t)(w * 4));
        }
    } else {
        /* Alpha texture */
        int px;
        for (row = 0; row < h; row++) {
            int dy = y + row;
            if (dy < 0 || dy >= tex->height) continue;
            for (px = 0; px < w; px++) {
                int dx = x + px;
                int dst_idx = (dy * tex->width + dx) * 4;
                int src_idx = dy * tex->width + dx;
                tex->data[dst_idx + 0] = 255;
                tex->data[dst_idx + 1] = 255;
                tex->data[dst_idx + 2] = 255;
                tex->data[dst_idx + 3] = data[src_idx];
            }
        }
    }
    return 1;
}

static int nvgci__renderGetTextureSize(void *uptr, int image, int *w, int *h)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    NVGCItexture *tex = nvgci__findTexture(ci, image);
    if (!tex) return 0;
    *w = tex->width;
    *h = tex->height;
    return 1;
}

static void nvgci__renderViewport(void *uptr, float width, float height,
                                  float devicePixelRatio)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    (void)width;
    (void)height;
    ci->device_pixel_ratio = devicePixelRatio;
}

static void nvgci__renderCancel(void *uptr)
{
    (void)uptr;
}

static void nvgci__renderFlush(void *uptr)
{
    (void)uptr;
}

static void nvgci__renderFill(void *uptr, NVGpaint *paint,
    NVGcompositeOperationState compositeOp, NVGscissor *scissor,
    float fringe, const float *bounds,
    const NVGpath *paths, int npaths)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    int i, j;
    int did_scissor_save = 0;

    (void)fringe;

    ci_canvas_save(ci->canvas);
    /* Identity transform — vertices already in screen space */
    ci_canvas_set_transform(ci->canvas, 1, 0, 0, 1, 0, 0);

    /* Apply scissor clip */
    if (scissor->extent[0] >= 0.0f && scissor->extent[1] >= 0.0f) {
        float cx = scissor->xform[4];
        float cy = scissor->xform[5];
        float hw = scissor->extent[0];
        float hh = scissor->extent[1];
        ci_canvas_begin_path(ci->canvas);
        ci_canvas_rectangle(ci->canvas, cx - hw, cy - hh, hw * 2, hh * 2);
        ci_canvas_clip(ci->canvas);
        did_scissor_save = 1;
    }

    /* Apply paint */
    nvgci__applyPaint(ci, paint, compositeOp, bounds);

    /* Build compound path from all NVGpath fill vertices */
    ci_canvas_begin_path(ci->canvas);
    for (i = 0; i < npaths; i++) {
        const NVGpath *path = &paths[i];
        if (path->nfill > 0) {
            ci_canvas_move_to(ci->canvas,
                path->fill[0].x, path->fill[0].y);
            for (j = 1; j < path->nfill; j++) {
                ci_canvas_line_to(ci->canvas,
                    path->fill[j].x, path->fill[j].y);
            }
            ci_canvas_close_path(ci->canvas);
        }
    }

    /* Use evenodd fill rule (NanoVG default) */
    ci_canvas_set_fill_rule(ci->canvas, CI_FILL_EVENODD);
    ci_canvas_fill(ci->canvas);

    ci_canvas_restore(ci->canvas);
    (void)did_scissor_save;
}

static void nvgci__renderStroke(void *uptr, NVGpaint *paint,
    NVGcompositeOperationState compositeOp, NVGscissor *scissor,
    float fringe, float strokeWidth,
    const NVGpath *paths, int npaths)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    int i, j;

    (void)fringe;
    (void)strokeWidth;

    ci_canvas_save(ci->canvas);
    ci_canvas_set_transform(ci->canvas, 1, 0, 0, 1, 0, 0);

    /* Apply scissor clip */
    if (scissor->extent[0] >= 0.0f && scissor->extent[1] >= 0.0f) {
        float cx = scissor->xform[4];
        float cy = scissor->xform[5];
        float hw = scissor->extent[0];
        float hh = scissor->extent[1];
        ci_canvas_begin_path(ci->canvas);
        ci_canvas_rectangle(ci->canvas, cx - hw, cy - hh, hw * 2, hh * 2);
        ci_canvas_clip(ci->canvas);
    }

    nvgci__applyPaint(ci, paint, compositeOp, NULL);

    /* Stroke vertices form a triangle strip — extract outline polygon.
       Even-indexed vertices are one edge, odd-indexed are the other.
       Walk evens forward, then odds backward, to form a closed polygon. */
    ci_canvas_begin_path(ci->canvas);
    for (i = 0; i < npaths; i++) {
        const NVGpath *path = &paths[i];
        int nstroke = path->nstroke;
        if (nstroke < 2) continue;

        /* Even vertices (left edge) forward */
        ci_canvas_move_to(ci->canvas,
            path->stroke[0].x, path->stroke[0].y);
        for (j = 2; j < nstroke; j += 2) {
            ci_canvas_line_to(ci->canvas,
                path->stroke[j].x, path->stroke[j].y);
        }
        /* Odd vertices (right edge) backward */
        for (j = ((nstroke - 1) / 2) * 2 + 1; j >= 1; j -= 2) {
            ci_canvas_line_to(ci->canvas,
                path->stroke[j].x, path->stroke[j].y);
        }
        ci_canvas_close_path(ci->canvas);
    }

    ci_canvas_set_fill_rule(ci->canvas, CI_FILL_NONZERO);
    ci_canvas_fill(ci->canvas);

    ci_canvas_restore(ci->canvas);
}

static void nvgci__renderTriangles(void *uptr, NVGpaint *paint,
    NVGcompositeOperationState compositeOp, NVGscissor *scissor,
    const NVGvertex *verts, int nverts, float fringe)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    NVGCItexture *tex = NULL;
    unsigned char *pixels;
    int w, h, stride;
    int tri;

    (void)compositeOp;
    (void)scissor;
    (void)fringe;

    if (paint->image != 0)
        tex = nvgci__findTexture(ci, paint->image);

    w = ci->width;
    h = ci->height;
    stride = w * 4;

    /* Read current canvas pixels */
    pixels = (unsigned char *)malloc((size_t)(stride * h));
    if (!pixels) return;
    ci_canvas_get_image_data(ci->canvas, pixels, w, h, stride, 0, 0);

    /* Rasterize each triangle using barycentric coordinates */
    for (tri = 0; tri + 2 < nverts; tri += 3) {
        const NVGvertex *v0 = &verts[tri + 0];
        const NVGvertex *v1 = &verts[tri + 1];
        const NVGvertex *v2 = &verts[tri + 2];
        float x0 = v0->x, y0_ = v0->y, u0 = v0->u, vv0 = v0->v;
        float x1 = v1->x, y1_ = v1->y, u1 = v1->u, vv1 = v1->v;
        float x2 = v2->x, y2_ = v2->y, u2 = v2->u, vv2 = v2->v;
        float minx, miny, maxx, maxy;
        float denom, inv_denom;
        int ix0, iy0, ix1, iy1, py, px;

        /* Bounding box */
        minx = x0; if (x1 < minx) minx = x1; if (x2 < minx) minx = x2;
        miny = y0_; if (y1_ < miny) miny = y1_; if (y2_ < miny) miny = y2_;
        maxx = x0; if (x1 > maxx) maxx = x1; if (x2 > maxx) maxx = x2;
        maxy = y0_; if (y1_ > maxy) maxy = y1_; if (y2_ > maxy) maxy = y2_;

        ix0 = (int)minx; if (ix0 < 0) ix0 = 0;
        iy0 = (int)miny; if (iy0 < 0) iy0 = 0;
        ix1 = (int)(maxx + 1.0f); if (ix1 > w) ix1 = w;
        iy1 = (int)(maxy + 1.0f); if (iy1 > h) iy1 = h;

        denom = (y1_ - y2_) * (x0 - x2) + (x2 - x1) * (y0_ - y2_);
        if ((float)fabs((double)denom) < 1e-10f) continue;
        inv_denom = 1.0f / denom;

        for (py = iy0; py < iy1; py++) {
            for (px = ix0; px < ix1; px++) {
                float sx = (float)px + 0.5f;
                float sy = (float)py + 0.5f;
                float w0, w1, w2;
                float tu, tv;
                float alpha;
                float cr, cg, cb, ca;
                int dst_idx;
                unsigned char dr, dg, db, da;
                float sa, inv_sa;

                w0 = ((y1_ - y2_) * (sx - x2) + (x2 - x1) * (sy - y2_)) * inv_denom;
                w1 = ((y2_ - y0_) * (sx - x2) + (x0 - x2) * (sy - y2_)) * inv_denom;
                w2 = 1.0f - w0 - w1;

                if (w0 < 0.0f || w1 < 0.0f || w2 < 0.0f) continue;

                tu = u0 * w0 + u1 * w1 + u2 * w2;
                tv = vv0 * w0 + vv1 * w1 + vv2 * w2;

                /* Sample texture */
                alpha = 1.0f;
                if (tex && tex->data) {
                    int tix = (int)(tu * (float)tex->width);
                    int tiy = (int)(tv * (float)tex->height);
                    if (tix < 0) tix = 0;
                    if (tix >= tex->width) tix = tex->width - 1;
                    if (tiy < 0) tiy = 0;
                    if (tiy >= tex->height) tiy = tex->height - 1;
                    if (tex->type == NVG_TEXTURE_ALPHA) {
                        alpha = (float)tex->data[tiy * tex->width + tix] / 255.0f;
                    } else {
                        int ti = (tiy * tex->width + tix) * 4;
                        alpha = (float)tex->data[ti + 3] / 255.0f;
                    }
                }

                /* Paint color * alpha */
                cr = paint->innerColor.u.col.r;
                cg = paint->innerColor.u.col.g;
                cb = paint->innerColor.u.col.b;
                ca = paint->innerColor.u.col.a * alpha;

                if (ca < 1.0f / 255.0f) continue;

                /* Alpha-blend onto the pixel buffer */
                dst_idx = (py * w + px) * 4;
                dr = pixels[dst_idx + 0];
                dg = pixels[dst_idx + 1];
                db = pixels[dst_idx + 2];
                da = pixels[dst_idx + 3];

                sa = ca;
                inv_sa = 1.0f - sa;
                pixels[dst_idx + 0] = (unsigned char)(cr * sa * 255.0f + (float)dr * inv_sa + 0.5f);
                pixels[dst_idx + 1] = (unsigned char)(cg * sa * 255.0f + (float)dg * inv_sa + 0.5f);
                pixels[dst_idx + 2] = (unsigned char)(cb * sa * 255.0f + (float)db * inv_sa + 0.5f);
                pixels[dst_idx + 3] = (unsigned char)(sa * 255.0f + (float)da * inv_sa + 0.5f);
            }
        }
    }

    /* Write pixels back */
    ci_canvas_put_image_data(ci->canvas, pixels, w, h, stride, 0, 0);
    free(pixels);
}

static void nvgci__renderDelete(void *uptr)
{
    NVGCIcontext *ci = (NVGCIcontext *)uptr;
    int i;
    if (!ci) return;

    for (i = 0; i < ci->ntextures; i++) {
        free(ci->textures[i].data);
    }
    free(ci->textures);

    if (ci->canvas) {
        ci_canvas_destroy(ci->canvas);
    }
    free(ci);
}

/* ---- public API ---- */

NVGcontext *nvgCreateCI(int width, int height, int flags)
{
    NVGparams params;
    NVGCIcontext *ci;
    NVGcontext *ctx;

    ci = (NVGCIcontext *)calloc(1, sizeof(NVGCIcontext));
    if (!ci) return NULL;

    ci->canvas = ci_canvas_create(width, height);
    if (!ci->canvas) {
        free(ci);
        return NULL;
    }
    ci->width = width;
    ci->height = height;
    ci->device_pixel_ratio = 1.0f;

    memset(&params, 0, sizeof(params));
    params.renderCreate         = nvgci__renderCreate;
    params.renderCreateTexture  = nvgci__renderCreateTexture;
    params.renderDeleteTexture  = nvgci__renderDeleteTexture;
    params.renderUpdateTexture  = nvgci__renderUpdateTexture;
    params.renderGetTextureSize = nvgci__renderGetTextureSize;
    params.renderViewport       = nvgci__renderViewport;
    params.renderCancel         = nvgci__renderCancel;
    params.renderFlush          = nvgci__renderFlush;
    params.renderFill           = nvgci__renderFill;
    params.renderStroke         = nvgci__renderStroke;
    params.renderTriangles      = nvgci__renderTriangles;
    params.renderDelete         = nvgci__renderDelete;
    params.userPtr              = ci;
    params.edgeAntiAlias        = (flags & NVG_ANTIALIAS) ? 1 : 0;

    ctx = nvgCreateInternal(&params);
    if (!ctx) {
        nvgci__renderDelete(ci);
        return NULL;
    }
    return ctx;
}

void nvgDeleteCI(NVGcontext *ctx)
{
    nvgDeleteInternal(ctx);
}

void nvgciReadPixels(NVGcontext *ctx, unsigned char *dst,
                     int width, int height, int stride)
{
    NVGparams *params = nvgInternalParams(ctx);
    NVGCIcontext *ci = (NVGCIcontext *)params->userPtr;
    ci_canvas_get_image_data(ci->canvas, dst, width, height, stride, 0, 0);
}

#endif /* NANOVG_CI_IMPLEMENTATION */
