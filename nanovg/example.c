/*
 * example.c -- NanoVG + canvas_ity demo
 *
 * Renders various shapes with gradients and compositing to screenshot.png.
 * Runs headless (no windowing system required).
 *
 * Build:  make example
 * Run:    ./build/example
 */

#define CANVAS_ITY_IMPLEMENTATION
#include "../src/canvas_ity.h"

#include "nanovg.h"
#include "nanovg.c"

#define NANOVG_CI_IMPLEMENTATION
#include "nanovg_ci.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <math.h>

#define WIDTH  480
#define HEIGHT 360

int main(void)
{
    NVGcontext *vg;
    unsigned char *pixels;
    NVGpaint bg_grad, btn_grad, circle_grad;
    int stride = WIDTH * 4;
    float cx, cy;
    int i;

    vg = nvgCreateCI(WIDTH, HEIGHT, NVG_ANTIALIAS);
    if (!vg) {
        fprintf(stderr, "Failed to create NanoVG context\n");
        return 1;
    }

    nvgBeginFrame(vg, (float)WIDTH, (float)HEIGHT, 1.0f);

    /* Dark background */
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)WIDTH, (float)HEIGHT);
    bg_grad = nvgLinearGradient(vg, 0, 0, 0, (float)HEIGHT,
        nvgRGBA(40, 44, 52, 255), nvgRGBA(25, 28, 34, 255));
    nvgFillPaint(vg, bg_grad);
    nvgFill(vg);

    /* Rounded rectangle "button" with gradient */
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 30, 30, 200, 50, 8);
    btn_grad = nvgLinearGradient(vg, 30, 30, 30, 80,
        nvgRGBA(80, 140, 240, 255), nvgRGBA(50, 100, 200, 255));
    nvgFillPaint(vg, btn_grad);
    nvgFill(vg);

    /* Button stroke */
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 30, 30, 200, 50, 8);
    nvgStrokeColor(vg, nvgRGBA(100, 160, 255, 128));
    nvgStrokeWidth(vg, 1.5f);
    nvgStroke(vg);

    /* Circle with radial gradient */
    cx = 350.0f;
    cy = 100.0f;
    nvgBeginPath(vg);
    nvgCircle(vg, cx, cy, 60);
    circle_grad = nvgRadialGradient(vg, cx, cy, 10, 60,
        nvgRGBA(255, 200, 50, 255), nvgRGBA(200, 80, 20, 200));
    nvgFillPaint(vg, circle_grad);
    nvgFill(vg);

    /* Stroked triangle */
    nvgBeginPath(vg);
    nvgMoveTo(vg, 80, 200);
    nvgLineTo(vg, 180, 320);
    nvgLineTo(vg, 30, 280);
    nvgClosePath(vg);
    nvgStrokeColor(vg, nvgRGBA(200, 220, 255, 200));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    /* Semi-transparent filled triangle */
    nvgBeginPath(vg);
    nvgMoveTo(vg, 80, 200);
    nvgLineTo(vg, 180, 320);
    nvgLineTo(vg, 30, 280);
    nvgClosePath(vg);
    nvgFillColor(vg, nvgRGBA(100, 200, 150, 80));
    nvgFill(vg);

    /* Polyline (stroked only) */
    nvgBeginPath(vg);
    for (i = 0; i < 8; i++) {
        float x = 240.0f + (float)i * 28.0f;
        float y = 250.0f + (float)sin((double)i * 0.9) * 40.0f;
        if (i == 0)
            nvgMoveTo(vg, x, y);
        else
            nvgLineTo(vg, x, y);
    }
    nvgStrokeColor(vg, nvgRGBA(255, 100, 100, 220));
    nvgStrokeWidth(vg, 2.5f);
    nvgLineCap(vg, NVG_ROUND);
    nvgLineJoin(vg, NVG_ROUND);
    nvgStroke(vg);

    /* Small solid rectangles */
    nvgBeginPath(vg);
    nvgRect(vg, 260, 180, 40, 40);
    nvgFillColor(vg, nvgRGBA(255, 255, 100, 180));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 310, 195, 40, 40);
    nvgFillColor(vg, nvgRGBA(100, 255, 100, 120));
    nvgFill(vg);

    /* Ellipse */
    nvgBeginPath(vg);
    nvgEllipse(vg, 400, 280, 60, 35);
    nvgFillColor(vg, nvgRGBA(180, 100, 255, 150));
    nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBA(220, 180, 255, 200));
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);

    nvgEndFrame(vg);

    /* Read pixels and write PNG */
    pixels = (unsigned char *)malloc((size_t)(stride * HEIGHT));
    if (!pixels) {
        nvgDeleteCI(vg);
        return 1;
    }
    nvgciReadPixels(vg, pixels, WIDTH, HEIGHT, stride);

    if (!stbi_write_png("screenshot.png", WIDTH, HEIGHT, 4, pixels, stride)) {
        fprintf(stderr, "Failed to write screenshot.png\n");
    } else {
        printf("Wrote screenshot.png (%dx%d)\n", WIDTH, HEIGHT);
    }

    free(pixels);
    nvgDeleteCI(vg);
    return 0;
}
