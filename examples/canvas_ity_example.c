/*
 * canvas_ity_example.c -- standalone canvas_ity demo
 *
 * Draws a star with gradient fills, dashed strokes, and drop shadows.
 * Writes output to canvas_ity_example.tga.
 *
 * Build:
 *   gcc -std=c89 -O2 -I../src -o canvas_ity_example canvas_ity_example.c -lm
 *
 * Or via the Makefile:
 *   make canvas_ity_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

static int write_tga(const char *path,
                     const unsigned char *rgba, int w, int h)
{
    unsigned char header[18];
    unsigned char *bgra;
    int pixel;
    FILE *fp;

    memset(header, 0, sizeof(header));
    header[2] = 2;
    header[12] = (unsigned char)(w & 0xff);
    header[13] = (unsigned char)((w >> 8) & 0xff);
    header[14] = (unsigned char)(h & 0xff);
    header[15] = (unsigned char)((h >> 8) & 0xff);
    header[16] = 32;
    header[17] = 40;

    bgra = (unsigned char *)malloc((size_t)(w * h * 4));
    if (!bgra) return 0;

    for (pixel = 0; pixel < w * h; pixel++) {
        bgra[pixel * 4 + 0] = rgba[pixel * 4 + 2];
        bgra[pixel * 4 + 1] = rgba[pixel * 4 + 1];
        bgra[pixel * 4 + 2] = rgba[pixel * 4 + 0];
        bgra[pixel * 4 + 3] = rgba[pixel * 4 + 3];
    }

    fp = fopen(path, "wb");
    if (!fp) { free(bgra); return 0; }
    fwrite(header, 1, sizeof(header), fp);
    fwrite(bgra, 1, (size_t)(w * h * 4), fp);
    fclose(fp);
    free(bgra);
    return 1;
}

int main(void)
{
    static int const width = 256, height = 256;
    unsigned char *image;
    float segments[] = {
        21.0f, 9.0f, 1.0f, 9.0f,
         7.0f, 9.0f, 1.0f, 9.0f
    };
    ci_canvas_t *ctx = ci_canvas_create(width, height);

    /* Build a star path. */
    ci_canvas_move_to(ctx, 128.0f,  28.0f);
    ci_canvas_line_to(ctx, 157.0f,  87.0f);
    ci_canvas_line_to(ctx, 223.0f,  97.0f);
    ci_canvas_line_to(ctx, 175.0f, 143.0f);
    ci_canvas_line_to(ctx, 186.0f, 208.0f);
    ci_canvas_line_to(ctx, 128.0f, 178.0f);
    ci_canvas_line_to(ctx,  69.0f, 208.0f);
    ci_canvas_line_to(ctx,  80.0f, 143.0f);
    ci_canvas_line_to(ctx,  32.0f,  97.0f);
    ci_canvas_line_to(ctx,  98.0f,  87.0f);
    ci_canvas_close_path(ctx);

    /* Drop shadow. */
    ci_canvas_set_shadow_blur(ctx, 8.0f);
    ctx->shadow_offset_y = 4.0f;
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.5f);

    /* Fill the star with yellow. */
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.9f, 0.2f, 1.0f);
    ci_canvas_fill(ctx);

    /* Thick red stroke with rounded joins. */
    ctx->line_join = CI_JOIN_ROUNDED;
    ci_canvas_set_line_width(ctx, 12.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.9f, 0.0f, 0.5f, 1.0f);
    ci_canvas_stroke(ctx);

    /* Dashed thinner orange stroke. */
    ci_canvas_set_line_dash(ctx, segments, 8);
    ctx->line_dash_offset = 10.0f;
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.95f, 0.65f, 0.15f, 1.0f);
    ci_canvas_stroke(ctx);

    /* Turn off the drop shadow. */
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.0f);

    /* Add a shine layer over the star. */
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE,
        64.0f, 0.0f, 192.0f, 256.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.30f, 1.0f, 1.0f, 1.0f, 0.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.35f, 1.0f, 1.0f, 1.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.45f, 1.0f, 1.0f, 1.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.50f, 1.0f, 1.0f, 1.0f, 0.0f);
    ctx->global_composite_operation = CI_SOURCE_ATOP;
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, 256.0f, 256.0f);

    /* Write TGA output. */
    image = (unsigned char *)malloc((size_t)(height * width * 4));
    ci_canvas_get_image_data(ctx, image, width, height, width * 4, 0, 0);

    if (write_tga("canvas_ity_example.tga", image, width, height))
        printf("Wrote canvas_ity_example.tga (%dx%d)\n", width, height);
    else
        fprintf(stderr, "Failed to write canvas_ity_example.tga\n");

    free(image);
    ci_canvas_destroy(ctx);
    return 0;
}
