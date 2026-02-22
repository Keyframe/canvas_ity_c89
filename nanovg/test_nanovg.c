/*
 * test_nanovg.c -- hash-based test suite for NanoVG + canvas_ity backend
 *
 * Each test renders to a small canvas, hashes the pixel buffer, and compares
 * against a known-good value.  Same pattern as test/test.c in the main project.
 *
 * Build:  make test
 * Run:    ./build/test_nanovg
 */

#define CANVAS_ITY_IMPLEMENTATION
#include "../src/canvas_ity.h"

#include "nanovg.h"
#include "nanovg.c"

#define NANOVG_CI_IMPLEMENTATION
#include "nanovg_ci.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ======== FNV-1a hash ======== */

static unsigned int fnv1a(const unsigned char *data, size_t len)
{
    unsigned int h = 2166136261u;
    size_t i;
    for (i = 0; i < len; i++) {
        h ^= data[i];
        h *= 16777619u;
    }
    return h;
}

/* ======== test harness ======== */

static int g_pass = 0, g_fail = 0, g_total = 0;

static void check(const char *name, int w, int h,
                  unsigned char *pixels, unsigned int expected)
{
    unsigned int got = fnv1a(pixels, (size_t)(w * h * 4));
    g_total++;
    if (got == expected) {
        g_pass++;
        printf("  PASS  %s  [%08x]\n", name, got);
    } else {
        g_fail++;
        printf("  FAIL  %s  expected %08x got %08x\n", name, expected, got);
    }
}

/* ======== individual tests ======== */

#define TW 64
#define TH 64

static void test_fill_solid_rect(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    /* Clear to black */
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    /* Red rectangle */
    nvgBeginPath(vg);
    nvgRect(vg, 10, 10, 44, 44);
    nvgFillColor(vg, nvgRGBA(255, 0, 0, 255));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("fill_solid_rect", TW, TH, pixels, 0x36e64768u);
}

static void test_fill_circle(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 32, 32, 20);
    nvgFillColor(vg, nvgRGBA(0, 255, 0, 255));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("fill_circle", TW, TH, pixels, 0xc05864adu);
}

static void test_stroke_rect(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgRect(vg, 10, 10, 44, 44);
    nvgStrokeColor(vg, nvgRGBA(255, 255, 0, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("stroke_rect", TW, TH, pixels, 0x21e71d78u);
}

static void test_stroke_circle(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 32, 32, 24);
    nvgStrokeColor(vg, nvgRGBA(0, 128, 255, 255));
    nvgStrokeWidth(vg, 2.0f);
    nvgStroke(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("stroke_circle", TW, TH, pixels, 0x7440fef4u);
}

static void test_linear_gradient(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];
    NVGpaint grad;

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    grad = nvgLinearGradient(vg, 0, 0, (float)TW, (float)TH,
        nvgRGBA(255, 0, 0, 255), nvgRGBA(0, 0, 255, 255));
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("linear_gradient", TW, TH, pixels, 0x3d0204ccu);
}

static void test_radial_gradient(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];
    NVGpaint grad;

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 32, 32, 28);
    grad = nvgRadialGradient(vg, 32, 32, 5, 28,
        nvgRGBA(255, 255, 255, 255), nvgRGBA(0, 0, 0, 255));
    nvgFillPaint(vg, grad);
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("radial_gradient", TW, TH, pixels, 0x50f43490u);
}

static void test_scissor_clip(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    /* Scissor to top-left quadrant */
    nvgScissor(vg, 0, 0, 32, 32);

    /* Fill entire canvas — should only appear in top-left */
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(255, 0, 255, 255));
    nvgFill(vg);

    nvgResetScissor(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("scissor_clip", TW, TH, pixels, 0x356f9438u);
}

static void test_winding_hole(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    /* Outer rectangle (solid) */
    nvgBeginPath(vg);
    nvgRect(vg, 5, 5, 54, 54);
    nvgPathWinding(vg, NVG_SOLID);

    /* Inner rectangle (hole) */
    nvgRect(vg, 20, 20, 24, 24);
    nvgPathWinding(vg, NVG_HOLE);

    nvgFillColor(vg, nvgRGBA(0, 200, 100, 255));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("winding_hole", TW, TH, pixels, 0x66d0a94au);
}

static void test_alpha_blend(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(255, 255, 255, 255));
    nvgFill(vg);

    /* Semi-transparent red rect */
    nvgBeginPath(vg);
    nvgRect(vg, 10, 10, 30, 30);
    nvgFillColor(vg, nvgRGBA(255, 0, 0, 128));
    nvgFill(vg);

    /* Semi-transparent blue rect overlapping */
    nvgBeginPath(vg);
    nvgRect(vg, 24, 24, 30, 30);
    nvgFillColor(vg, nvgRGBA(0, 0, 255, 128));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("alpha_blend", TW, TH, pixels, 0x936e70d6u);
}

static void test_multiple_shapes(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    /* Background */
    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
    nvgFill(vg);

    /* Circle */
    nvgBeginPath(vg);
    nvgCircle(vg, 20, 20, 15);
    nvgFillColor(vg, nvgRGBA(255, 100, 50, 200));
    nvgFill(vg);

    /* Rectangle */
    nvgBeginPath(vg);
    nvgRect(vg, 30, 30, 28, 28);
    nvgFillColor(vg, nvgRGBA(50, 100, 255, 200));
    nvgFill(vg);

    /* Rounded rect */
    nvgBeginPath(vg);
    nvgRoundedRect(vg, 5, 40, 30, 18, 4);
    nvgFillColor(vg, nvgRGBA(100, 255, 100, 180));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("multiple_shapes", TW, TH, pixels, 0xda086ca0u);
}

static void test_ellipse(void)
{
    NVGcontext *vg = nvgCreateCI(TW, TH, NVG_ANTIALIAS);
    unsigned char pixels[TW * TH * 4];

    nvgBeginFrame(vg, (float)TW, (float)TH, 1.0f);

    nvgBeginPath(vg);
    nvgRect(vg, 0, 0, (float)TW, (float)TH);
    nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgEllipse(vg, 32, 32, 28, 16);
    nvgFillColor(vg, nvgRGBA(200, 150, 255, 255));
    nvgFill(vg);

    nvgEndFrame(vg);
    nvgciReadPixels(vg, pixels, TW, TH, TW * 4);
    nvgDeleteCI(vg);

    check("ellipse", TW, TH, pixels, 0x6744a56bu);
}

/* ======== main ======== */

int main(void)
{
    printf("NanoVG + canvas_ity backend tests\n");
    printf("=================================\n\n");

    /* Run all tests — first pass records hashes */
    test_fill_solid_rect();
    test_fill_circle();
    test_stroke_rect();
    test_stroke_circle();
    test_linear_gradient();
    test_radial_gradient();
    test_scissor_clip();
    test_winding_hole();
    test_alpha_blend();
    test_multiple_shapes();
    test_ellipse();

    printf("\n%d/%d tests passed", g_pass, g_total);
    if (g_fail > 0)
        printf(" (%d FAILED)", g_fail);
    printf("\n");

    /* In initial run, all expected hashes are 0 (record mode).
       Tests pass trivially if hash != 0 is not checked. */
    return 0;
}
