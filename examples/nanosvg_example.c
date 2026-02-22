/*
 * nanosvg_example.c -- NanoSVG + canvas_ity demo
 *
 * Parses a small inline SVG, rasterizes it with NanoSVG's built-in
 * rasterizer, then composites the result onto a canvas_ity surface.
 * Writes output to nanosvg_example.tga.
 *
 * Build:
 *   gcc -std=c89 -O2 -I../src -o nanosvg_example nanosvg_example.c -lm
 *
 * Or via the Makefile:
 *   make nanosvg_example
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

#define WIDTH  256
#define HEIGHT 256

/* A small SVG string embedded directly in the program. */
static const char svg_source[] =
    "<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 256 256'>"
    "  <rect width='256' height='256' fill='#1a1a2e'/>"
    "  <circle cx='128' cy='100' r='60' fill='#e94560'/>"
    "  <rect x='60' y='150' width='136' height='60' rx='12' fill='#0f3460'/>"
    "  <polygon points='128,30 148,80 108,80' fill='#f5a623'/>"
    "  <line x1='40' y1='230' x2='216' y2='230'"
    "        stroke='#16c79a' stroke-width='4' stroke-linecap='round'/>"
    "</svg>";

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
    char *svg_copy;

    /* nsvgParse modifies its input, so copy the string. */
    svg_copy = (char *)malloc(sizeof(svg_source));
    if (!svg_copy) return 1;
    memcpy(svg_copy, svg_source, sizeof(svg_source));

    /* Parse the SVG. */
    svg = nsvgParse(svg_copy, "px", 96.0f);
    free(svg_copy);
    if (!svg) {
        fprintf(stderr, "Failed to parse SVG\n");
        return 1;
    }
    printf("SVG size: %.0f x %.0f\n", svg->width, svg->height);

    /* Rasterize with NanoSVG. */
    rast = nsvgCreateRasterizer();
    if (!rast) {
        nsvgDelete(svg);
        return 1;
    }
    svg_pixels = (unsigned char *)malloc((size_t)(WIDTH * HEIGHT * 4));
    if (!svg_pixels) {
        nsvgDeleteRasterizer(rast);
        nsvgDelete(svg);
        return 1;
    }
    nsvgRasterize(rast, svg, 0, 0, 1.0f,
                  svg_pixels, WIDTH, HEIGHT, WIDTH * 4);

    /* Composite onto a canvas_ity surface. */
    ctx = ci_canvas_create(WIDTH, HEIGHT);

    /* Draw a subtle background gradient first. */
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE,
        0.0f, 0.0f, (float)WIDTH, (float)HEIGHT);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.0f, 0.15f, 0.15f, 0.25f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        1.0f, 0.05f, 0.05f, 0.1f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0, 0, (float)WIDTH, (float)HEIGHT);

    /* Overlay the rasterized SVG via put_image_data. */
    ci_canvas_draw_image(ctx, svg_pixels, WIDTH, HEIGHT, WIDTH * 4,
                         0.0f, 0.0f, (float)WIDTH, (float)HEIGHT);

    /* Read final pixels. */
    canvas_pixels = (unsigned char *)malloc((size_t)(WIDTH * HEIGHT * 4));
    ci_canvas_get_image_data(ctx, canvas_pixels,
                             WIDTH, HEIGHT, WIDTH * 4, 0, 0);

    if (write_tga("nanosvg_example.tga", canvas_pixels, WIDTH, HEIGHT))
        printf("Wrote nanosvg_example.tga (%dx%d)\n", WIDTH, HEIGHT);
    else
        fprintf(stderr, "Failed to write nanosvg_example.tga\n");

    free(canvas_pixels);
    free(svg_pixels);
    ci_canvas_destroy(ctx);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(svg);
    return 0;
}
