/*
 * tiger_example.c -- NanoSVG + canvas_ity: classic SVG tiger
 *
 * Loads tiger.svg (Ghostscript tiger), rasterizes it with NanoSVG,
 * composites onto a canvas_ity surface, and writes tiger.tga.
 *
 * Build:
 *   gcc -std=c89 -O2 -I../src -o tiger_example tiger_example.c -lm
 *
 * Or via the Makefile:
 *   make tiger_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

#define NANOSVG_IMPLEMENTATION
#include "../nanovg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "../nanovg/nanosvgrast.h"

#define WIDTH  512
#define HEIGHT 512

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
    NSVGimage *svg;
    NSVGrasterizer *rast;
    unsigned char *svg_pixels;
    unsigned char *canvas_pixels;
    ci_canvas_t *ctx;
    float scale;

    svg = nsvgParseFromFile("tiger.svg", "px", 96.0f);
    if (!svg) {
        fprintf(stderr, "Failed to load tiger.svg\n");
        return 1;
    }
    printf("SVG size: %.0f x %.0f\n", svg->width, svg->height);

    /* Scale to fit WIDTH x HEIGHT, preserving aspect ratio. */
    scale = (float)WIDTH / svg->width;
    if (svg->height * scale > (float)HEIGHT)
        scale = (float)HEIGHT / svg->height;

    rast = nsvgCreateRasterizer();
    if (!rast) { nsvgDelete(svg); return 1; }

    svg_pixels = (unsigned char *)malloc((size_t)(WIDTH * HEIGHT * 4));
    if (!svg_pixels) {
        nsvgDeleteRasterizer(rast);
        nsvgDelete(svg);
        return 1;
    }
    memset(svg_pixels, 0, (size_t)(WIDTH * HEIGHT * 4));
    nsvgRasterize(rast, svg, 0, 0, scale,
                  svg_pixels, WIDTH, HEIGHT, WIDTH * 4);

    /* Composite onto a canvas_ity surface with a background. */
    ctx = ci_canvas_create(WIDTH, HEIGHT);

    /* Light grey background. */
    ci_canvas_set_color(ctx, CI_FILL_STYLE,
        0.93f, 0.93f, 0.93f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0, 0, (float)WIDTH, (float)HEIGHT);

    /* Draw the rasterized tiger. */
    ci_canvas_draw_image(ctx, svg_pixels, WIDTH, HEIGHT, WIDTH * 4,
                         0.0f, 0.0f, (float)WIDTH, (float)HEIGHT);

    canvas_pixels = (unsigned char *)malloc((size_t)(WIDTH * HEIGHT * 4));
    ci_canvas_get_image_data(ctx, canvas_pixels,
                             WIDTH, HEIGHT, WIDTH * 4, 0, 0);

    if (write_tga("tiger.tga", canvas_pixels, WIDTH, HEIGHT))
        printf("Wrote tiger.tga (%dx%d)\n", WIDTH, HEIGHT);
    else
        fprintf(stderr, "Failed to write tiger.tga\n");

    free(canvas_pixels);
    free(svg_pixels);
    ci_canvas_destroy(ctx);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(svg);
    return 0;
}
