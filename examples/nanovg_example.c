/*
 * nanovg_example.c -- NanoVG + canvas_ity demo
 *
 * Renders shapes with gradients, strokes, and alpha blending using
 * NanoVG's API backed by canvas_ity's rasterizer.  Writes output to
 * nanovg_example.tga.
 *
 * Build:
 *   gcc -std=c89 -O2 -I../src -DNVG_NO_STB -o nanovg_example nanovg_example.c -lm
 *
 * Or via the Makefile:
 *   make nanovg_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

#include "../nanovg/nanovg.h"
#include "../nanovg/nanovg.c"

#define NANOVG_CI_IMPLEMENTATION
#include "../nanovg/nanovg_ci.h"

#define WIDTH  400
#define HEIGHT 300

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
    NVGcontext *vg;
    unsigned char *pixels;
    NVGpaint grad;
    int stride = WIDTH * 4;
    int i;

    vg = nvgCreateCI(WIDTH, HEIGHT, NVG_ANTIALIAS);
    if (!vg) {
        fprintf(stderr, "Failed to create NanoVG context\n");
        return 1;
    }

    nvgBeginFrame(vg, (float)WIDTH, (float)HEIGHT, 1.0f);

    /* Dark background with gradient */
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)WIDTH, (float)HEIGHT);
    grad = nvgLinearGradient(vg, 0, 0, 0, (float)HEIGHT,
        nvgRGBA(30, 30, 50, 255), nvgRGBA(10, 10, 20, 255));
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    /* Rounded rectangle with linear gradient */
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 20, 20, 160, 60, 10);
    grad = nvgLinearGradient(vg, 20, 20, 20, 80,
        nvgRGBA(80, 160, 240, 255), nvgRGBA(40, 100, 200, 255));
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    /* Circle with radial gradient */
    nvgBeginPath(vg);
    nvgCircle(vg, 310, 70, 50);
    grad = nvgRadialGradient(vg, 310, 70, 5, 50,
        nvgRGBA(255, 220, 80, 255), nvgRGBA(200, 60, 20, 200));
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    /* Stroked + filled polygon */
    nvgBeginPath(vg);
    nvgMoveTo(vg, 100, 150);
    nvgLineTo(vg, 180, 280);
    nvgLineTo(vg, 20, 230);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBA(60, 180, 120, 120));
    nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBA(180, 240, 200, 220));
    nvgStrokeWidth(vg, 2.5f);
    nvgStroke(vg);

    /* Sine wave polyline */
    nvgBeginPath(vg);
    for (i = 0; i < 10; i++) {
        float x = 210.0f + (float)i * 18.0f;
        float y = 200.0f + (float)sin((double)i * 0.8) * 40.0f;
        if (i == 0) nvgMoveTo(vg, x, y);
        else        nvgLineTo(vg, x, y);
    }
    nvgStrokeColor(vg, nvgRGBA(255, 120, 80, 220));
    nvgStrokeWidth(vg, 2.0f);
    nvgLineCap(vg, NVG_ROUND);
    nvgLineJoin(vg, NVG_ROUND);
    nvgStroke(vg);

    /* Overlapping semi-transparent rectangles */
    nvgBeginPath(vg);
    nvgRect(vg, 240, 130, 50, 50);
    nvgFillColor(vg, nvgRGBA(255, 80, 80, 150));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 260, 150, 50, 50);
    nvgFillColor(vg, nvgRGBA(80, 80, 255, 150));
    nvgFill(vg);

    /* Ellipse */
    nvgBeginPath(vg);
    nvgEllipse(vg, 310, 260, 60, 30);
    nvgFillColor(vg, nvgRGBA(200, 130, 255, 160));
    nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBA(230, 200, 255, 200));
    nvgStrokeWidth(vg, 1.5f);
    nvgStroke(vg);

    nvgEndFrame(vg);

    /* Read pixels and write TGA */
    pixels = (unsigned char *)malloc((size_t)(stride * HEIGHT));
    if (!pixels) { nvgDeleteCI(vg); return 1; }
    nvgciReadPixels(vg, pixels, WIDTH, HEIGHT, stride);

    if (write_tga("nanovg_example.tga", pixels, WIDTH, HEIGHT))
        printf("Wrote nanovg_example.tga (%dx%d)\n", WIDTH, HEIGHT);
    else
        fprintf(stderr, "Failed to write nanovg_example.tga\n");

    free(pixels);
    nvgDeleteCI(vg);
    return 0;
}
