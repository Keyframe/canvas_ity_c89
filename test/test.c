/* canvas_ity test suite and harness v1.00 -- ISC license
 * Copyright (c) 2022 Andrew Kensler
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * C89 port of the C++ test suite.
 */

/* POSIX feature test macros - must precede all includes */
#if !defined(_WIN32)
#define _POSIX_C_SOURCE 199309L
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS
#endif

#define CANVAS_ITY_IMPLEMENTATION
#include "../src/canvas_ity.h"

#if defined( __linux__ )
#include <time.h>
#include <unistd.h>
#elif defined( _WIN32 )
#include <windows.h>
#elif defined( __MACH__ )
#include <mach/mach_time.h>
#include <unistd.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ======== UTILITY MACROS ======== */

#define CI_MINF(a,b) ((a)<(b)?(a):(b))
#define CI_MAXF(a,b) ((a)>(b)?(a):(b))

/* ======== BYTE BUFFER TYPE ======== */

typedef struct {
    unsigned char *data;
    size_t size;
} ci_byte_buf_t;

static ci_byte_buf_t font_a, font_b, font_c, font_d, font_e, font_f, font_g;

/* ======== RESOURCES ======== */

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverlength-strings"
#endif

static char const font_a_base64[] =
    "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"
    "FxY3FhUUBwYnJgcCFxYXFjcGJyYTEjU0aCIGBBxcQhUKIAMIVD+VMjKMTk8BCAgJoVVJOc3z"
    "ERQDLgUXEBZKQhUECyBQAgEHCi41AwcBAVH+u4mnAQEnlAQFAQEBNKpSAAH/+f+6A7QDjAAe"
    "AAAlJgEmJwUyFRQHBhUUEzYTNicmJzQ3NjcGBwAHBgciAbYX/tMRaAFkHh494U93Bz4sASik"
    "hV8Y/uEJDR4kDoACfSRdAhYSCxZAJv4/LwHaGRIMGhABAgU9Rv1/VHkBAAH/+f5tA7QDjAAm"
    "AAAlNAEmJwUyFRQHBhUUEzYTNicmJzQ3NjcGBwIHAgcGIyY1Njc2NzYBqv7IEWgBZB4ePeFF"
    "gQc+LAEopIVfGOJGngEYOFgBWSAGWixJApYkXQIWEgsWQCb+PygB4RoRDBoQAQIFPUb927D+"
    "cQM1AVAZGgkNy///ABT/+gLXB/MQZwAMABEC1T/4QAASBgAEAAAAAQEFAyMCxgUeAA0AAAE2"
    "EzY3NhcWBwYHBicmARAwqBoOWkoSHsKSFBwfA0prASUtAxQYBSf+ohcHBwAAEAAA/nAHkAYA"
    "AAMABwALAA8AEwAXABsAHwAjACcAKwAvADMANwA7AD8AABAQIBAAECARABAhEAAQIRESESAQ"
    "ABEgEQARIRAAESERExAgEAEQIBEBECEQARAhERMRIBABESARAREhEAERIREBkP5wAZD+cAGQ"
    "/nABkHABkP5wAZD+cAGQ/nABkHABkP5wAZD+cAGQ/nABkHABkP5wAZD+cAGQ/nABkP5wAZD+"
    "cAIAAZD+cAIAAZD+cAIAAZD+cPoAAZD+cAIAAZD+cAIAAZD+cAIAAZD+cPoAAZD+cAIAAZD+"
    "cAIAAZD+cAIAAZD+cPoAAZD+cAIAAZD+cAIAAZD+cAIAAZD+cAD//wBkADIDIAWqECcAEgAA"
    "/qIABAASAAP//wBkAZADIARMEAYAEgAA//8AZAGQAyAETBAGABIAAP//AGQBkAMgBEwQBgAS"
    "AAAAAQBkAZADIARMAAMAABIgECBkArz9RARM/UQAAAAAAAAMAJYAAQAAAAAAAQAFAAAAAQAA"
    "AAAAAgAHAAUAAQAAAAAAAwAFAAAAAQAAAAAABAAFAAAAAQAAAAAABQALAAwAAQAAAAAABgAF"
    "AAAAAwABBAkAAQAKABcAAwABBAkAAgAOACEAAwABBAkAAwAKABcAAwABBAkABAAKABcAAwAB"
    "BAkABQAWAC8AAwABBAkABgAKABdGb250QVJlZ3VsYXJWZXJzaW9uIDEuMABGAG8AbgB0AEEA"
    "UgBlAGcAdQBsAGEAcgBWAGUAcgBzAGkAbwBuACAAMQAuADAAAAMAAAAAAAD/ZgBmAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAA=";

static char const font_b_base64[] =
    "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"
    "PeFPdwc+LAEopIVfGP7hCQ0eJA6AAn0kXQIWEgsWQCb+Py8B2hkSDBoQAQIFPUb9f1R5AQAB"
    "//n+bQO0A4wAJgAAJTQBJicFMhUUBwYVFBM2EzYnJic0NzY3BgcCBwIHBiMmNTY3Njc2Aar+"
    "yBFoAWQeHj3hRYEHPiwBKKSFXxjiRp4BGDhYAVkgBlosSQKWJF0CFhILFkAm/j8oAeEaEQwa"
    "EAECBT1G/duw/nEDNQFQGRoJDcv//wAU//oC1wfzEGcADAARAtU/+EAAEgYABAAAAAEBBQMj"
    "AsYFHgANAAABNhM2NzYXFgcGBwYnJgEQMKgaDlpKEh7CkhQcHwNKawElLQMUGAUn/qIXBwcA"
    "ABAAAP5wB5AGAAADAAcACwAPABMAFwAbAB8AIwAnACsALwAzADcAOwA/AAAQECAQABAgEQAQ"
    "IRAAECEREhEgEAARIBEAESEQABEhERMQIBABECARARAhEAEQIRETESAQAREgEQERIRABESER"
    "AZD+cAGQ/nABkP5wAZBwAZD+cAGQ/nABkP5wAZBwAZD+cAGQ/nABkP5wAZBwAZD+cAGQ/nAB"
    "kP5wAZD+cAGQ/nACAAGQ/nACAAGQ/nACAAGQ/nD6AAGQ/nACAAGQ/nACAAGQ/nACAAGQ/nD6"
    "AAGQ/nACAAGQ/nACAAGQ/nACAAGQ/nD6AAGQ/nACAAGQ/nACAAGQ/nACAAGQ/nAA//8AZAAy"
    "AyAFqhAnABIAAP6iAAQAEgAD//8AZAGQAyAETBAGABIAAP//AGQBkAMgBEwQBgASAAD//wBk"
    "AZADIARMEAYAEgAAAAEAZAGQAyAETAADAAASIBAgZAK8/UQETP1EAAAAAAAADACWAAEAAAAA"
    "AAEABQAAAAEAAAAAAAIABwAFAAEAAAAAAAMABQAAAAEAAAAAAAQABQAAAAEAAAAAAAUACwAM"
    "AAEAAAAAAAYABQAAAAMAAQQJAAEACgAXAAMAAQQJAAIADgAhAAMAAQQJAAMACgAXAAMAAQQJ"
    "AAQACgAXAAMAAQQJAAUAFgAvAAMAAQQJAAYACgAXRm9udEJSZWd1bGFyVmVyc2lvbiAxLjAA"
    "RgBvAG4AdABCAFIAZQBnAHUAbABhAHIAVgBlAHIAcwBpAG8AbgAgADEALgAwAAADAAAAAAAA"
    "/2YAZgAAAAAAAAAAAAAAAAAAAAAAAAAA";

static char const font_c_base64[] =
    "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"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAA=";

static char const font_d_base64[] =
    "AAEAAAA=";

static char const font_e_base64[] =
    "AAEAAAALAIAAAwAwT1MvMmisck8AAAE4AAAAYGNtYXAXewGCAAAB3AAAAUJjdnQgAEQFEQAA"
    "AyAAAAAEZ2x5ZjCUlAIAAANMAAAGhmhlYWQe1bIjAAAAvAAAADZoaGVhDf8FBAAAAPQAAAAk"
    "aG10eDmaBAMAAAGYAAAARGxvY2ERbxMOAAADJAAAAChtYXhwAHUAtwAAARgAAAAgbmFtZVZp"
    "NvsAAAnUAAAA23Bvc3T/aQBmAAAKsAAAAA==";

static char const font_f_base64[] =
    "AAEAAAALAIAAAwAwT1MvMmisck8AAAE4AAAAYGNtYXAXewGCAAAB3AAAAUJjdnQgAEQFEQAA"
    "AyAAAAAEZ2x5ZjCUlAIAAANMAAAGhmhlYWQe1bIjAAAAvAAAADZoaGVhDf8FBAAAAPQAAAAk"
    "aG10eDmaBAMAAAGYAAAARGxvY2ERbxMOAAADJAAAAChtYXhwAHUAtwAAARgAAAAgbmFtZVZp"
    "NvsAAAnUAAAA23Bvc3T/aQBmAAAKsAAAACAAAQAAAAEAAEPW4v5fDzz1AB0IAAAAAADcB1gv"
    "AAAAANwUDpf/+f5tB5AH8wAAAAgAAgAAAAAAAA==";

static char const font_g_base64[] =
    "AAEAAAABABAAAAAAaGVhZB7h+0cAAAAcAAAANgABAAAAAQAAC4VXZl8PPPUAHQgAAAAAANwH"
    "WC8AAAAA3CBXu//5/m0HkAfzAAAACAACAAAAAAAA";

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/* ======== TEST SUITE ======== */

static void scale_uniform(ci_canvas_t *ctx, float width, float height)
{
    float size;
    float segments[1];
    segments[0] = 1.0f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_dash(ctx, segments, 1);
    ctx->line_cap = CI_CAP_CIRCLE;
    for (size = 8.0f; size < CI_MINF(width, height); size *= 2.0f)
    {
        ci_canvas_scale(ctx, 2.0f, 2.0f);
        ci_canvas_stroke_rectangle(ctx, 0.0f, 0.0f, 8.0f, 8.0f);
    }
}

static void scale_non_uniform(ci_canvas_t *ctx, float width, float height)
{
    float segments[1];
    segments[0] = 4.0f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_dash(ctx, segments, 1);
    ci_canvas_scale(ctx, 4.0f, 0.5f);
    ci_canvas_stroke_rectangle(ctx, width * 0.125f / 4.0f, height * 0.125f / 0.5f,
                               width * 0.75f / 4.0f, height * 0.75f / 0.5f);
}

static void test_rotate(ci_canvas_t *ctx, float width, float height)
{
    int step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0; step < 64; ++step)
    {
        ci_canvas_rotate(ctx, 3.14159265f / 2.0f / 64.0f);
        ci_canvas_stroke_rectangle(ctx, 0.0f, 0.0f, width, height);
    }
}

static void test_translate(ci_canvas_t *ctx, float width, float height)
{
    float step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0.0f; step < 32.0f; step += 1.0f)
    {
        ci_canvas_translate(ctx, (0.5f - step / 32.0f) * width * 0.2f,
                            height / 32.0f);
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, 0.0f, 0.0f, width * 0.125f, 0.0f, 6.28318531f, 0);
        ci_canvas_close_path(ctx);
        ci_canvas_stroke(ctx);
    }
}

static void test_transform(ci_canvas_t *ctx, float width, float height)
{
    int step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0; step < 8; ++step)
    {
        ci_canvas_transform(ctx, 1.0f, 0.0f, 0.1f, 1.0f, width * -0.05f, 0.0f);
        ci_canvas_stroke_rectangle(ctx, width * 0.25f, height * 0.25f,
                                   width * 0.5f, height * 0.5f);
    }
}

static void transform_fill(ci_canvas_t *ctx, float width, float height)
{
    unsigned char checker[1024];
    int index;
    for (index = 0; index < 1024; ++index)
        checker[index] = (unsigned char)(
            ((index >> 5 & 1) ^ (index >> 9 & 1) ^
             ((index & 3) == 3)) * 255);
    ci_canvas_set_pattern(ctx, CI_FILL_STYLE, checker, 16, 16, 64, CI_REPEAT);
    ci_canvas_begin_path(ctx);
    ci_canvas_rectangle(ctx, width * 0.2f, height * 0.2f,
                        width * 0.6f, height * 0.6f);
    ci_canvas_transform(ctx, 1.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f);
    ci_canvas_fill(ctx);
}

static void transform_stroke(ci_canvas_t *ctx, float width, float height)
{
    float segments[4];
    segments[0] = 22.0f; segments[1] = 8.0f;
    segments[2] = 10.0f; segments[3] = 8.0f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_set_line_dash(ctx, segments, 4);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, width * 0.5f, height * 0.5f, CI_MINF(width, height) * 0.4f,
                  0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_transform(ctx, 1.0f, 1.0f, 0.0f, 2.0f, 0.0f, 0.0f);
    ci_canvas_stroke(ctx);
}

static void test_set_transform(ci_canvas_t *ctx, float width, float height)
{
    int step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0; step < 8; ++step)
        ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.1f, 1.0f, width * -0.05f, 0.0f);
    ci_canvas_stroke_rectangle(ctx, width * 0.25f, height * 0.25f,
                               width * 0.5f, height * 0.5f);
}

static void test_global_alpha(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 3.0f);
    for (y = 0.0f; y < 6.0f; y += 1.0f)
        for (x = 0.0f; x < 6.0f; x += 1.0f)
        {
            ci_canvas_set_color(ctx, CI_FILL_STYLE, x / 5.0f, 1.0f, y / 5.0f, x / 5.0f);
            ci_canvas_set_global_alpha(ctx, y / 4.0f - 0.25f);
            ci_canvas_begin_path(ctx);
            ci_canvas_rectangle(ctx,
                (x + 0.1f) / 6.0f * width, (y + 0.1f) / 6.0f * height,
                0.8f / 6.0f * width, 0.8f / 6.0f * height);
            ci_canvas_fill(ctx);
            ci_canvas_stroke(ctx);
        }
}

static void test_global_composite_operation(ci_canvas_t *ctx, float width, float height)
{
    ci_composite_operation operations[11];
    float box_width, box_height;
    int index;
    operations[0] = CI_SOURCE_IN;
    operations[1] = CI_SOURCE_COPY;
    operations[2] = CI_SOURCE_OUT;
    operations[3] = CI_DESTINATION_IN;
    operations[4] = CI_DESTINATION_ATOP;
    operations[5] = CI_LIGHTER;
    operations[6] = CI_DESTINATION_OVER;
    operations[7] = CI_DESTINATION_OUT;
    operations[8] = CI_SOURCE_ATOP;
    operations[9] = CI_SOURCE_OVER;
    operations[10] = CI_EXCLUSIVE_OR;
    box_width = 0.25f * width;
    box_height = 0.25f * height;
    for (index = 0; index < 11; ++index)
    {
        float column = (float)(operations[index] % 4);
        float row = (float)(operations[index] / 4);
        ci_canvas_save(ctx);
        ci_canvas_begin_path(ctx);
        ci_canvas_rectangle(ctx, column * box_width, row * box_height,
                            box_width, box_height);
        ci_canvas_clip(ctx);
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
        ci_canvas_fill_rectangle(ctx, (column + 0.4f) * box_width,
                                 (row + 0.4f) * box_height,
                                 0.4f * box_width, 0.4f * box_height);
        ctx->global_composite_operation = operations[index];
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_fill_rectangle(ctx, (column + 0.2f) * box_width,
                                 (row + 0.2f) * box_height,
                                 0.4f * box_width, 0.4f * box_height);
        ci_canvas_restore(ctx);
    }
}

static void shadow_color(ci_canvas_t *ctx, float width, float height)
{
    float row;
    ctx->shadow_offset_x = 5.0f;
    ctx->shadow_offset_y = 5.0f;
    ci_canvas_set_shadow_blur(ctx, 1.0f);
    for (row = 0.0f; row < 5.0f; row += 1.0f)
    {
        float y = (row + 0.25f) * 0.2f * height;
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 0.25f * row);
        ci_canvas_set_shadow_color(ctx, 1.0f, -1.0f, 0.0f, 0.25f);
        ci_canvas_fill_rectangle(ctx, 0.05f * width, y,
                                 0.15f * width, 0.1f * height);
        ci_canvas_set_shadow_color(ctx, 0.0f, 1.0f, 0.0f, 0.5f);
        ci_canvas_fill_rectangle(ctx, 0.30f * width, y,
                                 0.15f * width, 0.1f * height);
        ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 2.0f, 0.75f);
        ci_canvas_fill_rectangle(ctx, 0.55f * width, y,
                                 0.15f * width, 0.1f * height);
        ci_canvas_set_shadow_color(ctx, 1.0f, 1.0f, 1.0f, 100.0f);
        ci_canvas_fill_rectangle(ctx, 0.80f * width, y,
                                 0.15f * width, 0.1f * height);
    }
}

static void shadow_offset(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_shadow_blur(ctx, 2.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    for (y = 0.0f; y < 5.0f; y += 1.0f)
        for (x = 0.0f; x < 5.0f; x += 1.0f)
        {
            ctx->shadow_offset_x = (x - 2.0f) * 4.0f;
            ctx->shadow_offset_y = (y - 2.0f) * 4.0f;
            ci_canvas_fill_rectangle(ctx, (x + 0.25f) * 0.2f * width,
                                     (y + 0.25f) * 0.2f * height,
                                     0.1f * width, 0.1f * height);
        }
}

static void shadow_offset_offscreen(ci_canvas_t *ctx, float width, float height)
{
    ctx->shadow_offset_x = width;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_fill_rectangle(ctx, width * -0.6875f, height * 0.0625f,
                             width * 0.375f, height * 0.375f);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, width * 0.5f, height * 0.75f, CI_MINF(width, height) * 0.2f,
                  0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
}

static void shadow_blur(ci_canvas_t *ctx, float width, float height)
{
    float x, y;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ctx->shadow_offset_x = 5.0f;
    ctx->shadow_offset_y = 5.0f;
    for (x = 0.0f; x < 5.0f; x += 1.0f)
        for (y = 4.0f; y >= 0.0f; y -= 1.0f)
        {
            ci_canvas_set_shadow_blur(ctx, (y * 5.0f + x) * 0.5f - 0.5f);
            ci_canvas_fill_rectangle(ctx, (x + 0.25f) * 0.2f * width,
                                     (y + 0.25f) * 0.2f * height,
                                     0.1f * width, 0.1f * height);
        }
}

static void shadow_blur_offscreen(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_shadow_blur(ctx, 5.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, height * 2.0f, width, height);
    ci_canvas_fill_rectangle(ctx, 0.0f, height * -2.0f, width, height);
    ci_canvas_fill_rectangle(ctx, width + 1.0f, 0.0f, width, height);
    ci_canvas_fill_rectangle(ctx, -width - 1.0f, 0.0f, width, height);
}

static void shadow_blur_composite(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.5f;
    float dashing[2];
    ci_canvas_arc(ctx, 0.5f * width, 0.5f * height, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_clip(ctx);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.4f * width, 0.0f, 0.2f * width, height);
    ctx->global_composite_operation = CI_DESTINATION_ATOP;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    dashing[0] = 16.0f; dashing[1] = 4.0f;
    ci_canvas_set_line_dash(ctx, dashing, 2);
    ci_canvas_set_line_width(ctx, 15.0f);
    ctx->shadow_offset_x = 5.0f;
    ctx->shadow_offset_y = 5.0f;
    ci_canvas_set_shadow_blur(ctx, 6.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, 0.45f * width, 0.85f * height, radius * 0.5f, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
    ctx->global_composite_operation = CI_SOURCE_OVER;
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, 0.75f * width, 0.25f * height, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
}

static void test_line_width(ci_canvas_t *ctx, float width, float height)
{
    float step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 4.0f);
    for (step = 0.0f; step < 16.0f; step += 1.0f)
    {
        float left = (step + 0.25f) / 16.0f * width;
        float right = (step + 0.75f) / 16.0f * width;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, left, 0.0f);
        ci_canvas_bezier_curve_to(ctx, left, 0.5f * height,
                                  right, 0.5f * height,
                                  right, height);
        ci_canvas_set_line_width(ctx, 0.5f * (step - 1));
        ci_canvas_stroke(ctx);
    }
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ctx->global_composite_operation = CI_SOURCE_ATOP;
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.5f * height, width, 0.5f * height);
    ctx->global_composite_operation = CI_DESTINATION_OVER;
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.25f * height, width, 0.25f * height);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.5f * height, width, 0.25f * height);
}

static void line_width_angular(ci_canvas_t *ctx, float width, float height)
{
    float step;
    for (step = 0.0f; step < 5.0f; step += 1.0f)
    {
        float grey = (step + 1.0f) / 5.0f;
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, grey, grey, grey, 1.0f);
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, 0.1f * width, 0.1f * height);
        ci_canvas_bezier_curve_to(ctx, 1.2f * width, 1.0f * height,
                                  1.2f * width, -0.0f * height,
                                  0.1f * width, 0.9f * height);
        ci_canvas_set_line_width(ctx, 30.0f - 7.0f * step);
        ci_canvas_stroke(ctx);
    }
}

static void test_line_cap(ci_canvas_t *ctx, float width, float height)
{
    ci_cap_style caps[3];
    int index;
    caps[0] = CI_CAP_BUTT; caps[1] = CI_CAP_SQUARE; caps[2] = CI_CAP_CIRCLE;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 24.0f);
    for (index = 0; index < 3; ++index)
    {
        float right = (float)(index + 1) / 3.0f * width - 20.0f;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, right, 0.125f * height);
        ci_canvas_bezier_curve_to(ctx, right, 0.125f * height + 100.0f,
                                  right - 100.0f, 0.875f * height,
                                  right, 0.875f * height);
        ctx->line_cap = caps[index];
        ci_canvas_stroke(ctx);
    }
}

static void line_cap_offscreen(ci_canvas_t *ctx, float width, float height)
{
    ci_cap_style caps[3];
    int index;
    caps[0] = CI_CAP_BUTT; caps[1] = CI_CAP_SQUARE; caps[2] = CI_CAP_CIRCLE;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 36.0f);
    for (index = 0; index < 3; ++index)
    {
        float x = ((float)(index) + 0.5f) / 3.0f * width;
        float y = ((float)(index) + 0.5f) / 3.0f * height;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, x, -19.0f);
        ci_canvas_line_to(ctx, x, -9.0f);
        ci_canvas_move_to(ctx, x, height + 17.0f);
        ci_canvas_line_to(ctx, x, height + 27.0f);
        ci_canvas_move_to(ctx, -27.0f, y);
        ci_canvas_line_to(ctx, -17.0f, y);
        ci_canvas_move_to(ctx, width + 9.0f, y);
        ci_canvas_line_to(ctx, width + 19.0f, y);
        ctx->line_cap = caps[index];
        ci_canvas_stroke(ctx);
    }
}

static void test_line_join(ci_canvas_t *ctx, float width, float height)
{
    ci_join_style joins[3];
    int index;
    joins[0] = CI_JOIN_MITER; joins[1] = CI_JOIN_BEVEL; joins[2] = CI_JOIN_ROUNDED;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 16.0f);
    for (index = 0; index < 3; ++index)
    {
        float left = ((float)(index) + 0.25f) / 3.0f * width;
        float right = ((float)(index) + 0.75f) / 3.0f * width;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, left, 0.2f * height);
        ci_canvas_line_to(ctx, left, 0.1f * height);
        ci_canvas_line_to(ctx, left, 0.2f * height);
        ci_canvas_line_to(ctx, right, 0.2f * height);
        ci_canvas_line_to(ctx, left, 0.2f * height);
        ci_canvas_line_to(ctx, left, 0.3f * height);
        ci_canvas_line_to(ctx, right, 0.3f * height);
        ci_canvas_line_to(ctx, right, 0.4f * height);
        ci_canvas_line_to(ctx, right, 0.5f * height);
        ci_canvas_line_to(ctx, left, 0.4f * height);
        ci_canvas_line_to(ctx, left, 0.5f * height);
        ci_canvas_line_to(ctx, right, 0.6f * height);
        ci_canvas_bezier_curve_to(ctx, right, height,
                                  left, 0.4f * height,
                                  left, 0.7f * height);
        ci_canvas_bezier_curve_to(ctx, left, 0.8f * height,
                                  right, 0.8f * height,
                                  right, 0.9f * height);
        ctx->line_join = joins[index];
        ci_canvas_stroke(ctx);
    }
}

static void line_join_offscreen(ci_canvas_t *ctx, float width, float height)
{
    ci_join_style joins[3];
    int index;
    joins[0] = CI_JOIN_MITER; joins[1] = CI_JOIN_BEVEL; joins[2] = CI_JOIN_ROUNDED;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 36.0f);
    for (index = 0; index < 3; ++index)
    {
        float x = ((float)(index) + 0.5f) / 3.0f * width;
        float y = ((float)(index) + 0.5f) / 3.0f * height;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, x - 10.0f, -55.0f);
        ci_canvas_line_to(ctx, x - 10.0f, -5.0f);
        ci_canvas_line_to(ctx, x + 10.0f, -55.0f);
        ci_canvas_move_to(ctx, x - 10.0f, height + 130.0f);
        ci_canvas_line_to(ctx, x + 10.0f, height + 80.0f);
        ci_canvas_line_to(ctx, x + 10.0f, height + 130.0f);
        ci_canvas_move_to(ctx, -130.0f, y - 10.0f);
        ci_canvas_line_to(ctx, -80.0f, y - 10.0f);
        ci_canvas_line_to(ctx, -130.0f, y + 10.0f);
        ci_canvas_move_to(ctx, height + 55.0f, y - 10.0f);
        ci_canvas_line_to(ctx, height + 5.0f, y + 10.0f);
        ci_canvas_line_to(ctx, height + 55.0f, y + 10.0f);
        ctx->line_join = joins[index];
        ci_canvas_stroke(ctx);
    }
}

static void test_miter_limit(ci_canvas_t *ctx, float width, float height)
{
    float line, limit;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (line = 0.0f; line < 4.0f; line += 1.0f)
    {
        ci_canvas_set_line_width(ctx, 1.5f * line + 1.0f);
        ci_canvas_set_miter_limit(ctx, 20.0f);
        for (limit = 0.0f; limit < 8.0f; limit += 1.0f)
        {
            float left = (limit + 0.2f) / 8.0f * width;
            float middle = (limit + 0.5f) / 8.0f * width;
            float right = (limit + 0.7f) / 8.0f * width;
            float top = (line + 0.3f) / 4.0f * height;
            float bottom = (line + 0.7f) / 4.0f * height;
            ci_canvas_begin_path(ctx);
            ci_canvas_move_to(ctx, left, bottom);
            ci_canvas_line_to(ctx, left, top);
            ci_canvas_line_to(ctx, right, bottom);
            ci_canvas_line_to(ctx, middle, top);
            ci_canvas_set_miter_limit(ctx, 1.5f * limit);
            ci_canvas_stroke(ctx);
        }
    }
}

static void line_dash_offset(ci_canvas_t *ctx, float width, float height)
{
    float step;
    float segments[4];
    segments[0] = 20.0f; segments[1] = 8.0f;
    segments[2] = 8.0f; segments[3] = 8.0f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_set_line_dash(ctx, segments, 4);
    for (step = 0.0f; step < 16.0f; step += 1.0f)
    {
        float left = (step + 0.125f) / 16.0f * width;
        float right = (step + 0.875f) / 16.0f * width;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, left, 0.0f);
        ci_canvas_line_to(ctx, right, 0.125f * height);
        ci_canvas_line_to(ctx, left, 0.375f * height);
        ci_canvas_line_to(ctx, right, 0.625f * height);
        ci_canvas_line_to(ctx, left, 0.875f * height);
        ci_canvas_line_to(ctx, right, height);
        ctx->line_dash_offset = (step / 16.0f - 0.5f) * 44.0f;
        ci_canvas_stroke(ctx);
    }
}

static void test_line_dash(ci_canvas_t *ctx, float width, float height)
{
    float segments_1[1]; float segments_2[2]; float segments_3[4]; float segments_4[4];
    segments_1[0] = 10.0f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_set_line_dash(ctx, segments_1, 1);
    ci_canvas_stroke(ctx);
    ci_canvas_move_to(ctx, 0.0f, 0.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width * 0.25f, 0.0f);
    ci_canvas_line_to(ctx, width * 0.25f, height);
    ci_canvas_stroke(ctx);
    segments_2[0] = 20.0f; segments_2[1] = -8.0f;
    ci_canvas_set_line_dash(ctx, segments_2, 2);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width * 0.375f, 0.0f);
    ci_canvas_line_to(ctx, width * 0.375f, height);
    ci_canvas_stroke(ctx);
    segments_3[0] = 20.0f; segments_3[1] = 8.0f; segments_3[2] = 8.0f; segments_3[3] = 8.0f;
    ci_canvas_set_line_dash(ctx, segments_3, 4);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width * 0.5f, 0.0f);
    ci_canvas_line_to(ctx, width * 0.5f, height);
    ci_canvas_stroke(ctx);
    segments_4[0] = 0.0f; segments_4[1] = 8.0f; segments_4[2] = 2.0f; segments_4[3] = 8.0f;
    ci_canvas_set_line_dash(ctx, segments_4, 4);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width * 0.625f, 0.0f);
    ci_canvas_line_to(ctx, width * 0.625f, height);
    ci_canvas_stroke(ctx);
    ci_canvas_set_line_dash(ctx, 0, 0);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width * 0.75f, 0.0f);
    ci_canvas_line_to(ctx, width * 0.75f, height);
    ci_canvas_stroke(ctx);
}

static void line_dash_closed(ci_canvas_t *ctx, float width, float height)
{
    float segments_1[2]; float segments_2[4];
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 32.0f);
    segments_1[0] = 96.0f; segments_1[1] = 32.0f;
    ci_canvas_set_line_dash(ctx, segments_1, 2);
    ctx->line_dash_offset = -80.0f;
    ci_canvas_stroke_rectangle(ctx, 0.25f * width, 0.25f * height,
                               0.5f * width, 0.5f * height);
    segments_2[0] = 96.0f; segments_2[1] = 32.0f; segments_2[2] = 1024.0f; segments_2[3] = 16.0f;
    ci_canvas_set_line_dash(ctx, segments_2, 4);
    ctx->line_dash_offset = 128.0f;
    ci_canvas_stroke_rectangle(ctx, 0.09375f * width, 0.09375f * height,
                               0.8125f * width, 0.8125f * height);
}

static void line_dash_overlap(ci_canvas_t *ctx, float width, float height)
{
    float segments[2];
    int index;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_set_line_width(ctx, 16.0f);
    segments[0] = 14.0f; segments[1] = 12.0f;
    ci_canvas_set_line_dash(ctx, segments, 2);
    for (index = 0; index < 4; ++index)
    {
        float flip = (index == 3 ? -1.0f : 1.0f);
        float top_y = (index & 1 ? 0.25f : 0.1f) * height;
        float bottom_y = (index & 1 ? 0.9f : 0.75f) * height;
        float mid_x = (index & 2 ? 0.75f : 0.25f) * width;
        float top_width = (index & 1 ? 0.25f : 0.55f) * flip * width;
        float bottom_width = (index & 1 ? 0.55f : 0.25f) * flip * width;
        ci_canvas_move_to(ctx, mid_x, top_y);
        ci_canvas_bezier_curve_to(ctx, mid_x - top_width, top_y,
                                  mid_x + bottom_width, bottom_y,
                                  mid_x, bottom_y);
        ci_canvas_bezier_curve_to(ctx, mid_x - bottom_width, bottom_y,
                                  mid_x + top_width, top_y,
                                  mid_x, top_y);
        ci_canvas_close_path(ctx);
    }
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void line_dash_offscreen(ci_canvas_t *ctx, float width, float height)
{
    float step;
    float segments[4];
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 6.0f);
    segments[0] = 0.0f;
    segments[1] = width * 20.5f * 3.14159265f - height * 0.5f + 1.0f;
    segments[2] = height - 2.0f;
    segments[3] = 0.0f;
    ci_canvas_set_line_dash(ctx, segments, 4);
    for (step = -2.0f; step <= 2.0f; step += 1.0f)
    {
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, width * -20.0f, height * 0.5f,
                      width * (20.5f - step * 0.1f),
                      3.14159265f, 1.5707963268f, 0);
        ctx->line_dash_offset = width * step * 0.1f * 3.14159265f;
        ci_canvas_stroke(ctx);
    }
}

static void test_color(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.4f;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 2.0f, -1.0f, 0.0f, 0.5f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 1.0f, 1.5f);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_arc(ctx, 0.5f * width, 0.5f * height, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_linear_gradient(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.4f;
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE,
                                  0.3f * width, 0.3f * height,
                                  0.7f * width, 0.7f * height);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 0.0f, 1.0f, 100.0f);
    ci_canvas_set_linear_gradient(ctx, CI_STROKE_STYLE,
                                  0.3f * width, 0.7f * height,
                                  0.7f * width, 0.3f * height);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_arc(ctx, 0.5f * width, 0.5f * height, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    ci_canvas_set_linear_gradient(ctx, CI_STROKE_STYLE,
                                  0.5f * width, 0.5f * height,
                                  0.5f * width, 0.5f * height);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_stroke(ctx);
}

static void test_radial_gradient(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.4f;
    ci_canvas_set_radial_gradient(ctx, CI_FILL_STYLE,
                                  0.0f, 0.0f, radius,
                                  width, height, 0.5f * radius);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_radial_gradient(ctx, CI_STROKE_STYLE,
                                  0.0f, height, radius,
                                  width, 0.0f, 0.5f * radius);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_arc(ctx, 0.5f * width, 0.5f * height, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    ci_canvas_set_radial_gradient(ctx, CI_STROKE_STYLE,
                                  0.5f * width, 0.4f * height, 10.0f,
                                  0.5f * width, 0.6f * height, 0.0f);
    ci_canvas_set_radial_gradient(ctx, CI_STROKE_STYLE,
                                  0.0f, 0.5f * height, -10.0f,
                                  width, 0.5f * height, 10.0f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_radial_gradient(ctx, CI_FILL_STYLE,
                                  0.5f * width, 0.5f * height, 0.0f,
                                  0.5f * width, 0.5f * height, radius);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.20f, 0.0f, 0.0f, 0.0f, 0.0f);
    ci_canvas_fill(ctx);
}

static void test_conic_gradient(ci_canvas_t *ctx, float width, float height)
{
    float cx = 0.5f * width;
    float cy = 0.5f * height;
    float radius = CI_MINF(width, height) * 0.4f;
    /* basic conic gradient: color wheel */
    ci_canvas_set_conic_gradient(ctx, CI_FILL_STYLE, 0.0f, cx, cy);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.333f, 0.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.667f, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_arc(ctx, cx, cy, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    /* conic gradient on stroke with offset start angle */
    ci_canvas_set_conic_gradient(ctx, CI_STROKE_STYLE,
        1.57079633f, cx, cy);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE,
        1.0f, 0.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_stroke(ctx);
}

static void conic_gradient_rect(ci_canvas_t *ctx, float width, float height)
{
    /* conic gradient filling a full rectangle */
    ci_canvas_set_conic_gradient(ctx, CI_FILL_STYLE,
        0.0f, 0.5f * width, 0.5f * height);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.5f, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, height);
    /* offset start angle half-circle */
    ci_canvas_set_conic_gradient(ctx, CI_FILL_STYLE,
        3.14159265f, 0.5f * width, 0.5f * height);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.0f, 1.0f, 0.0f, 0.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        0.5f, 0.0f, 0.0f, 1.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE,
        1.0f, 1.0f, 0.0f, 0.0f, 0.8f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, 0.5f * height);
}

static void test_color_stop(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE,
                                  0.1f * width, 0.0f,
                                  0.9f * width, 0.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, 0.1f * height);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 2.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.3f, -1.0f, 0.0f, 2.0f, 2.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.3f, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 0.8f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.7f, 0.9f, 0.9f, 0.9f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.6f, 0.1f, 0.1f, 0.1f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.1f * height, width, 0.4f * height);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.5f * height, width, 0.4f * height);
}

static void test_pattern(ci_canvas_t *ctx, float width, float height)
{
    unsigned char checker[256];
    int index;
    float scale;
    for (index = 0; index < 256; ++index)
        checker[index] = (unsigned char)(
            (((index >> 2 & 1) ^ (index >> 5 & 1)) |
             ((index & 3) == 3)) * 255);
    ci_canvas_arc(ctx, 0.5f * width, 0.5f * height, 32.0f, 0.0f, 6.28318531f, 0);
    ci_canvas_close_path(ctx);
    ci_canvas_set_line_width(ctx, 20.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_pattern(ctx, CI_STROKE_STYLE, 0, 8, 8, 32, CI_REPEAT);
    ci_canvas_stroke(ctx);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_set_pattern(ctx, CI_STROKE_STYLE, checker, 8, 8, 32, CI_REPEAT);
    ci_canvas_stroke(ctx);
    for (scale = 8.0f; scale >= 1.0f; scale /= 2.0f)
    {
        float size_x, size_y;
        ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        ci_canvas_scale(ctx, scale, scale);
        size_x = 0.5f * width / scale;
        size_y = 0.5f * height / scale;
        ci_canvas_set_pattern(ctx, CI_FILL_STYLE, checker, 8, 8, 32, CI_NO_REPEAT);
        ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, size_x, size_y);
        ci_canvas_set_pattern(ctx, CI_FILL_STYLE, checker, 8, 8, 32, CI_REPEAT_X);
        ci_canvas_fill_rectangle(ctx, size_x, 0.0f, size_x, size_y);
        ci_canvas_set_pattern(ctx, CI_FILL_STYLE, checker, 8, 8, 32, CI_REPEAT_Y);
        ci_canvas_fill_rectangle(ctx, 0.0f, size_y, size_x, size_y);
        ci_canvas_set_pattern(ctx, CI_FILL_STYLE, checker, 8, 8, 32, CI_REPEAT);
        ci_canvas_fill_rectangle(ctx, size_x, size_y, size_x, size_y);
    }
}

static void test_begin_path(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_move_to(ctx, 0.0f, 0.0f);
    ci_canvas_line_to(ctx, width, height);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, width, 0.0f);
    ci_canvas_line_to(ctx, 0.0f, height);
    ci_canvas_stroke(ctx);
    ci_canvas_begin_path(ctx);
    ci_canvas_line_to(ctx, 0.5f * width, height);
    ci_canvas_stroke(ctx);
}

static void test_move_to(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_move_to(ctx, 0.6f * width, height);
    ci_canvas_move_to(ctx, 0.4f * width, 0.1f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.5f * height);
    ci_canvas_line_to(ctx, 0.4f * width, 0.9f * height);
    ci_canvas_move_to(ctx, 0.6f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.4f * height);
    ci_canvas_move_to(ctx, 0.8f * width, 0.6f * height);
    ci_canvas_line_to(ctx, 0.6f * width, 0.8f * height);
    ci_canvas_move_to(ctx, 0.7f * width, 0.5f * height);
    ci_canvas_line_to(ctx, 0.7f * width, 0.5f * height);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_close_path(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_line_to(ctx, 0.5f * width, 0.5f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.8f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.2f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_line_to(ctx, 0.5f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.2f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 0.5f * width, 0.8f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.8f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.5f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_line_to(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 16.0f);
    ci_canvas_line_to(ctx, 0.1f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.1f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.5f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.5f * height);
    ci_canvas_line_to(ctx, 0.3f * width, 0.8f * height);
    ci_canvas_line_to(ctx, 0.4f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.4f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.6f * width, 0.8f * height);
    ci_canvas_line_to(ctx, 0.6f * width, 0.8f * height);
    ci_canvas_move_to(ctx, 0.7f * width, 0.4f * height);
    ci_canvas_line_to(ctx, 0.9f * width, 0.4f * height);
    ci_canvas_line_to(ctx, 0.9f * width, 0.6f * height);
    ci_canvas_line_to(ctx, 0.7f * width, 0.6f * height);
    ci_canvas_line_to(ctx, 0.7f * width, 0.4f * height);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_quadratic_curve_to(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_quadratic_curve_to(ctx, 0.1f * width, 0.2f * height,
                                 0.1f * width, 0.2f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.2f * width, 0.5f * height,
                                 0.2f * width, 0.5f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.3f * width, 0.8f * height,
                                 0.4f * width, 0.2f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.6f * width, 0.8f * height,
                                 0.7f * width, 0.2f * height);
    ci_canvas_move_to(ctx, 0.7f * width, 0.6f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.9f * width, 0.6f * height,
                                 0.9f * width, 0.8f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.9f * width, 0.9f * height,
                                 0.7f * width, 0.9f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 0.1f * width, 0.9f * height);
    ci_canvas_quadratic_curve_to(ctx, 0.5f * width, 0.5f * height,
                                 0.1f * width, 0.9f * height);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_bezier_curve_to(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_bezier_curve_to(ctx, 0.9f * width, 0.9f * height,
                              0.6f * width, 0.6f * height,
                              0.6f * width, 0.9f * height);
    ci_canvas_move_to(ctx, 0.1f * width, 0.1f * height);
    ci_canvas_bezier_curve_to(ctx, 0.9f * width, 0.9f * height,
                              0.9f * width, 0.1f * height,
                              0.1f * width, 0.9f * height);
    ci_canvas_move_to(ctx, 0.4f * width, 0.1f * height);
    ci_canvas_bezier_curve_to(ctx, 0.1f * width, 0.3f * height,
                              0.7f * width, 0.3f * height,
                              0.4f * width, 0.1f * height);
    ci_canvas_move_to(ctx, 0.9f * width, 0.1f * height);
    ci_canvas_bezier_curve_to(ctx, 0.6f * width, 0.2f * height,
                              0.9f * width, 0.1f * height,
                              0.6f * width, 0.2f * height);
    ci_canvas_move_to(ctx, 0.7f * width, 0.3f * height);
    ci_canvas_bezier_curve_to(ctx, 0.9f * width, 0.3f * height,
                              0.9f * width, 0.4f * height,
                              0.8f * width, 0.5f * height);
    ci_canvas_bezier_curve_to(ctx, 0.7f * width, 0.6f * height,
                              0.7f * width, 0.7f * height,
                              0.9f * width, 0.7f * height);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_arc_to(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.5f;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_arc_to(ctx, 0.3f * width, 0.3f * height,
                     0.5f * width, 0.5f * height, 16.0f);
    ci_canvas_move_to(ctx, 0.4f * width, 0.4f * height);
    ci_canvas_arc_to(ctx, 0.7f * width, 0.1f * height,
                     0.7f * width, 0.4f * height, 0.0f);
    ci_canvas_arc_to(ctx, 0.9f * width, 0.5f * height,
                     0.7f * width, 0.7f * height, 0.125f * radius);
    ci_canvas_arc_to(ctx, 0.5f * width, 0.9f * height,
                     0.3f * width, 0.8f * height, 0.25f * radius);
    ci_canvas_arc_to(ctx, 0.1f * width, 0.7f * height,
                     0.4f * width, 0.4f * height, 0.375f * radius);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 0.1f * width, 0.6f * height);
    ci_canvas_transform(ctx, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.2f * height);
    ci_canvas_arc_to(ctx, 0.1f * width, 0.9f * height,
                     0.5f * width, 0.9f * height, 0.3f * radius);
    ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 0.2f * width, 0.1f * height);
    ci_canvas_arc_to(ctx, 0.1f * width, 0.1f * height,
                     0.1f * width, 0.7f * height, 0.6f * radius);
    ci_canvas_arc_to(ctx, 0.2f * width, 0.4f * height,
                     0.2f * width, 0.4f * height, 0.5f * radius);
    ci_canvas_arc_to(ctx, 0.4f * width, 0.2f * height,
                     0.2f * width, 0.4f * height, 0.5f * radius);
    ci_canvas_arc_to(ctx, 0.5f * width, 0.5f * height,
                     0.9f * width, 0.1f * height, -1.0f);
    ci_canvas_move_to(ctx, 0.6f * width, 0.9f * height);
    ci_canvas_set_transform(ctx, 0.0f, 0.0f, 0.0f, 1.0f, 0.9f * width, 0.0f);
    ci_canvas_arc_to(ctx, 0.9f * width, 0.9f * height,
                     0.9f * width, 0.6f * height, 0.3f * radius);
    ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
    ci_canvas_arc_to(ctx, 0.9f * width, 0.6f * height,
                     0.9f * width, 0.6f * height, 0.0f);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_arc(ci_canvas_t *ctx, float width, float height)
{
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 3; ++j)
        {
            float x = ((float)(j) + 0.5f) * width / 3.0f;
            float y = ((float)(i) + 0.5f) * height / 4.0f;
            float radius = CI_MINF(width, height) * 0.1f;
            float start = (3.14159265f + 1.0e-6f) * (float)(i % 2);
            float end = (3.14159265f + 1.0e-6f) * (1.0f + 0.5f * (float)(j));
            int counter = i / 2;
            ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
            ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
            ci_canvas_set_line_width(ctx, 8.0f);
            ci_canvas_begin_path(ctx);
            ci_canvas_arc(ctx, x, y, -radius, start, end, counter);
            ci_canvas_arc(ctx, x, y, radius, start, end, counter);
            ci_canvas_close_path(ctx);
            ci_canvas_fill(ctx);
            ci_canvas_stroke(ctx);
        }
}

static void test_ellipse(ci_canvas_t *ctx, float width, float height)
{
    int i, j;
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 3; ++j)
        {
            float x = ((float)(j) + 0.5f) * width / 3.0f;
            float y = ((float)(i) + 0.5f) * height / 4.0f;
            float rx = CI_MINF(width, height) * 0.12f;
            float ry = rx * 0.5f;
            float start = (3.14159265f + 1.0e-6f) * (float)(i % 2);
            float end = (3.14159265f + 1.0e-6f) * (1.0f + 0.5f * (float)(j));
            int counter = i / 2;
            ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
            ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
            ci_canvas_set_line_width(ctx, 4.0f);
            ci_canvas_begin_path(ctx);
            /* negative radius must be a no-op */
            ci_canvas_ellipse(ctx, x, y, -rx, ry, 0.0f,
                start, end, counter);
            ci_canvas_ellipse(ctx, x, y, rx, ry, 0.0f,
                start, end, counter);
            ci_canvas_close_path(ctx);
            ci_canvas_fill(ctx);
            ci_canvas_stroke(ctx);
        }
}

static void test_ellipse_rotated(ci_canvas_t *ctx, float width, float height)
{
    float cx = width * 0.5f;
    float cy = height * 0.5f;
    float rx = width * 0.35f;
    float ry = height * 0.15f;
    float pi = 3.14159265f;
    float tau = 6.28318531f;
    int i;
    ci_canvas_set_line_width(ctx, 3.0f);
    /* draw three rotated filled ellipses at 0, 60, 120 degrees */
    for (i = 0; i < 3; ++i) {
        float rot = (float)i * pi / 3.0f;
        float r = (i == 0) ? 1.0f : 0.0f;
        float g = (i == 1) ? 1.0f : 0.0f;
        float b = (i == 2) ? 1.0f : 0.0f;
        ci_canvas_set_color(ctx, CI_FILL_STYLE, r, g, b, 0.4f);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, r, g, b, 1.0f);
        ci_canvas_begin_path(ctx);
        ci_canvas_ellipse(ctx, cx, cy, rx, ry, rot,
            0.0f, tau, 0);
        ci_canvas_close_path(ctx);
        ci_canvas_fill(ctx);
        ci_canvas_stroke(ctx);
    }
}

static void test_rectangle(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_move_to(ctx, 0.3f * width, 0.3f * height);
    ci_canvas_line_to(ctx, 0.7f * width, 0.3f * height);
    ci_canvas_line_to(ctx, 0.7f * width, 0.7f * height);
    ci_canvas_line_to(ctx, 0.3f * width, 0.7f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 0.0f, 0.0f);
    for (y = -1.0f; y <= 1.0f; y += 1.0f)
        for (x = -1.0f; x <= 1.0f; x += 1.0f)
            ci_canvas_rectangle(ctx, (0.5f + 0.1f * x) * width,
                                (0.5f + 0.1f * y) * height,
                                x * 0.3f * width, y * 0.3f * height);
    ci_canvas_line_to(ctx, width, height);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_round_rect(ci_canvas_t *ctx, float width, float height)
{
    float r_uniform = 20.0f;
    float radii4[4];
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 3.0f);
    /* Uniform radius */
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.2f, 0.4f, 0.8f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_round_rectangle(ctx, 15.0f, 15.0f,
        width - 30.0f, height * 0.4f - 10.0f, &r_uniform, 1);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    /* Four different corner radii */
    radii4[0] = 5.0f;
    radii4[1] = 15.0f;
    radii4[2] = 30.0f;
    radii4[3] = 40.0f;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.8f, 0.2f, 0.2f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_round_rectangle(ctx, 15.0f, height * 0.5f + 5.0f,
        width - 30.0f, height * 0.4f - 10.0f, radii4, 4);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_round_rect_clamped(ci_canvas_t *ctx,
    float width, float height)
{
    float r_large = 200.0f;
    float r_zero = 0.0f;
    float radii_mixed[4];
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 2.0f);
    /* Large radius: creates pill/capsule shape */
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.2f, 0.8f, 0.2f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_round_rectangle(ctx, 20.0f, 15.0f,
        width - 40.0f, 60.0f, &r_large, 1);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    /* Zero radius: sharp corners like regular rect */
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.4f, 0.8f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_round_rectangle(ctx, 20.0f, height * 0.33f + 10.0f,
        width - 40.0f, 60.0f, &r_zero, 1);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    /* Mixed large radii on small rect: all get scaled down */
    radii_mixed[0] = 100.0f;
    radii_mixed[1] = 50.0f;
    radii_mixed[2] = 30.0f;
    radii_mixed[3] = 10.0f;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.8f, 0.8f, 0.2f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_round_rectangle(ctx, 30.0f, height * 0.66f + 5.0f,
        width - 60.0f, 65.0f, radii_mixed, 4);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
}

static void test_fill(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.45f;
    float step, angle, x, y;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill(ctx);
    ci_canvas_begin_path(ctx);
    for (step = 0.0f; step < 128.0f; step += 1.0f)
    {
        angle = step * (59.0f / 128.0f * 6.28318531f);
        x = (float)cos((double)angle) * radius + width / 2.0f;
        y = (float)sin((double)angle) * radius + height / 2.0f;
        ci_canvas_line_to(ctx, x, y);
    }
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_scale(ctx, 0.0f, 1.0f);
    ci_canvas_fill(ctx);
}

static void fill_rounding(ci_canvas_t *ctx, float width, float height)
{
    (void)width;
    (void)height;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 4.00000191f, 4.00000763f);
    ci_canvas_line_to(ctx, 3.99999809f, 192.0f);
    ci_canvas_line_to(ctx, 28.0000019f, 192.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, -10390.0664f, 52.3311195f);
    ci_canvas_line_to(ctx, -10389.9941f, 47.6248589f);
    ci_canvas_line_to(ctx, -10395.9941f, 47.5328255f);
    ci_canvas_line_to(ctx, -10396.0664f, 52.2478294f);
    ci_canvas_close_path(ctx);
    ci_canvas_move_to(ctx, 110.0f, 256.0f);
    ci_canvas_line_to(ctx, 124.086205f, 255.998276f);
    ci_canvas_line_to(ctx, 123.203453f, 0.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
}

static void fill_converging(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.48f;
    float step, angle_1, angle_2;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0.0f; step < 256.0f; step += 1.0f)
    {
        angle_1 = (step + 0.0f) / 256.0f * 6.28318531f;
        angle_2 = (step + 0.5f) / 256.0f * 6.28318531f;
        ci_canvas_move_to(ctx, width / 2.0f + 0.5f, height / 2.0f + 0.5f);
        ci_canvas_line_to(ctx, (float)cos((double)angle_1) * radius + width  / 2.0f + 0.5f,
                          (float)sin((double)angle_1) * radius + height / 2.0f + 0.5f);
        ci_canvas_line_to(ctx, (float)cos((double)angle_2) * radius + width  / 2.0f + 0.5f,
                          (float)sin((double)angle_2) * radius + height / 2.0f + 0.5f);
        ci_canvas_close_path(ctx);
    }
    ci_canvas_fill(ctx);
}

static void fill_zone_plate(ci_canvas_t *ctx, float width, float height)
{
    float radius = (float)floor((double)(CI_MINF(width, height) * 0.48f / 4.0f)) * 4.0f;
    float step, inner, outer;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0.0f; step < radius; step += 2.0f)
    {
        inner = (float)sqrt((double)((step + 0.0f) / radius)) * radius;
        outer = (float)sqrt((double)((step + 1.0f) / radius)) * radius;
        ci_canvas_move_to(ctx, width / 2.0f + inner, height / 2.0f);
        ci_canvas_arc(ctx, width / 2.0f, height / 2.0f, inner,
                      0.0f, 6.28318531f, 0);
        ci_canvas_close_path(ctx);
        ci_canvas_move_to(ctx, width / 2.0f + outer, height / 2.0f);
        ci_canvas_arc(ctx, width / 2.0f, height / 2.0f, outer,
                      6.28318531f, 0.0f, 1);
        ci_canvas_close_path(ctx);
    }
    ci_canvas_fill(ctx);
}

static void test_stroke(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.45f;
    float step, angle, x, y;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_begin_path(ctx);
    for (step = 0.0f; step < 128.0f; step += 1.0f)
    {
        angle = step * (59.0f / 128.0f * 6.28318531f);
        x = (float)cos((double)angle) * radius + width / 2.0f;
        y = (float)sin((double)angle) * radius + height / 2.0f;
        ci_canvas_line_to(ctx, x, y);
    }
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_scale(ctx, 0.0f, 1.0f);
    ci_canvas_stroke(ctx);
}

static void stroke_wide(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ctx->line_join = CI_JOIN_ROUNDED;
    ci_canvas_move_to(ctx, 24.0f, 104.0f);
    ci_canvas_bezier_curve_to(ctx, 112.0f, 24.0f, 16.0f, 24.0f, 104.0f, 104.0f);
    ci_canvas_move_to(ctx, 152.0f, 104.0f);
    ci_canvas_bezier_curve_to(ctx, 232.8f, 24.0f, 151.2f, 24.0f, 232.0f, 104.0f);
    ci_canvas_move_to(ctx, 24.0f, 232.0f);
    ci_canvas_bezier_curve_to(ctx, 104.0f, 152.0f, 24.0f, 152.0f, 104.0f, 232.0f);
    ci_canvas_move_to(ctx, 188.0f, 232.0f);
    ci_canvas_bezier_curve_to(ctx, 196.0f, 184.0f, 188.0f, 184.0f, 196.0f, 192.0f);
    ci_canvas_set_line_width(ctx, 40.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 1.0f);
    ci_canvas_stroke(ctx);
}

static void stroke_inner_join(ci_canvas_t *ctx, float width, float height)
{
    ci_join_style joins[3];
    int index;
    joins[0] = CI_JOIN_MITER; joins[1] = CI_JOIN_BEVEL; joins[2] = CI_JOIN_ROUNDED;
    for (index = 0; index < 3; ++index)
    {
        float center = ((float)(index) + 0.5f) / 3.0f * width;
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx, center - 0.05f * width, 0.275f * height);
        ci_canvas_line_to(ctx, center, 0.225f * height);
        ci_canvas_line_to(ctx, center + 0.025f * width, 0.25f * height);
        ci_canvas_move_to(ctx, center - 0.05f * width, 0.775f * height);
        ci_canvas_bezier_curve_to(ctx, center, 0.725f * height,
                                  center, 0.725f * height,
                                  center + 0.025f * width, 0.75f * height);
        ctx->line_join = joins[index];
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_set_line_width(ctx, 0.3f * width);
        ci_canvas_stroke(ctx);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_set_line_width(ctx, 1.0f);
        ci_canvas_stroke(ctx);
    }
}

static void stroke_spiral(ci_canvas_t *ctx, float width, float height)
{
    float outside, step, parameter, angle, radius;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 2.0f);
    ci_canvas_begin_path(ctx);
    outside = CI_MINF(width, height) * 0.48f;
    for (step = 0.0f; step <= 2048.0f; step += 1.0f)
    {
        parameter = (step - 1024.0f) / 1024.0f;
        angle = (float)fabs((double)parameter) * 12.0f * 6.28318531f;
        radius = parameter * outside;
        ci_canvas_line_to(ctx, (float)cos((double)angle) * radius + width * 0.5f,
                          (float)sin((double)angle) * radius + height * 0.5f);
    }
    ci_canvas_stroke(ctx);
}

static void stroke_long(ci_canvas_t *ctx, float width, float height)
{
    float step;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (step = 0.0f; step <= 29.0f; step += 1.0f)
    {
        ci_canvas_move_to(ctx, 0.4f * width, -23.0f * height);
        ci_canvas_line_to(ctx, width * step / 29.0f, height);
        ci_canvas_move_to(ctx, -23.0f * width, 0.4f * height);
        ci_canvas_line_to(ctx, width, height * step / 29.0f);
    }
    ci_canvas_stroke(ctx);
}

static void test_clip(ci_canvas_t *ctx, float width, float height)
{
    float radius = CI_MINF(width, height) * 0.5f;
    int step;
    ci_canvas_set_line_width(ctx, 8.0f);
    for (step = 0; step < 8; ++step)
    {
        float fraction = (float)(step) / 8.0f;
        float angle = fraction * 6.28318531f;
        ci_canvas_set_color(ctx, CI_STROKE_STYLE,
                            0.0f, (float)(step & 1), 0.0f, 1.0f);
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, 0.5f * width + 0.8f * radius * (float)cos((double)angle),
                      0.5f * height + 0.8f * radius * (float)sin((double)angle),
                      radius, 0.0f, 6.28318531f, 0);
        ci_canvas_close_path(ctx);
        ci_canvas_stroke(ctx);
        ci_canvas_clip(ctx);
    }
    ci_canvas_begin_path(ctx);
    ci_canvas_clip(ctx);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, height);
}

static void clip_winding(ci_canvas_t *ctx, float width, float height)
{
    float step;
    ci_canvas_move_to(ctx, 0.125f * width, 0.125f * height);
    ci_canvas_line_to(ctx, 0.625f * width, 0.125f * height);
    ci_canvas_line_to(ctx, 0.625f * width, 0.625f * height);
    ci_canvas_line_to(ctx, 0.125f * width, 0.625f * height);
    ci_canvas_move_to(ctx, 0.250f * width, 0.250f * height);
    ci_canvas_line_to(ctx, 0.750f * width, 0.250f * height);
    ci_canvas_line_to(ctx, 0.750f * width, 0.750f * height);
    ci_canvas_line_to(ctx, 0.250f * width, 0.750f * height);
    ci_canvas_move_to(ctx, 0.375f * width, 0.375f * height);
    ci_canvas_line_to(ctx, 0.375f * width, 0.875f * height);
    ci_canvas_line_to(ctx, 0.875f * width, 0.875f * height);
    ci_canvas_line_to(ctx, 0.875f * width, 0.375f * height);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill(ctx);
    ci_canvas_clip(ctx);
    ci_canvas_set_line_width(ctx, 4.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_begin_path(ctx);
    for (step = 0.0f; step < 32.0f; step += 1.0f)
    {
        ci_canvas_move_to(ctx, step / 16.0f * width, 0.0f);
        ci_canvas_line_to(ctx, step / 16.0f * width - width, height);
    }
    ci_canvas_stroke(ctx);
}

static void test_is_point_in_path(ci_canvas_t *ctx, float width, float height)
{
    int index;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    if (ci_canvas_is_point_in_path(ctx, 0.0f, 0.0f))
        ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, 16.0f, 16.0f);
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 65.0f, 16.0f);
    ci_canvas_line_to(ctx, 113.0f, 24.0f);
    ci_canvas_bezier_curve_to(ctx, 113.0f, 24.0f, 93.0f, 126.0f, 119.0f, 160.0f);
    ci_canvas_bezier_curve_to(ctx, 133.0f, 180.0f, 170.0f, 196.0f, 186.0f, 177.0f);
    ci_canvas_bezier_curve_to(ctx, 198.0f, 162.0f, 182.0f, 130.0f, 166.0f, 118.0f);
    ci_canvas_bezier_curve_to(ctx, 123.0f, 80.0f, 84.0f, 124.0f, 84.0f, 124.0f);
    ci_canvas_line_to(ctx, 35.0f, 124.0f);
    ci_canvas_line_to(ctx, 18.0f, 56.0f);
    ci_canvas_line_to(ctx, 202.0f, 56.0f);
    ci_canvas_line_to(ctx, 202.0f, 90.0f);
    ci_canvas_bezier_curve_to(ctx, 202.0f, 90.0f, 240.0f, 168.0f, 209.0f, 202.0f);
    ci_canvas_bezier_curve_to(ctx, 175.0f, 240.0f, 65.0f, 187.0f, 65.0f, 187.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_translate(ctx, 40.0f, 160.0f);
    ci_canvas_move_to(ctx, 110.0f, 0.0f);
    ci_canvas_line_to(ctx, 0.0f, 0.0f);
    ci_canvas_line_to(ctx, 0.0f, 0.0f);
    ci_canvas_bezier_curve_to(ctx, 0.0f, 90.0f, 110.0f, 90.0f, 110.0f, 40.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    for (index = 0; index < 256; ++index)
    {
        int bits = index;
        int inside;
        float x, y;
        bits = (bits << 1 & 0xaa) | (bits >> 1 & 0x55);
        bits = (bits << 2 & 0xcc) | (bits >> 2 & 0x33);
        bits = (bits << 4 & 0xf0) | (bits >> 4 & 0x0f);
        x = (float)(bits) / 256.0f * width;
        y = (float)(index) / 256.0f * height;
        ci_canvas_rotate(ctx, 0.5f);
        inside = ci_canvas_is_point_in_path(ctx, x, y);
        ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f - inside, (float)inside, 0.0f, 1.0f);
        ci_canvas_stroke_rectangle(ctx, x - 1.5f, y - 1.5f, 3.0f, 3.0f);
    }
}

static void is_point_in_path_offscreen(ci_canvas_t *ctx, float width, float height)
{
    int index;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 321.0f, -240.0f);
    ci_canvas_line_to(ctx, 369.0f, -232.0f);
    ci_canvas_bezier_curve_to(ctx, 369.0f, -232.0f, 349.0f, -130.0f, 375.0f, -96.0f);
    ci_canvas_bezier_curve_to(ctx, 389.0f, -76.0f, 426.0f, -60.0f, 442.0f, -79.0f);
    ci_canvas_bezier_curve_to(ctx, 454.0f, -94.0f, 438.0f, -126.0f, 422.0f, -138.0f);
    ci_canvas_bezier_curve_to(ctx, 379.0f, -176.0f, 340.0f, -132.0f, 340.0f, -132.0f);
    ci_canvas_line_to(ctx, 291.0f, -132.0f);
    ci_canvas_line_to(ctx, 274.0f, -200.0f);
    ci_canvas_line_to(ctx, 458.0f, -200.0f);
    ci_canvas_line_to(ctx, 458.0f, -166.0f);
    ci_canvas_bezier_curve_to(ctx, 458.0f, -166.0f, 496.0f, -88.0f, 465.0f, -54.0f);
    ci_canvas_bezier_curve_to(ctx, 431.0f, -16.0f, 321.0f, -69.0f, 321.0f, -69.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_translate(ctx, 40.0f, 160.0f);
    ci_canvas_move_to(ctx, 366.0f, -256.0f);
    ci_canvas_line_to(ctx, 256.0f, -256.0f);
    ci_canvas_line_to(ctx, 256.0f, -256.0f);
    ci_canvas_bezier_curve_to(ctx, 256.0f, -166.0f, 366.0f, -166.0f, 366.0f, -216.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_stroke(ctx);
    for (index = 0; index < 256; ++index)
    {
        int bits = index;
        int inside;
        float x, y;
        bits = (bits << 1 & 0xaa) | (bits >> 1 & 0x55);
        bits = (bits << 2 & 0xcc) | (bits >> 2 & 0x33);
        bits = (bits << 4 & 0xf0) | (bits >> 4 & 0x0f);
        x = (float)(bits) / 256.0f * width;
        y = (float)(index) / 256.0f * height;
        ci_canvas_rotate(ctx, 0.5f);
        inside = ci_canvas_is_point_in_path(ctx, x + width, y - height);
        ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f - inside, (float)inside, 0.0f, 1.0f);
        ci_canvas_stroke_rectangle(ctx, x - 1.5f, y - 1.5f, 3.0f, 3.0f);
    }
}

static void fill_evenodd(ci_canvas_t *ctx, float width, float height)
{
    /* Concentric circles, all wound the same direction.
       Nonzero fills solid; evenodd alternates filled/unfilled rings. */
    float cx = width * 0.5f;
    float cy = height * 0.5f;
    float radii[5];
    int i;
    radii[0] = CI_MINF(width, height) * 0.45f;
    radii[1] = CI_MINF(width, height) * 0.36f;
    radii[2] = CI_MINF(width, height) * 0.27f;
    radii[3] = CI_MINF(width, height) * 0.18f;
    radii[4] = CI_MINF(width, height) * 0.09f;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_fill_rule(ctx, CI_FILL_EVENODD);
    ci_canvas_begin_path(ctx);
    for (i = 0; i < 5; ++i) {
        ci_canvas_move_to(ctx, cx + radii[i], cy);
        ci_canvas_arc(ctx, cx, cy, radii[i], 0.0f, 6.28318531f, 0);
        ci_canvas_close_path(ctx);
    }
    ci_canvas_fill(ctx);
}

static void fill_evenodd_star(ci_canvas_t *ctx, float width, float height)
{
    /* Five-pointed star drawn as a single self-intersecting polygon.
       With evenodd, the center pentagon is unfilled. */
    float cx = width * 0.5f;
    float cy = height * 0.5f;
    float r = CI_MINF(width, height) * 0.45f;
    int order[5];
    int i;
    order[0] = 0; order[1] = 2; order[2] = 4; order[3] = 1; order[4] = 3;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_fill_rule(ctx, CI_FILL_EVENODD);
    ci_canvas_begin_path(ctx);
    for (i = 0; i < 5; ++i) {
        float angle = (float)order[i] * 1.25663706f - 1.57079633f;
        float px = cx + r * (float)cos((double)angle);
        float py = cy + r * (float)sin((double)angle);
        if (i == 0)
            ci_canvas_move_to(ctx, px, py);
        else
            ci_canvas_line_to(ctx, px, py);
    }
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
}

static void clip_evenodd(ci_canvas_t *ctx, float width, float height)
{
    /* Clip with evenodd on concentric circles, then fill a rectangle.
       Only the alternating rings should be visible. */
    float cx = width * 0.5f;
    float cy = height * 0.5f;
    float radii[4];
    int i;
    radii[0] = CI_MINF(width, height) * 0.45f;
    radii[1] = CI_MINF(width, height) * 0.33f;
    radii[2] = CI_MINF(width, height) * 0.21f;
    radii[3] = CI_MINF(width, height) * 0.09f;
    ci_canvas_set_fill_rule(ctx, CI_FILL_EVENODD);
    ci_canvas_begin_path(ctx);
    for (i = 0; i < 4; ++i) {
        ci_canvas_move_to(ctx, cx + radii[i], cy);
        ci_canvas_arc(ctx, cx, cy, radii[i], 0.0f, 6.28318531f, 0);
        ci_canvas_close_path(ctx);
    }
    ci_canvas_clip(ctx);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, height);
}

static void is_point_in_path_evenodd(ci_canvas_t *ctx, float width, float height)
{
    /* Test is_point_in_path with evenodd on concentric rectangles.
       Points in odd rings = inside, even rings = outside. */
    int index;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ci_canvas_set_fill_rule(ctx, CI_FILL_EVENODD);
    ci_canvas_begin_path(ctx);
    ci_canvas_rectangle(ctx, 16.0f, 16.0f, 224.0f, 224.0f);
    ci_canvas_rectangle(ctx, 48.0f, 48.0f, 160.0f, 160.0f);
    ci_canvas_rectangle(ctx, 80.0f, 80.0f, 96.0f, 96.0f);
    ci_canvas_fill(ctx);
    for (index = 0; index < 256; ++index)
    {
        int bits = index;
        int inside;
        float x, y;
        bits = (bits << 1 & 0xaa) | (bits >> 1 & 0x55);
        bits = (bits << 2 & 0xcc) | (bits >> 2 & 0x33);
        bits = (bits << 4 & 0xf0) | (bits >> 4 & 0x0f);
        x = (float)(bits);
        y = (float)(index);
        inside = ci_canvas_is_point_in_path(ctx, x, y);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f - inside, (float)inside, 0.0f, 1.0f);
        ci_canvas_stroke_rectangle(ctx, x - 1.5f, y - 1.5f, 3.0f, 3.0f);
    }
}

static void test_clear_rectangle(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.05f, 0.2f, 1.0f);
    ci_canvas_move_to(ctx, 0.0f, 0.0f);
    ci_canvas_line_to(ctx, width, 0.0f);
    ci_canvas_line_to(ctx, width, height);
    ci_canvas_line_to(ctx, 0.0f, height);
    ci_canvas_fill(ctx);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 0.2f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.8f * height);
    ctx->shadow_offset_x = 5.0f;
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ctx->global_composite_operation = CI_DESTINATION_ATOP;
    ci_canvas_set_global_alpha(ctx, 0.5f);
    for (y = -1.0f; y <= 1.0f; y += 1.0f)
        for (x = -1.0f; x <= 1.0f; x += 1.0f)
            ci_canvas_clear_rectangle(ctx, (0.5f + 0.05f * x) * width,
                                      (0.5f + 0.05f * y) * height,
                                      x * 0.2f * width, y * 0.2f * height);
    ci_canvas_set_global_alpha(ctx, 1.0f);
    ctx->global_composite_operation = CI_SOURCE_OVER;
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.0f);
    ci_canvas_line_to(ctx, 0.2f * width, 0.8f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
}

static void test_fill_rectangle(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.05f, 0.2f, 1.0f);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_move_to(ctx, 0.2f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.8f * height);
    for (y = -1.0f; y <= 1.0f; y += 1.0f)
        for (x = -1.0f; x <= 1.0f; x += 1.0f)
            ci_canvas_fill_rectangle(ctx, (0.5f + 0.05f * x) * width,
                                     (0.5f + 0.05f * y) * height,
                                     x * 0.2f * width, y * 0.2f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.8f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
}

static void test_stroke_rectangle(ci_canvas_t *ctx, float width, float height)
{
    float y, x;
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 0.2f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.2f * height);
    ci_canvas_line_to(ctx, 0.8f * width, 0.8f * height);
    for (y = -1.0f; y <= 1.0f; y += 1.0f)
        for (x = -1.0f; x <= 1.0f; x += 1.0f)
            ci_canvas_stroke_rectangle(ctx, (0.5f + 0.05f * x) * width,
                                       (0.5f + 0.05f * y) * height,
                                       x * 0.2f * width, y * 0.2f * height);
    ci_canvas_line_to(ctx, 0.2f * width, 0.8f * height);
    ci_canvas_close_path(ctx);
    ci_canvas_stroke(ctx);
}

static void test_text_align(ci_canvas_t *ctx, float width, float height)
{
    ci_align_style alignments[5];
    int index;
    alignments[0] = CI_ALIGN_LEFTWARD; alignments[1] = CI_ALIGN_CENTER;
    alignments[2] = CI_ALIGN_RIGHTWARD; alignments[3] = CI_ALIGN_START;
    alignments[4] = CI_ALIGN_ENDING;
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (index = 0; index < 5; ++index)
    {
        float base = (0.1f + 0.2f * (float)(index)) * height;
        ctx->text_align = alignments[index];
        ci_canvas_fill_text(ctx, "HIty", 0.5f * width, base, 1.0e30f);
    }
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_set_line_width(ctx, 1.0f);
    ci_canvas_move_to(ctx, 0.0f, 0.5f * height);
    ci_canvas_line_to(ctx, width, 0.5f * height);
    ci_canvas_move_to(ctx, 0.5f * width, 0.0f);
    ci_canvas_line_to(ctx, 0.5f * width, height);
    ci_canvas_stroke(ctx);
}

static void test_text_baseline(ci_canvas_t *ctx, float width, float height)
{
    ci_baseline_style baselines[6];
    int index;
    baselines[0] = CI_BASELINE_ALPHABETIC; baselines[1] = CI_BASELINE_TOP;
    baselines[2] = CI_BASELINE_MIDDLE; baselines[3] = CI_BASELINE_BOTTOM;
    baselines[4] = CI_BASELINE_HANGING; baselines[5] = CI_BASELINE_IDEOGRAPHIC;
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    for (index = 0; index < 6; ++index)
    {
        float left = (0.1f + 0.15f * (float)(index)) * width;
        ctx->text_baseline = baselines[index];
        ci_canvas_fill_text(ctx, "Iy", left, 0.5f * height, 1.0e30f);
    }
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_set_line_width(ctx, 1.0f);
    ci_canvas_move_to(ctx, 0.0f, 0.5f * height);
    ci_canvas_line_to(ctx, width, 0.5f * height);
    ci_canvas_move_to(ctx, 0.5f * width, 0.0f);
    ci_canvas_line_to(ctx, 0.5f * width, height);
    ci_canvas_stroke(ctx);
}

static void test_font(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_stroke_text(ctx, "D", 0.8f * width, 0.95f * height, 1.0e30f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_text(ctx, "D", 0.9f * width, 0.95f * height, 1.0e30f);
    ci_canvas_set_font(ctx, 0, 0, 0.1f * height);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "CE\xc3\x8d\xf4\x8f\xbf\xbd\xf0I", 0.0f, 0.20f * height, 1.0e30f);
    ci_canvas_set_font(ctx, 0, 0, 0.1f * height);
    ci_canvas_fill_text(ctx, "CE\xc3\x8d\xf4\x8f\xbf\xbd\xf0I", 0.65f * width, 0.20f * height, 1.0e30f);
    ci_canvas_set_font(ctx, font_b.data, (int)font_b.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "CE\xc3\x8d\xf4\x8f\xbf\xbd\xf0I", 0.0f, 0.45f * height, 1.0e30f);
    ci_canvas_set_font(ctx, font_c.data, (int)font_c.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "CE\xc3\x8d\xf4\x8f\xbf\xbd\xf0I", 0.0f, 0.70f * height, 1.0e30f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_d.data, (int)font_d.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "D", 0.1f * width, 0.95f * height, 1.0e30f);
    ci_canvas_set_font(ctx, font_e.data, (int)font_e.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "D", 0.2f * width, 0.95f * height, 1.0e30f);
    ci_canvas_set_font(ctx, font_f.data, (int)font_f.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "D", 0.3f * width, 0.95f * height, 1.0e30f);
    ci_canvas_set_font(ctx, font_g.data, (int)font_g.size, 0.2f * height);
    ci_canvas_fill_text(ctx, "D", 0.4f * width, 0.95f * height, 1.0e30f);
}

static void test_fill_text(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE, 0.4f * width, 0.0f, 0.6f * width, 0.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.00f, 0.0f, 0.00f, 1.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.45f, 0.0f, 0.25f, 0.5f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.50f, 1.0f, 0.00f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.55f, 0.0f, 0.25f, 0.5f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.00f, 0.0f, 0.50f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.3f * height);
    ci_canvas_rotate(ctx, 0.2f);
    ctx->shadow_offset_x = 2.0f;
    ctx->shadow_offset_y = 2.0f;
    ci_canvas_set_shadow_blur(ctx, 4.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.75f);
    ci_canvas_move_to(ctx, 0.0f, 0.2f * height);
    ci_canvas_fill_text(ctx, "Canvas", 0.1f * width, 0.2f * height, 1.0e30f);
    ci_canvas_line_to(ctx, width, 0.2f * height);
    ci_canvas_fill_text(ctx, "Ity\n*", 0.2f * width, 0.5f * height, width);
    ci_canvas_move_to(ctx, 0.0f, 0.5f * height);
    ci_canvas_fill_text(ctx, "*Canvas\fIty*", 0.2f * width, 0.8f * height, 0.7f * width);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_text(ctx, "****", 0.1f * width, 0.35f * height, 0.0f);
    ci_canvas_line_to(ctx, width, 0.5f * height);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 2.0f);
    ci_canvas_stroke(ctx);
}

static void test_stroke_text(ci_canvas_t *ctx, float width, float height)
{
    float segments[2];
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.3f * height);
    ci_canvas_rotate(ctx, 0.2f);
    ci_canvas_set_line_width(ctx, 2.0f);
    segments[0] = 8.0f; segments[1] = 2.0f;
    ci_canvas_set_line_dash(ctx, segments, 2);
    ci_canvas_move_to(ctx, 0.0f, 0.2f * height);
    ci_canvas_stroke_text(ctx, "Canvas", 0.1f * width, 0.2f * height, 1.0e30f);
    ci_canvas_line_to(ctx, width, 0.2f * height);
    ci_canvas_stroke_text(ctx, "Ity\n*", 0.2f * width, 0.5f * height, width);
    ci_canvas_move_to(ctx, 0.0f, 0.5f * height);
    ci_canvas_stroke_text(ctx, "*Canvas\fIty*", 0.2f * width, 0.8f * height, 0.7f * width);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_stroke_text(ctx, "****", 0.1f * width, 0.35f * height, 0.0f);
    ci_canvas_line_to(ctx, width, 0.5f * height);
    ci_canvas_set_line_dash(ctx, 0, 0);
    ci_canvas_stroke(ctx);
}

static void test_measure_text(ci_canvas_t *ctx, float width, float height)
{
    float place;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    place = 0.1f * width;
    place += ci_canvas_measure_text(ctx, "C");
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.3f * height);
    ci_canvas_rotate(ctx, 0.5f);
    ci_canvas_scale(ctx, 1.15f, 1.0f);
    ci_canvas_fill_text(ctx, "C", place, 0.2f * height, 1.0e30f);
    place += ci_canvas_measure_text(ctx, "C");
    ci_canvas_fill_text(ctx, "a", place, 0.25f * height, 1.0e30f);
    place += ci_canvas_measure_text(ctx, "a");
    ci_canvas_fill_text(ctx, "nv", place, 0.2f * height, 1.0e30f);
    place += ci_canvas_measure_text(ctx, "nv");
    ci_canvas_fill_text(ctx, "a", place, 0.15f * height, 1.0e30f);
    place += ci_canvas_measure_text(ctx, "a");
    ci_canvas_fill_text(ctx, "s", place, 0.2f * height, 1.0e30f);
}

/* ======== KERNING TEST HELPERS ======== */

static unsigned char *make_kerned_font(
    unsigned char const *font, int font_bytes,
    int left_glyph, int right_glyph, int kern_value,
    int *out_bytes)
{
    int orig_tables = (font[4] << 8) | font[5];
    int orig_dir_end = 12 + orig_tables * 16;
    int new_tables = orig_tables + 1;
    int new_dir_end = 12 + new_tables * 16;
    int kern_data_size = 24;
    int body_size = font_bytes - orig_dir_end;
    int total = new_dir_end + body_size + kern_data_size;
    unsigned char *out = (unsigned char *)calloc(1, (size_t)total);
    int i, kern_off, pos, sr, es, rs;
    unsigned short kv;
    if (!out) { *out_bytes = 0; return NULL; }
    memcpy(out, font, 12);
    out[4] = (unsigned char)((new_tables >> 8) & 0xff);
    out[5] = (unsigned char)(new_tables & 0xff);
    sr = 1; es = 0;
    while (sr * 2 <= new_tables) { sr *= 2; ++es; }
    sr *= 16;
    rs = new_tables * 16 - sr;
    out[6] = (unsigned char)((sr >> 8) & 0xff);
    out[7] = (unsigned char)(sr & 0xff);
    out[8] = (unsigned char)((es >> 8) & 0xff);
    out[9] = (unsigned char)(es & 0xff);
    out[10] = (unsigned char)((rs >> 8) & 0xff);
    out[11] = (unsigned char)(rs & 0xff);
    for (i = 0; i < orig_tables; ++i) {
        int src_off = 12 + i * 16;
        int old_off;
        memcpy(out + 12 + i * 16, font + src_off, 16);
        old_off = ((int)font[src_off + 8] << 24) |
                  ((int)font[src_off + 9] << 16) |
                  ((int)font[src_off + 10] << 8) |
                  (int)font[src_off + 11];
        old_off += 16;
        out[12 + i * 16 + 8] = (unsigned char)((old_off >> 24) & 0xff);
        out[12 + i * 16 + 9] = (unsigned char)((old_off >> 16) & 0xff);
        out[12 + i * 16 + 10] = (unsigned char)((old_off >> 8) & 0xff);
        out[12 + i * 16 + 11] = (unsigned char)(old_off & 0xff);
    }
    kern_off = new_dir_end + body_size;
    pos = 12 + orig_tables * 16;
    out[pos + 0] = 0x6b; out[pos + 1] = 0x65;
    out[pos + 2] = 0x72; out[pos + 3] = 0x6e;
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 0;
    out[pos + 8] = (unsigned char)((kern_off >> 24) & 0xff);
    out[pos + 9] = (unsigned char)((kern_off >> 16) & 0xff);
    out[pos + 10] = (unsigned char)((kern_off >> 8) & 0xff);
    out[pos + 11] = (unsigned char)(kern_off & 0xff);
    out[pos + 12] = (unsigned char)((kern_data_size >> 24) & 0xff);
    out[pos + 13] = (unsigned char)((kern_data_size >> 16) & 0xff);
    out[pos + 14] = (unsigned char)((kern_data_size >> 8) & 0xff);
    out[pos + 15] = (unsigned char)(kern_data_size & 0xff);
    memcpy(out + new_dir_end, font + orig_dir_end, (size_t)body_size);
    pos = kern_off;
    out[pos + 0] = 0; out[pos + 1] = 0;
    out[pos + 2] = 0; out[pos + 3] = 1;
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 20;
    out[pos + 8] = 0; out[pos + 9] = 1;
    out[pos + 10] = 0; out[pos + 11] = 1;
    out[pos + 12] = 0; out[pos + 13] = 6;
    out[pos + 14] = 0; out[pos + 15] = 0;
    out[pos + 16] = 0; out[pos + 17] = 0;
    out[pos + 18] = (unsigned char)((left_glyph >> 8) & 0xff);
    out[pos + 19] = (unsigned char)(left_glyph & 0xff);
    out[pos + 20] = (unsigned char)((right_glyph >> 8) & 0xff);
    out[pos + 21] = (unsigned char)(right_glyph & 0xff);
    kv = (unsigned short)(kern_value & 0xffff);
    out[pos + 22] = (unsigned char)((kv >> 8) & 0xff);
    out[pos + 23] = (unsigned char)(kv & 0xff);
    *out_bytes = total;
    return out;
}

static unsigned char *make_aat_kerned_font(
    unsigned char const *font, int font_bytes,
    int left_glyph, int right_glyph, int kern_value,
    int *out_bytes)
{
    int orig_tables = (font[4] << 8) | font[5];
    int orig_dir_end = 12 + orig_tables * 16;
    int new_tables = orig_tables + 1;
    int new_dir_end = 12 + new_tables * 16;
    int kern_data_size = 30; /* 8 header + 8 sub-header + 8 fmt0 + 6 pair */
    int body_size = font_bytes - orig_dir_end;
    int total = new_dir_end + body_size + kern_data_size;
    unsigned char *out = (unsigned char *)calloc(1, (size_t)total);
    int i, kern_off, pos, sr, es, rs;
    unsigned short kv;
    if (!out) { *out_bytes = 0; return NULL; }
    memcpy(out, font, 12);
    out[4] = (unsigned char)((new_tables >> 8) & 0xff);
    out[5] = (unsigned char)(new_tables & 0xff);
    sr = 1; es = 0;
    while (sr * 2 <= new_tables) { sr *= 2; ++es; }
    sr *= 16;
    rs = new_tables * 16 - sr;
    out[6] = (unsigned char)((sr >> 8) & 0xff);
    out[7] = (unsigned char)(sr & 0xff);
    out[8] = (unsigned char)((es >> 8) & 0xff);
    out[9] = (unsigned char)(es & 0xff);
    out[10] = (unsigned char)((rs >> 8) & 0xff);
    out[11] = (unsigned char)(rs & 0xff);
    for (i = 0; i < orig_tables; ++i) {
        int src_off = 12 + i * 16;
        int old_off;
        memcpy(out + 12 + i * 16, font + src_off, 16);
        old_off = ((int)font[src_off + 8] << 24) |
                  ((int)font[src_off + 9] << 16) |
                  ((int)font[src_off + 10] << 8) |
                  (int)font[src_off + 11];
        old_off += 16;
        out[12 + i * 16 + 8] = (unsigned char)((old_off >> 24) & 0xff);
        out[12 + i * 16 + 9] = (unsigned char)((old_off >> 16) & 0xff);
        out[12 + i * 16 + 10] = (unsigned char)((old_off >> 8) & 0xff);
        out[12 + i * 16 + 11] = (unsigned char)(old_off & 0xff);
    }
    kern_off = new_dir_end + body_size;
    pos = 12 + orig_tables * 16;
    out[pos + 0] = 0x6b; out[pos + 1] = 0x65;
    out[pos + 2] = 0x72; out[pos + 3] = 0x6e;
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 0;
    out[pos + 8] = (unsigned char)((kern_off >> 24) & 0xff);
    out[pos + 9] = (unsigned char)((kern_off >> 16) & 0xff);
    out[pos + 10] = (unsigned char)((kern_off >> 8) & 0xff);
    out[pos + 11] = (unsigned char)(kern_off & 0xff);
    out[pos + 12] = (unsigned char)((kern_data_size >> 24) & 0xff);
    out[pos + 13] = (unsigned char)((kern_data_size >> 16) & 0xff);
    out[pos + 14] = (unsigned char)((kern_data_size >> 8) & 0xff);
    out[pos + 15] = (unsigned char)(kern_data_size & 0xff);
    memcpy(out + new_dir_end, font + orig_dir_end, (size_t)body_size);
    /* Apple AAT kern table version 1 */
    pos = kern_off;
    out[pos + 0] = 0; out[pos + 1] = 1;    /* version high = 0x0001 */
    out[pos + 2] = 0; out[pos + 3] = 0;    /* version low  = 0x0000 */
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 1;    /* nTables = 1 */
    /* subtable: length=22, coverage=0x01 (horiz), format=0, tuple=0 */
    out[pos + 8] = 0; out[pos + 9] = 0;
    out[pos + 10] = 0; out[pos + 11] = 22; /* length */
    out[pos + 12] = 0x01;                   /* coverage: horizontal */
    out[pos + 13] = 0x00;                   /* format: 0 */
    out[pos + 14] = 0; out[pos + 15] = 0;  /* tupleIndex */
    /* format 0: nPairs=1, searchRange=6, entrySelector=0, rangeShift=0 */
    out[pos + 16] = 0; out[pos + 17] = 1;
    out[pos + 18] = 0; out[pos + 19] = 6;
    out[pos + 20] = 0; out[pos + 21] = 0;
    out[pos + 22] = 0; out[pos + 23] = 0;
    /* pair entry */
    out[pos + 24] = (unsigned char)((left_glyph >> 8) & 0xff);
    out[pos + 25] = (unsigned char)(left_glyph & 0xff);
    out[pos + 26] = (unsigned char)((right_glyph >> 8) & 0xff);
    out[pos + 27] = (unsigned char)(right_glyph & 0xff);
    kv = (unsigned short)(kern_value & 0xffff);
    out[pos + 28] = (unsigned char)((kv >> 8) & 0xff);
    out[pos + 29] = (unsigned char)(kv & 0xff);
    *out_bytes = total;
    return out;
}

static void test_text_kerning(ci_canvas_t *ctx, float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    ci_canvas_fill_text(ctx, "Canvas", 0.05f * width, 0.3f * height,
        1.0e30f);
    kerned_font = make_kerned_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        ci_canvas_fill_text(ctx, "Canvas", 0.05f * width,
            0.7f * height, 1.0e30f);
        free(kerned_font);
    }
}

static void test_text_kerning_measure(ci_canvas_t *ctx,
    float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    float w_normal, w_kerned, bar_y;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    w_normal = ci_canvas_measure_text(ctx, "CaCaCa");
    ci_canvas_fill_text(ctx, "CaCaCa", 0.05f * width, 0.25f * height,
        1.0e30f);
    bar_y = 0.32f * height;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.05f * width, bar_y,
        w_normal, 4.0f);
    kerned_font = make_kerned_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        w_kerned = ci_canvas_measure_text(ctx, "CaCaCa");
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_fill_text(ctx, "CaCaCa", 0.05f * width,
            0.65f * height, 1.0e30f);
        bar_y = 0.72f * height;
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
        ci_canvas_fill_rectangle(ctx, 0.05f * width, bar_y,
            w_kerned, 4.0f);
        free(kerned_font);
    }
}

/* Build a font with a GPOS PairPos format 1 (individual pairs) kern table.
   GPOS layout: Header -> ScriptList(DFLT) -> FeatureList(kern) ->
   LookupList -> Lookup(type=2) -> PairPos(fmt=1) -> Coverage + PairSet */
static unsigned char *make_gpos_fmt1_font(
    unsigned char const *font, int font_bytes,
    int left_glyph, int right_glyph, int kern_value,
    int *out_bytes)
{
    int orig_tables = (font[4] << 8) | font[5];
    int orig_dir_end = 12 + orig_tables * 16;
    int new_tables = orig_tables + 1;
    int new_dir_end = 12 + new_tables * 16;
    int gpos_size = 80;
    int body_size = font_bytes - orig_dir_end;
    int total = new_dir_end + body_size + gpos_size;
    unsigned char *out = (unsigned char *)calloc(1, (size_t)total);
    int i, gpos_off, pos, sr, es, rs;
    short kv;
    if (!out) { *out_bytes = 0; return NULL; }
    memcpy(out, font, 12);
    out[4] = (unsigned char)((new_tables >> 8) & 0xff);
    out[5] = (unsigned char)(new_tables & 0xff);
    sr = 1; es = 0;
    while (sr * 2 <= new_tables) { sr *= 2; ++es; }
    sr *= 16;
    rs = new_tables * 16 - sr;
    out[6] = (unsigned char)((sr >> 8) & 0xff);
    out[7] = (unsigned char)(sr & 0xff);
    out[8] = (unsigned char)((es >> 8) & 0xff);
    out[9] = (unsigned char)(es & 0xff);
    out[10] = (unsigned char)((rs >> 8) & 0xff);
    out[11] = (unsigned char)(rs & 0xff);
    for (i = 0; i < orig_tables; ++i) {
        int src_off = 12 + i * 16;
        int old_off;
        memcpy(out + 12 + i * 16, font + src_off, 16);
        old_off = ((int)font[src_off + 8] << 24) |
                  ((int)font[src_off + 9] << 16) |
                  ((int)font[src_off + 10] << 8) |
                  (int)font[src_off + 11];
        old_off += 16;
        out[12 + i * 16 + 8] = (unsigned char)((old_off >> 24) & 0xff);
        out[12 + i * 16 + 9] = (unsigned char)((old_off >> 16) & 0xff);
        out[12 + i * 16 + 10] = (unsigned char)((old_off >> 8) & 0xff);
        out[12 + i * 16 + 11] = (unsigned char)(old_off & 0xff);
    }
    gpos_off = new_dir_end + body_size;
    pos = 12 + orig_tables * 16;
    /* 'GPOS' tag = 0x47504f53 */
    out[pos + 0] = 0x47; out[pos + 1] = 0x50;
    out[pos + 2] = 0x4f; out[pos + 3] = 0x53;
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 0;
    out[pos + 8] = (unsigned char)((gpos_off >> 24) & 0xff);
    out[pos + 9] = (unsigned char)((gpos_off >> 16) & 0xff);
    out[pos + 10] = (unsigned char)((gpos_off >> 8) & 0xff);
    out[pos + 11] = (unsigned char)(gpos_off & 0xff);
    out[pos + 12] = (unsigned char)((gpos_size >> 24) & 0xff);
    out[pos + 13] = (unsigned char)((gpos_size >> 16) & 0xff);
    out[pos + 14] = (unsigned char)((gpos_size >> 8) & 0xff);
    out[pos + 15] = (unsigned char)(gpos_size & 0xff);
    memcpy(out + new_dir_end, font + orig_dir_end, (size_t)body_size);
    pos = gpos_off;
    /* GPOS Header (10 bytes) */
    out[pos + 0] = 0; out[pos + 1] = 1;   /* majorVersion = 1 */
    out[pos + 2] = 0; out[pos + 3] = 0;   /* minorVersion = 0 */
    out[pos + 4] = 0; out[pos + 5] = 10;  /* scriptListOffset */
    out[pos + 6] = 0; out[pos + 7] = 30;  /* featureListOffset */
    out[pos + 8] = 0; out[pos + 9] = 44;  /* lookupListOffset */
    /* ScriptList at +10 (8 bytes) */
    out[pos + 10] = 0; out[pos + 11] = 1;  /* scriptCount = 1 */
    out[pos + 12] = 0x44; out[pos + 13] = 0x46;
    out[pos + 14] = 0x4c; out[pos + 15] = 0x54; /* 'DFLT' */
    out[pos + 16] = 0; out[pos + 17] = 8;  /* scriptOffset from SL */
    /* Script at +18 (4 bytes) */
    out[pos + 18] = 0; out[pos + 19] = 4;  /* defaultLangSysOff */
    out[pos + 20] = 0; out[pos + 21] = 0;  /* langSysCount = 0 */
    /* LangSys at +22 (8 bytes) */
    out[pos + 22] = 0; out[pos + 23] = 0;  /* lookupOrder */
    out[pos + 24] = 0xff; out[pos + 25] = 0xff; /* reqFeatIdx */
    out[pos + 26] = 0; out[pos + 27] = 1;  /* featureIndexCount */
    out[pos + 28] = 0; out[pos + 29] = 0;  /* featureIndex[0]=0 */
    /* FeatureList at +30 (8 bytes) */
    out[pos + 30] = 0; out[pos + 31] = 1;  /* featureCount = 1 */
    out[pos + 32] = 0x6b; out[pos + 33] = 0x65;
    out[pos + 34] = 0x72; out[pos + 35] = 0x6e; /* 'kern' */
    out[pos + 36] = 0; out[pos + 37] = 8;  /* featureOffset from FL */
    /* Feature at +38 (6 bytes) */
    out[pos + 38] = 0; out[pos + 39] = 0;  /* featureParams */
    out[pos + 40] = 0; out[pos + 41] = 1;  /* lookupIndexCount */
    out[pos + 42] = 0; out[pos + 43] = 0;  /* lookupIndex[0]=0 */
    /* LookupList at +44 (4 bytes) */
    out[pos + 44] = 0; out[pos + 45] = 1;  /* lookupCount = 1 */
    out[pos + 46] = 0; out[pos + 47] = 4;  /* lookupOffset from LL */
    /* Lookup at +48 (8 bytes) */
    out[pos + 48] = 0; out[pos + 49] = 2;  /* lookupType = PairPos */
    out[pos + 50] = 0; out[pos + 51] = 0;  /* lookupFlag */
    out[pos + 52] = 0; out[pos + 53] = 1;  /* subTableCount = 1 */
    out[pos + 54] = 0; out[pos + 55] = 8;  /* subtableOff from Lk */
    /* PairPos Format 1 at +56 (12 bytes header) */
    out[pos + 56] = 0; out[pos + 57] = 1;  /* posFormat = 1 */
    out[pos + 58] = 0; out[pos + 59] = 12; /* coverageOff from PP */
    out[pos + 60] = 0; out[pos + 61] = 4;  /* vf1 = XAdvance */
    out[pos + 62] = 0; out[pos + 63] = 0;  /* vf2 = none */
    out[pos + 64] = 0; out[pos + 65] = 1;  /* pairSetCount = 1 */
    out[pos + 66] = 0; out[pos + 67] = 18; /* pairSetOff from PP */
    /* Coverage at +68 (6 bytes) */
    out[pos + 68] = 0; out[pos + 69] = 1;  /* coverageFormat=1 */
    out[pos + 70] = 0; out[pos + 71] = 1;  /* glyphCount = 1 */
    out[pos + 72] = (unsigned char)((left_glyph >> 8) & 0xff);
    out[pos + 73] = (unsigned char)(left_glyph & 0xff);
    /* PairSet at +74 (6 bytes) */
    out[pos + 74] = 0; out[pos + 75] = 1;  /* pairValueCount = 1 */
    out[pos + 76] = (unsigned char)((right_glyph >> 8) & 0xff);
    out[pos + 77] = (unsigned char)(right_glyph & 0xff);
    kv = (short)(kern_value & 0xffff);
    out[pos + 78] = (unsigned char)((kv >> 8) & 0xff);
    out[pos + 79] = (unsigned char)(kv & 0xff);
    *out_bytes = total;
    return out;
}

/* Build a font with a GPOS PairPos format 2 (class-based) kern table.
   left_glyph and right_glyph are assigned to class 1; kern_value
   applied for class1->class1 pair. */
static unsigned char *make_gpos_fmt2_font(
    unsigned char const *font, int font_bytes,
    int left_glyph, int right_glyph, int kern_value,
    int *out_bytes)
{
    int orig_tables = (font[4] << 8) | font[5];
    int orig_dir_end = 12 + orig_tables * 16;
    int new_tables = orig_tables + 1;
    int new_dir_end = 12 + new_tables * 16;
    int gpos_size = 102;
    int body_size = font_bytes - orig_dir_end;
    int total = new_dir_end + body_size + gpos_size;
    unsigned char *out = (unsigned char *)calloc(1, (size_t)total);
    int i, gpos_off, pos, sr, es, rs;
    short kv;
    if (!out) { *out_bytes = 0; return NULL; }
    memcpy(out, font, 12);
    out[4] = (unsigned char)((new_tables >> 8) & 0xff);
    out[5] = (unsigned char)(new_tables & 0xff);
    sr = 1; es = 0;
    while (sr * 2 <= new_tables) { sr *= 2; ++es; }
    sr *= 16;
    rs = new_tables * 16 - sr;
    out[6] = (unsigned char)((sr >> 8) & 0xff);
    out[7] = (unsigned char)(sr & 0xff);
    out[8] = (unsigned char)((es >> 8) & 0xff);
    out[9] = (unsigned char)(es & 0xff);
    out[10] = (unsigned char)((rs >> 8) & 0xff);
    out[11] = (unsigned char)(rs & 0xff);
    for (i = 0; i < orig_tables; ++i) {
        int src_off = 12 + i * 16;
        int old_off;
        memcpy(out + 12 + i * 16, font + src_off, 16);
        old_off = ((int)font[src_off + 8] << 24) |
                  ((int)font[src_off + 9] << 16) |
                  ((int)font[src_off + 10] << 8) |
                  (int)font[src_off + 11];
        old_off += 16;
        out[12 + i * 16 + 8] = (unsigned char)((old_off >> 24) & 0xff);
        out[12 + i * 16 + 9] = (unsigned char)((old_off >> 16) & 0xff);
        out[12 + i * 16 + 10] = (unsigned char)((old_off >> 8) & 0xff);
        out[12 + i * 16 + 11] = (unsigned char)(old_off & 0xff);
    }
    gpos_off = new_dir_end + body_size;
    pos = 12 + orig_tables * 16;
    out[pos + 0] = 0x47; out[pos + 1] = 0x50;
    out[pos + 2] = 0x4f; out[pos + 3] = 0x53;
    out[pos + 4] = 0; out[pos + 5] = 0;
    out[pos + 6] = 0; out[pos + 7] = 0;
    out[pos + 8] = (unsigned char)((gpos_off >> 24) & 0xff);
    out[pos + 9] = (unsigned char)((gpos_off >> 16) & 0xff);
    out[pos + 10] = (unsigned char)((gpos_off >> 8) & 0xff);
    out[pos + 11] = (unsigned char)(gpos_off & 0xff);
    out[pos + 12] = (unsigned char)((gpos_size >> 24) & 0xff);
    out[pos + 13] = (unsigned char)((gpos_size >> 16) & 0xff);
    out[pos + 14] = (unsigned char)((gpos_size >> 8) & 0xff);
    out[pos + 15] = (unsigned char)(gpos_size & 0xff);
    memcpy(out + new_dir_end, font + orig_dir_end, (size_t)body_size);
    pos = gpos_off;
    /* GPOS Header (10 bytes) — identical to fmt1 */
    out[pos + 0] = 0; out[pos + 1] = 1;
    out[pos + 2] = 0; out[pos + 3] = 0;
    out[pos + 4] = 0; out[pos + 5] = 10;
    out[pos + 6] = 0; out[pos + 7] = 30;
    out[pos + 8] = 0; out[pos + 9] = 44;
    /* ScriptList at +10 */
    out[pos + 10] = 0; out[pos + 11] = 1;
    out[pos + 12] = 0x44; out[pos + 13] = 0x46;
    out[pos + 14] = 0x4c; out[pos + 15] = 0x54;
    out[pos + 16] = 0; out[pos + 17] = 8;
    /* Script at +18 */
    out[pos + 18] = 0; out[pos + 19] = 4;
    out[pos + 20] = 0; out[pos + 21] = 0;
    /* LangSys at +22 */
    out[pos + 22] = 0; out[pos + 23] = 0;
    out[pos + 24] = 0xff; out[pos + 25] = 0xff;
    out[pos + 26] = 0; out[pos + 27] = 1;
    out[pos + 28] = 0; out[pos + 29] = 0;
    /* FeatureList at +30 */
    out[pos + 30] = 0; out[pos + 31] = 1;
    out[pos + 32] = 0x6b; out[pos + 33] = 0x65;
    out[pos + 34] = 0x72; out[pos + 35] = 0x6e;
    out[pos + 36] = 0; out[pos + 37] = 8;
    /* Feature at +38 */
    out[pos + 38] = 0; out[pos + 39] = 0;
    out[pos + 40] = 0; out[pos + 41] = 1;
    out[pos + 42] = 0; out[pos + 43] = 0;
    /* LookupList at +44 */
    out[pos + 44] = 0; out[pos + 45] = 1;
    out[pos + 46] = 0; out[pos + 47] = 4;
    /* Lookup at +48 */
    out[pos + 48] = 0; out[pos + 49] = 2;
    out[pos + 50] = 0; out[pos + 51] = 0;
    out[pos + 52] = 0; out[pos + 53] = 1;
    out[pos + 54] = 0; out[pos + 55] = 8;
    /* PairPos Format 2 at +56 (16 bytes header) */
    out[pos + 56] = 0; out[pos + 57] = 2;  /* posFormat = 2 */
    out[pos + 58] = 0; out[pos + 59] = 24; /* coverageOff from PP */
    out[pos + 60] = 0; out[pos + 61] = 4;  /* vf1 = XAdvance */
    out[pos + 62] = 0; out[pos + 63] = 0;  /* vf2 = none */
    out[pos + 64] = 0; out[pos + 65] = 30; /* classDef1Off from PP */
    out[pos + 66] = 0; out[pos + 67] = 38; /* classDef2Off from PP */
    out[pos + 68] = 0; out[pos + 69] = 2;  /* class1Count = 2 */
    out[pos + 70] = 0; out[pos + 71] = 2;  /* class2Count = 2 */
    /* Class matrix at +72 (8 bytes: 2×2 × 2 bytes xAdvance) */
    out[pos + 72] = 0; out[pos + 73] = 0;  /* c0->c0 = 0 */
    out[pos + 74] = 0; out[pos + 75] = 0;  /* c0->c1 = 0 */
    out[pos + 76] = 0; out[pos + 77] = 0;  /* c1->c0 = 0 */
    kv = (short)(kern_value & 0xffff);
    out[pos + 78] = (unsigned char)((kv >> 8) & 0xff);
    out[pos + 79] = (unsigned char)(kv & 0xff); /* c1->c1 = kern */
    /* Coverage at +80 (6 bytes) */
    out[pos + 80] = 0; out[pos + 81] = 1;
    out[pos + 82] = 0; out[pos + 83] = 1;
    out[pos + 84] = (unsigned char)((left_glyph >> 8) & 0xff);
    out[pos + 85] = (unsigned char)(left_glyph & 0xff);
    /* ClassDef1 at +86 (format 1, 8 bytes) */
    out[pos + 86] = 0; out[pos + 87] = 1;  /* classFormat = 1 */
    out[pos + 88] = (unsigned char)((left_glyph >> 8) & 0xff);
    out[pos + 89] = (unsigned char)(left_glyph & 0xff);
    out[pos + 90] = 0; out[pos + 91] = 1;  /* glyphCount = 1 */
    out[pos + 92] = 0; out[pos + 93] = 1;  /* class = 1 */
    /* ClassDef2 at +94 (format 1, 8 bytes) */
    out[pos + 94] = 0; out[pos + 95] = 1;
    out[pos + 96] = (unsigned char)((right_glyph >> 8) & 0xff);
    out[pos + 97] = (unsigned char)(right_glyph & 0xff);
    out[pos + 98] = 0; out[pos + 99] = 1;
    out[pos + 100] = 0; out[pos + 101] = 1;
    *out_bytes = total;
    return out;
}

static void test_text_kerning_aat(ci_canvas_t *ctx,
    float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    ci_canvas_fill_text(ctx, "Canvas", 0.05f * width, 0.3f * height,
        1.0e30f);
    kerned_font = make_aat_kerned_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        ci_canvas_fill_text(ctx, "Canvas", 0.05f * width,
            0.7f * height, 1.0e30f);
        free(kerned_font);
    }
}

static void test_text_kerning_aat_measure(ci_canvas_t *ctx,
    float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    float w_normal, w_kerned, bar_y;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    w_normal = ci_canvas_measure_text(ctx, "CaCaCa");
    ci_canvas_fill_text(ctx, "CaCaCa", 0.05f * width, 0.25f * height,
        1.0e30f);
    bar_y = 0.32f * height;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.05f * width, bar_y,
        w_normal, 4.0f);
    kerned_font = make_aat_kerned_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        w_kerned = ci_canvas_measure_text(ctx, "CaCaCa");
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_fill_text(ctx, "CaCaCa", 0.05f * width,
            0.65f * height, 1.0e30f);
        bar_y = 0.72f * height;
        ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
        ci_canvas_fill_rectangle(ctx, 0.05f * width, bar_y,
            w_kerned, 4.0f);
        free(kerned_font);
    }
}

static void test_text_kerning_gpos_fmt1(ci_canvas_t *ctx,
    float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    ci_canvas_fill_text(ctx, "Canvas", 0.05f * width, 0.3f * height,
        1.0e30f);
    kerned_font = make_gpos_fmt1_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        ci_canvas_fill_text(ctx, "Canvas", 0.05f * width,
            0.7f * height, 1.0e30f);
        free(kerned_font);
    }
}

static void test_text_kerning_gpos_fmt2(ci_canvas_t *ctx,
    float width, float height)
{
    int idx, glyph_c, glyph_a, kerned_size;
    unsigned char *kerned_font;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.2f * height);
    idx = 0;
    glyph_c = ci_character_to_glyph(ctx, "C", &idx);
    idx = 0;
    glyph_a = ci_character_to_glyph(ctx, "a", &idx);
    ci_canvas_fill_text(ctx, "Canvas", 0.05f * width, 0.3f * height,
        1.0e30f);
    kerned_font = make_gpos_fmt2_font(font_a.data, (int)font_a.size,
        glyph_c, glyph_a, -200, &kerned_size);
    if (kerned_font) {
        ci_canvas_set_font(ctx, kerned_font, kerned_size,
            0.2f * height);
        ci_canvas_fill_text(ctx, "Canvas", 0.05f * width,
            0.7f * height, 1.0e30f);
        free(kerned_font);
    }
}

static void test_draw_image(ci_canvas_t *ctx, float width, float height)
{
    unsigned char checker[1024];
    int index;
    float row, column;
    unsigned char pixel[4];
    for (index = 0; index < 1024; ++index)
        checker[index] = (unsigned char)(
            (((index >> 2 & 1) ^ (index >> 6 & 1)) |
             ((index & 3) == 3)) * 255);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64,
                         0.0f, 0.0f, width * 0.75f, height * 0.75f);
    for (row = 0.0f; row < 4.0f; row += 1.0f)
        for (column = 0.0f; column < 4.0f; column += 1.0f)
            ci_canvas_draw_image(ctx, checker, 16, 16, 64,
                                 column * 17.25f, row * 17.25f, 16.0f, 16.0f);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64, 128.0f, 0.0f, 32.0f, 8.0f);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64, 128.0f, 48.0f, 32.0f, -32.0f);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64, 200.0f, 16.0f, -32.0f, 32.0f);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64, 128.0f, 64.0f, 32.0f, 0.0f);
    ci_canvas_draw_image(ctx, 0, 16, 16, 64, 200.0f, 64.0f, 32.0f, 32.0f);
    pixel[0] = 0; pixel[1] = 255; pixel[2] = 0; pixel[3] = 255;
    ci_canvas_draw_image(ctx, pixel, 1, 1, 4,
                         width * 0.875f, height * 0.25f, 1.0f, 1.0f);
    ci_canvas_draw_image(ctx, pixel, 1, 1, 4,
                         width * 0.875f, height * 0.5f, 16.0f, 16.0f);
    ci_canvas_rotate(ctx, 0.2f);
    ctx->global_composite_operation = CI_LIGHTER;
    ci_canvas_set_global_alpha(ctx, 1.0f);
    ci_canvas_draw_image(ctx, checker, 16, 16, 64,
                         0.25f * width, 0.25f * height,
                         0.5f * width, 0.5f * height);
}

static void draw_image_matted(ci_canvas_t *ctx, float width, float height)
{
    unsigned char checker[36];
    float y_pos, size_y, x_pos, size_x;
    int step_y, step_x;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 1.0f, 0.0f, 0.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, height);
    checker[0]=0; checker[1]=0; checker[2]=255; checker[3]=255;
    checker[4]=255; checker[5]=0; checker[6]=0; checker[7]=0;
    checker[8]=0; checker[9]=0; checker[10]=255; checker[11]=255;
    checker[12]=255; checker[13]=0; checker[14]=0; checker[15]=0;
    checker[16]=0; checker[17]=0; checker[18]=255; checker[19]=255;
    checker[20]=255; checker[21]=0; checker[22]=0; checker[23]=0;
    checker[24]=0; checker[25]=0; checker[26]=255; checker[27]=255;
    checker[28]=255; checker[29]=0; checker[30]=0; checker[31]=0;
    checker[32]=0; checker[33]=0; checker[34]=255; checker[35]=255;
    y_pos = 0.5f;
    size_y = 3.0f;
    for (step_y = 0; step_y < 20 && y_pos < height; ++step_y)
    {
        x_pos = 0.5f;
        size_x = 3.0f;
        for (step_x = 0; step_x < 20 && x_pos < width; ++step_x)
        {
            ci_canvas_draw_image(ctx, checker, 3, 3, 12, x_pos, y_pos, size_x, size_y);
            x_pos += size_x + 5.0f;
            size_x *= 1.5f;
        }
        y_pos += size_y + 5.0f;
        size_y *= 1.5f;
    }
}

static void test_get_image_data(ci_canvas_t *ctx, float width, float height)
{
    int index;
    unsigned char data[4939];
    unsigned hash;
    unsigned expected;
    for (index = 0; index < 100; ++index)
    {
        ci_canvas_set_color(ctx, CI_FILL_STYLE,
                            (float)(index / 2 % 2),
                            (float)(index / 4 % 2),
                            (float)(index / 8 % 2),
                            (float)(index / 16 % 2));
        ci_canvas_fill_rectangle(ctx, 3.0f * (float)(index % 10),
                                 3.0f * (float)(index / 10),
                                 3.0f, 3.0f);
    }
    data[0] = 150;
    for (index = 1; index < 4939; ++index)
        data[index] = (unsigned char)(
            (data[index - 1] * 137 + 53) & 255);
    ci_canvas_get_image_data(ctx, data + 2, 35, 35, 141, -10, -10);
    hash = 0;
    for (index = 0; index < 4939; ++index)
        hash = (((hash & 0x1ffff) << 15) | (hash >> 17)) ^ data[index];
    expected = 0xf53f9792u;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, (float)(hash != expected), (float)(hash == expected), 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 30.0f, 0.0f, width, 30.0f);
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE, 0.0f, 0.0f, width, 0.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.45f * height, width, 0.1f * height);
    ci_canvas_get_image_data(ctx, 0, 32, 32, 128, 0, 0);
}

static void test_put_image_data(ci_canvas_t *ctx, float width, float height)
{
    unsigned char checker[2052];
    int index, iy, ix;
    for (index = 0; index < 2048; ++index)
        checker[index + 2] = (unsigned char)(
            ((((index >> 2 & 1) ^ (index >> 7 & 1)) |
              ((index & 3) == 3)) &
             (index >> 10 & 1)) * 255);
    checker[0] = 157;
    checker[1] = 157;
    checker[2050] = 157;
    checker[2051] = 157;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.05f, 0.2f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, 0.25f * width, 0.25f * height);
    ci_canvas_set_global_alpha(ctx, 0.5f);
    ctx->global_composite_operation = CI_LIGHTER;
    ci_canvas_rotate(ctx, 0.2f);
    for (iy = -10; iy < (int)(height); iy += 29)
        for (ix = -10; ix < (int)(width); ix += 29)
            ci_canvas_put_image_data(ctx, checker + 6, 16, 16, 128, ix, iy);
    ci_canvas_put_image_data(ctx, 0, 32, 32, 128, 0, 0);
}

static void test_save_restore(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_rectangle(ctx, width * 0.25f, height * 0.25f,
                        width * 0.25f, height * 0.25f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 1.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 8.0f);
    ci_canvas_save(ctx);
    ci_canvas_clip(ctx);
    ci_canvas_begin_path(ctx);
    ci_canvas_rectangle(ctx, width * 0.25f, height * 0.25f,
                        width * 0.5f, height * 0.5f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.0f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 1.0f);
    ci_canvas_restore(ctx);
    ci_canvas_restore(ctx);
    ci_canvas_stroke(ctx);
    ci_canvas_save(ctx);
    ci_canvas_save(ctx);
}

static void example_button(ci_canvas_t *ctx, float width, float height)
{
    float left, right, top, bottom, mid_x, mid_y;
    left = (float)floor((double)(0.25f * width) + 0.5);
    right = (float)floor((double)(0.75f * width) + 0.5);
    top = (float)floor((double)(0.375f * height) + 0.5);
    bottom = (float)floor((double)(0.625f * height) + 0.5);
    mid_x = (left + right) * 0.5f;
    mid_y = (top + bottom) * 0.5f;
    ctx->shadow_offset_x = 3.0f;
    ctx->shadow_offset_y = 3.0f;
    ci_canvas_set_shadow_blur(ctx, 3.0f);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE, 0.0f, top, 0.0f, bottom);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 0.3f, 0.3f, 0.3f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 0.2f, 0.2f, 0.2f, 1.0f);
    ci_canvas_move_to(ctx, left + 0.5f, mid_y);
    ci_canvas_arc_to(ctx, left + 0.5f, top + 0.5f, mid_x, top + 0.5f, 4.0f);
    ci_canvas_arc_to(ctx, right - 0.5f, top + 0.5f, right - 0.5f, mid_y, 4.0f);
    ci_canvas_arc_to(ctx, right - 0.5f, bottom - 0.5f, mid_x, bottom - 0.5f, 4.0f);
    ci_canvas_arc_to(ctx, left + 0.5f, bottom - 0.5f, left + 0.5f, mid_y, 4.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_fill(ctx);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.0f);
    ci_canvas_set_font(ctx, font_a.data, (int)font_a.size, 0.075f * height);
    ctx->text_align = CI_ALIGN_CENTER;
    ctx->text_baseline = CI_BASELINE_MIDDLE;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.8f, 0.8f, 0.8f, 1.0f);
    ci_canvas_fill_text(ctx, "* Cats", 0.5f * width, 0.5f * height, 1.0e30f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.4f, 0.4f, 1.0f);
    ci_canvas_fill_rectangle(ctx, left + 4.0f, top + 1.0f, right - left - 8.0f, 1.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.1f, 0.1f, 0.1f, 1.0f);
    ci_canvas_stroke(ctx);
}

static void example_smiley(ci_canvas_t *ctx, float width, float height)
{
    float center_x = 0.5f * width;
    float center_y = 0.5f * height;
    float radius = CI_MINF(width, height) * 0.4f;
    ci_canvas_set_radial_gradient(ctx, CI_FILL_STYLE,
                                  center_x, center_y, 0.0f,
                                  center_x, center_y, radius);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 1.0f, 0.9f, 0.2f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.95f, 0.95f, 0.65f, 0.15f, 1.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 0.9f, 0.55f, 0.0f, 1.0f);
    ci_canvas_arc(ctx, center_x, center_y, radius, 0.0f, 6.28318531f, 0);
    ci_canvas_fill(ctx);
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE,
                                  center_x, center_y - 0.95f * radius,
                                  center_x, center_y);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, center_x, center_y - 0.15f * radius, 0.8f * radius,
                  0.0f, 6.28318531f, 0);
    ci_canvas_fill(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 0.95f);
    ci_canvas_set_line_width(ctx, 0.2f * radius);
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, center_x - 0.2f * radius, center_y - 0.5f * radius);
    ci_canvas_line_to(ctx, center_x - 0.2f * radius, center_y - 0.2f * radius);
    ci_canvas_move_to(ctx, center_x + 0.2f * radius, center_y - 0.5f * radius);
    ci_canvas_line_to(ctx, center_x + 0.2f * radius, center_y - 0.2f * radius);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0f, 0.0f, 0.95f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, center_x - 0.6f * radius, center_y + 0.1f * radius);
    ci_canvas_bezier_curve_to(ctx, center_x - 0.3f * radius, center_y + 0.8f * radius,
                              center_x + 0.3f * radius, center_y + 0.8f * radius,
                              center_x + 0.6f * radius, center_y + 0.1f * radius);
    ci_canvas_bezier_curve_to(ctx, center_x + 0.3f * radius, center_y + 0.3f * radius,
                              center_x - 0.3f * radius, center_y + 0.3f * radius,
                              center_x - 0.6f * radius, center_y + 0.1f * radius);
    ci_canvas_fill(ctx);
}

static void example_knot(ci_canvas_t *ctx, float width, float height)
{
    float points[6][8];
    int index;
    points[0][0]=0.0f; points[0][1]=-1.0f; points[0][2]=0.0f; points[0][3]=0.0f;
    points[0][4]=0.0f; points[0][5]=0.0f; points[0][6]=0.0f; points[0][7]=1.0f;
    points[1][0]=-1.0f; points[1][1]=-1.0f; points[1][2]=-1.0f; points[1][3]=0.0f;
    points[1][4]=1.0f; points[1][5]=0.0f; points[1][6]=1.0f; points[1][7]=1.0f;
    points[2][0]=2.0f; points[2][1]=1.0f; points[2][2]=2.0f; points[2][3]=-2.0f;
    points[2][4]=-1.0f; points[2][5]=-2.0f; points[2][6]=-1.0f; points[2][7]=-1.0f;
    points[3][0]=-2.0f; points[3][1]=-1.0f; points[3][2]=-2.0f; points[3][3]=2.0f;
    points[3][4]=1.0f; points[3][5]=2.0f; points[3][6]=1.0f; points[3][7]=1.0f;
    points[4][0]=-2.0f; points[4][1]=-1.0f; points[4][2]=-2.0f; points[4][3]=-3.0f;
    points[4][4]=0.0f; points[4][5]=-3.0f; points[4][6]=0.0f; points[4][7]=-1.0f;
    points[5][0]=2.0f; points[5][1]=1.0f; points[5][2]=2.0f; points[5][3]=3.0f;
    points[5][4]=0.0f; points[5][5]=3.0f; points[5][6]=0.0f; points[5][7]=1.0f;
    ci_canvas_translate(ctx, width * 0.5f, height * 0.5f);
    ci_canvas_scale(ctx, width * 0.17f, height * 0.17f);
    ci_canvas_rotate(ctx, -15.0f * 3.14159265f / 180.0f);
    for (index = 0; index < 6; ++index)
    {
        ci_canvas_begin_path(ctx);
        ci_canvas_move_to(ctx,
            1.01f * points[index][0] - 0.01f * points[index][2],
            1.01f * points[index][1] - 0.01f * points[index][3]);
        ci_canvas_line_to(ctx, points[index][0], points[index][1]);
        ci_canvas_bezier_curve_to(ctx, points[index][2], points[index][3],
                                  points[index][4], points[index][5],
                                  points[index][6], points[index][7]);
        ci_canvas_line_to(ctx,
            -0.01f * points[index][4] + 1.01f * points[index][6],
            -0.01f * points[index][5] + 1.01f * points[index][7]);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_set_line_width(ctx, 0.75f);
        ctx->line_cap = CI_CAP_BUTT;
        ci_canvas_stroke(ctx);
        ci_canvas_set_radial_gradient(ctx, CI_STROKE_STYLE,
                                      0.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 3.0f);
        ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 0.0f, 0.8f, 1.0f, 0.6f, 1.0f);
        ci_canvas_add_color_stop(ctx, CI_STROKE_STYLE, 1.0f, 0.1f, 0.5f, 0.1f, 1.0f);
        ci_canvas_set_line_width(ctx, 0.5f);
        ctx->line_cap = CI_CAP_CIRCLE;
        ci_canvas_stroke(ctx);
    }
}

static void example_icon(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 1.0f);
    ctx->shadow_offset_x = width / 64.0f;
    ctx->shadow_offset_y = height / 64.0f;
    ci_canvas_set_shadow_blur(ctx, CI_MINF(width, height) / 32.0f);
    ci_canvas_scale(ctx, width / 32.0f, height / 32.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.4f, 0.05f, 0.2f, 1.0f);
    ci_canvas_move_to(ctx, 15.5f, 1.0f);
    ci_canvas_arc_to(ctx, 30.0f, 1.0f, 30.0f, 15.5f, 6.0f);
    ci_canvas_arc_to(ctx, 30.0f, 30.0f, 15.5f, 30.0f, 6.0f);
    ci_canvas_arc_to(ctx, 1.0f, 30.0f, 1.0f, 15.5f, 6.0f);
    ci_canvas_arc_to(ctx, 1.0f, 1.0f, 15.5f, 1.0f, 6.0f);
    ci_canvas_fill(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.5f, 0.5f, 0.5f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_move_to(ctx, 11.0f, 16.0f);
    ci_canvas_line_to(ctx, 27.0f, 16.0f);
    ci_canvas_move_to(ctx, 2.0f, 23.0f);
    ci_canvas_line_to(ctx, 29.0f, 23.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.75f, 0.75f, 0.75f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, 25.0f, 22.0f, 0.5f, 0.0f, 6.28318531f, 0);
    ci_canvas_move_to(ctx, 19.0f, 6.0f); ci_canvas_line_to(ctx, 18.5f, 8.0f);
    ci_canvas_move_to(ctx, 20.0f, 6.0f); ci_canvas_line_to(ctx, 20.0f, 8.0f);
    ci_canvas_move_to(ctx, 21.0f, 6.0f); ci_canvas_line_to(ctx, 21.5f, 8.0f);
    ci_canvas_move_to(ctx, 17.0f, 14.0f); ci_canvas_line_to(ctx, 16.0f, 18.0f);
    ci_canvas_move_to(ctx, 20.0f, 14.0f); ci_canvas_line_to(ctx, 20.0f, 18.0f);
    ci_canvas_move_to(ctx, 23.0f, 14.0f); ci_canvas_line_to(ctx, 24.0f, 18.0f);
    ci_canvas_move_to(ctx, 18.0f, 9.0f); ci_canvas_line_to(ctx, 22.0f, 9.0f);
    ci_canvas_move_to(ctx, 18.0f, 13.0f); ci_canvas_line_to(ctx, 22.0f, 13.0f);
    ci_canvas_rectangle(ctx, 16.0f, 8.0f, 8.0f, 6.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
    ci_canvas_begin_path(ctx);
    ci_canvas_arc(ctx, 19.0f, 12.0f, 9.0f, 0.0f, 6.28318531f, 0);
    ci_canvas_move_to(ctx, 12.3f, 17.3f); ci_canvas_line_to(ctx, 3.3f, 26.3f);
    ci_canvas_move_to(ctx, 13.0f, 18.0f); ci_canvas_line_to(ctx, 4.0f, 27.0f);
    ci_canvas_move_to(ctx, 13.7f, 18.7f); ci_canvas_line_to(ctx, 4.7f, 27.7f);
    ci_canvas_stroke(ctx);
}

static void example_illusion(ci_canvas_t *ctx, float width, float height)
{
    float spot;
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.4f, 1.0f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, width, height);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.8f, 0.8f, 0.0f, 1.0f);
    ci_canvas_set_line_width(ctx, 0.4f);
    for (spot = 0.0f; spot < 240.0f; spot += 1.0f)
    {
        float angle = (float)fmod((double)(spot * 0.61803398875f), 1.0) * 6.28318531f;
        float radius = spot / 240.0f * 0.5f * (float)sqrt((double)(width*width + height*height));
        float size = CI_MINF(width, height) * (float)sqrt((double)spot) / 240.0f;
        ci_canvas_set_transform(ctx, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
        ci_canvas_translate(ctx, 0.5f * width + radius * (float)cos((double)angle),
                            0.5f * height + radius * (float)sin((double)angle));
        ci_canvas_rotate(ctx, angle - 1.3f);
        ci_canvas_scale(ctx, 0.8f * size, 0.6f * size);
        ci_canvas_rotate(ctx, 1.3f);
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, 0.0f, 0.0f, 1.0f, 0.0f, 6.28318531f, 0);
        ci_canvas_fill(ctx);
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, 0.0f, 0.0f, 1.0f, 0.0f, 3.14159265f, 0);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 1.0f, 1.0f, 1.0f);
        ci_canvas_stroke(ctx);
        ci_canvas_begin_path(ctx);
        ci_canvas_arc(ctx, 0.0f, 0.0f, 1.0f, 3.14159265f, 6.28318531f, 0);
        ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.0f, 0.0f, 0.0f, 1.0f);
        ci_canvas_stroke(ctx);
    }
}

static void example_star(ci_canvas_t *ctx, float width, float height)
{
    float segments[8];
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ci_canvas_move_to(ctx, 128.0f, 28.0f);
    ci_canvas_line_to(ctx, 157.0f, 87.0f);
    ci_canvas_line_to(ctx, 223.0f, 97.0f);
    ci_canvas_line_to(ctx, 175.0f, 143.0f);
    ci_canvas_line_to(ctx, 186.0f, 208.0f);
    ci_canvas_line_to(ctx, 128.0f, 178.0f);
    ci_canvas_line_to(ctx, 69.0f, 208.0f);
    ci_canvas_line_to(ctx, 80.0f, 143.0f);
    ci_canvas_line_to(ctx, 32.0f, 97.0f);
    ci_canvas_line_to(ctx, 98.0f, 87.0f);
    ci_canvas_close_path(ctx);
    ci_canvas_set_shadow_blur(ctx, 8.0f);
    ctx->shadow_offset_y = 4.0f;
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.5f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 1.0f, 0.9f, 0.2f, 1.0f);
    ci_canvas_fill(ctx);
    ctx->line_join = CI_JOIN_ROUNDED;
    ci_canvas_set_line_width(ctx, 12.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.9f, 0.0f, 0.5f, 1.0f);
    ci_canvas_stroke(ctx);
    segments[0]=21.0f; segments[1]=9.0f; segments[2]=1.0f; segments[3]=9.0f;
    segments[4]=7.0f; segments[5]=9.0f; segments[6]=1.0f; segments[7]=9.0f;
    ci_canvas_set_line_dash(ctx, segments, 8);
    ctx->line_dash_offset = 10.0f;
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 0.95f, 0.65f, 0.15f, 1.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.0f);
    ci_canvas_set_linear_gradient(ctx, CI_FILL_STYLE, 64.0f, 0.0f, 192.0f, 256.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.30f, 1.0f, 1.0f, 1.0f, 0.0f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.35f, 1.0f, 1.0f, 1.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.45f, 1.0f, 1.0f, 1.0f, 0.8f);
    ci_canvas_add_color_stop(ctx, CI_FILL_STYLE, 0.50f, 1.0f, 1.0f, 1.0f, 0.0f);
    ctx->global_composite_operation = CI_SOURCE_ATOP;
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, 256.0f, 256.0f);
}

static void example_neon(ci_canvas_t *ctx, float width, float height)
{
    ci_canvas_scale(ctx, width / 256.0f, height / 256.0f);
    ci_canvas_set_color(ctx, CI_FILL_STYLE, 0.0f, 0.0625f, 0.125f, 1.0f);
    ci_canvas_fill_rectangle(ctx, 0.0f, 0.0f, 256.0f, 256.0f);
    ci_canvas_move_to(ctx, 45.5f, 96.2f);
    ci_canvas_bezier_curve_to(ctx, 45.5f, 96.2f, 31.3f, 106.2f, 31.5f, 113.1f);
    ci_canvas_bezier_curve_to(ctx, 31.7f, 119.5f, 50.6f, 104.8f, 50.6f, 93.9f);
    ci_canvas_bezier_curve_to(ctx, 50.6f, 91.1f, 46.6f, 89.1f, 43.3f, 89.4f);
    ci_canvas_bezier_curve_to(ctx, 27.5f, 90.6f, 8.5f, 108.2f, 8.8f, 121.8f);
    ci_canvas_bezier_curve_to(ctx, 9.1f, 133.1f, 21.3f, 136.6f, 29.8f, 136.3f);
    ci_canvas_bezier_curve_to(ctx, 52.4f, 135.5f, 62.3f, 115.6f, 62.3f, 115.6f);
    ci_canvas_move_to(ctx, 81.0f, 120.2f);
    ci_canvas_bezier_curve_to(ctx, 81.0f, 120.2f, 60.2f, 123.0f, 59.7f, 130.8f);
    ci_canvas_bezier_curve_to(ctx, 59.2f, 140.6f, 73.8f, 136.4f, 78.3f, 125.3f);
    ci_canvas_move_to(ctx, 80.7f, 130.5f);
    ci_canvas_bezier_curve_to(ctx, 79.5f, 132.4f, 80.9f, 135.0f, 83.4f, 135.0f);
    ci_canvas_bezier_curve_to(ctx, 95.8f, 135.6f, 99.3f, 122.5f, 111.4f, 121.6f);
    ci_canvas_bezier_curve_to(ctx, 112.8f, 121.5f, 114.0f, 123.0f, 114.0f, 124.3f);
    ci_canvas_bezier_curve_to(ctx, 113.9f, 126.1f, 106.7f, 133.9f, 106.7f, 133.9f);
    ci_canvas_move_to(ctx, 118.5f, 122.9f);
    ci_canvas_bezier_curve_to(ctx, 118.5f, 122.9f, 122.1f, 118.8f, 126.1f, 122.0f);
    ci_canvas_bezier_curve_to(ctx, 131.4f, 126.4f, 118.7f, 131.6f, 124.3f, 134.7f);
    ci_canvas_bezier_curve_to(ctx, 130.0f, 137.8f, 150.0f, 116.5f, 156.0f, 120.2f);
    ci_canvas_bezier_curve_to(ctx, 160.2f, 122.8f, 149.0f, 133.5f, 155.6f, 133.6f);
    ci_canvas_bezier_curve_to(ctx, 162.0f, 133.4f, 173.8f, 118.3f, 168.0f, 117.8f);
    ci_canvas_move_to(ctx, 173.1f, 123.2f);
    ci_canvas_bezier_curve_to(ctx, 177.8f, 124.8f, 182.8f, 123.2f, 187.0f, 119.7f);
    ci_canvas_move_to(ctx, 206.1f, 118.6f);
    ci_canvas_bezier_curve_to(ctx, 206.1f, 118.6f, 185.3f, 121.3f, 185.1f, 129.1f);
    ci_canvas_bezier_curve_to(ctx, 185.0f, 138.7f, 199.9f, 135.4f, 203.6f, 123.6f);
    ci_canvas_move_to(ctx, 205.6f, 129.9f);
    ci_canvas_bezier_curve_to(ctx, 204.4f, 131.8f, 205.8f, 134.4f, 208.3f, 134.4f);
    ci_canvas_bezier_curve_to(ctx, 220.3f, 134.4f, 246.6f, 117.1f, 246.6f, 117.1f);
    ci_canvas_move_to(ctx, 247.0f, 122.4f);
    ci_canvas_bezier_curve_to(ctx, 245.9f, 128.5f, 243.9f, 139.7f, 231.2f, 131.5f);
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_set_shadow_color(ctx, 1.0f, 0.5f, 0.0f, 1.0f);
    ci_canvas_set_shadow_blur(ctx, 20.0f);
    ci_canvas_set_line_width(ctx, 4.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.5f, 0.0f, 1.0f);
    ci_canvas_stroke(ctx);
    ci_canvas_set_shadow_blur(ctx, 5.0f);
    ci_canvas_set_line_width(ctx, 3.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE, 1.0f, 0.625f, 0.0f, 1.0f);
    ci_canvas_stroke(ctx);
}

/* ======== TEST HARNESS ======== */

typedef struct {
    unsigned hash;
    int width, height;
    void (*call)(ci_canvas_t *, float, float);
    char const *name;
} test_entry;

static test_entry const tests[] = {
    { 0xc99ddee7, 256, 256, scale_uniform, "scale_uniform" },
    { 0xe93d3c6f, 256, 256, scale_non_uniform, "scale_non_uniform" },
    { 0x05a0e377, 256, 256, test_rotate, "rotate" },
    { 0x36e7fa56, 256, 256, test_translate, "translate" },
    { 0xcfae3e4f, 256, 256, test_transform, "transform" },
    { 0x98f5594a, 256, 256, transform_fill, "transform_fill" },
    { 0x822964b0, 256, 256, transform_stroke, "transform_stroke" },
    { 0xb7056a3a, 256, 256, test_set_transform, "set_transform" },
    { 0x8f6dd6c3, 256, 256, test_global_alpha, "global_alpha" },
    { 0x98a0609d, 256, 256, test_global_composite_operation, "global_composite_operation" },
    { 0x9def5b00, 256, 256, shadow_color, "shadow_color" },
    { 0x8294edd8, 256, 256, shadow_offset, "shadow_offset" },
    { 0xcdeba51c, 256, 256, shadow_offset_offscreen, "shadow_offset_offscreen" },
    { 0x5b542224, 256, 256, shadow_blur, "shadow_blur" },
    { 0xd6c150e6, 256, 256, shadow_blur_offscreen, "shadow_blur_offscreen" },
    { 0x5affc092, 256, 256, shadow_blur_composite, "shadow_blur_composite" },
    { 0x1720e9b2, 256, 256, test_line_width, "line_width" },
    { 0xf8d2bb0d, 256, 256, line_width_angular, "line_width_angular" },
    { 0x7bda8673, 256, 256, test_line_cap, "line_cap" },
    { 0x53639198, 256, 256, line_cap_offscreen, "line_cap_offscreen" },
    { 0x8f49c41d, 256, 256, test_line_join, "line_join" },
    { 0xca27ce8c, 256, 256, line_join_offscreen, "line_join_offscreen" },
    { 0xe68273e2, 256, 256, test_miter_limit, "miter_limit" },
    { 0x27c38a8a, 256, 256, line_dash_offset, "line_dash_offset" },
    { 0x129f9595, 256, 256, test_line_dash, "line_dash" },
    { 0x88a74152, 256, 256, line_dash_closed, "line_dash_closed" },
    { 0x064f194d, 256, 256, line_dash_overlap, "line_dash_overlap" },
    { 0xf7259c0f, 256, 256, line_dash_offscreen, "line_dash_offscreen" },
    { 0xeb4338e8, 256, 256, test_color, "color" },
    { 0x6dc35a07, 256, 256, test_linear_gradient, "linear_gradient" },
    { 0x418fe678, 256, 256, test_radial_gradient, "radial_gradient" },
    { 0xd5dc2c85, 256, 256, test_conic_gradient, "conic_gradient" },
    { 0x3eda353a, 256, 256, conic_gradient_rect, "conic_gradient_rect" },
    { 0x67aada11, 256, 256, test_color_stop, "color_stop" },
    { 0xc6c721d6, 256, 256, test_pattern, "pattern" },
    { 0xb0b391cd, 256, 256, test_begin_path, "begin_path" },
    { 0xf79ed394, 256, 256, test_move_to, "move_to" },
    { 0xe9602309, 256, 256, test_close_path, "close_path" },
    { 0x3160ace7, 256, 256, test_line_to, "line_to" },
    { 0xb6176812, 256, 256, test_quadratic_curve_to, "quadratic_curve_to" },
    { 0x5f523029, 256, 256, test_bezier_curve_to, "bezier_curve_to" },
    { 0x1f847aaf, 256, 256, test_arc_to, "arc_to" },
    { 0x26457553, 256, 256, test_arc, "arc" },
    { 0x5f042c5d, 256, 256, test_ellipse, "ellipse" },
    { 0x3c8a0970, 256, 256, test_ellipse_rotated, "ellipse_rotated" },
    { 0x7520990c, 256, 256, test_rectangle, "rectangle" },
    { 0x7290ed64, 256, 256, test_round_rect, "round_rect" },
    { 0x407aa93b, 256, 256, test_round_rect_clamped, "round_rect_clamped" },
    { 0xf1d774dc, 256, 256, test_fill, "fill" },
    { 0x5e6e6b75, 256, 256, fill_rounding, "fill_rounding" },
    { 0xf0cf6566, 256, 256, fill_converging, "fill_converging" },
    { 0x3692d10e, 256, 256, fill_zone_plate, "fill_zone_plate" },
    { 0x2003f926, 256, 256, test_stroke, "stroke" },
    { 0xc44fc157, 256, 256, stroke_wide, "stroke_wide" },
    { 0x691cfe49, 256, 256, stroke_inner_join, "stroke_inner_join" },
    { 0xc0bd9324, 256, 256, stroke_spiral, "stroke_spiral" },
    { 0x3b2dae15, 256, 256, stroke_long, "stroke_long" },
    { 0xa7e06559, 256, 256, test_clip, "clip" },
    { 0x31e6112b, 256, 256, clip_winding, "clip_winding" },
    { 0xc2188d67, 256, 256, test_is_point_in_path, "is_point_in_path" },
    { 0x6505bdc9, 256, 256, is_point_in_path_offscreen, "is_point_in_path_offscreen" },
    { 0x3a3803eb, 256, 256, fill_evenodd, "fill_evenodd" },
    { 0x130d5578, 256, 256, fill_evenodd_star, "fill_evenodd_star" },
    { 0x64454d62, 256, 256, clip_evenodd, "clip_evenodd" },
    { 0xec607e9e, 256, 256, is_point_in_path_evenodd, "is_point_in_path_evenodd" },
    { 0x5e792c96, 256, 256, test_clear_rectangle, "clear_rectangle" },
    { 0x286e96fa, 256, 256, test_fill_rectangle, "fill_rectangle" },
    { 0xc2b0803d, 256, 256, test_stroke_rectangle, "stroke_rectangle" },
    { 0xe6c4d9c7, 256, 256, test_text_align, "text_align" },
    { 0x72cb6b06, 256, 256, test_text_baseline, "text_baseline" },
    { 0x4d41daa2, 256, 256, test_font, "font" },
    { 0x70e3232d, 256, 256, test_fill_text, "fill_text" },
    { 0xed6477c8, 256, 256, test_stroke_text, "stroke_text" },
    { 0x32d1ee3b, 256, 256, test_measure_text, "measure_text" },
    { 0x2c895307, 256, 256, test_text_kerning, "text_kerning" },
    { 0xd9969c18, 256, 256, test_text_kerning_measure, "text_kerning_measure" },
    { 0x2c895307, 256, 256, test_text_kerning_aat, "text_kerning_aat" },
    { 0xd9969c18, 256, 256, test_text_kerning_aat_measure, "text_kerning_aat_measure" },
    { 0x2c895307, 256, 256, test_text_kerning_gpos_fmt1, "text_kerning_gpos_fmt1" },
    { 0x2c895307, 256, 256, test_text_kerning_gpos_fmt2, "text_kerning_gpos_fmt2" },
    { 0x78cb460c, 256, 256, test_draw_image, "draw_image" },
    { 0xb530077b, 256, 256, draw_image_matted, "draw_image_matted" },
    { 0xaf04e7a2, 256, 256, test_get_image_data, "get_image_data" },
    { 0x5acae0b6, 256, 256, test_put_image_data, "put_image_data" },
    { 0xb6e854b1, 256, 256, test_save_restore, "save_restore" },
    { 0x62bc9606, 256, 256, example_button, "example_button" },
    { 0x92731a7b, 256, 256, example_smiley, "example_smiley" },
    { 0xe2f1e1de, 256, 256, example_knot, "example_knot" },
    { 0xc02d01ea, 256, 256, example_icon, "example_icon" },
    { 0xa1607c4a, 256, 256, example_illusion, "example_illusion" },
    { 0x7c861f87, 256, 256, example_star, "example_star" },
    { 0x429ca194, 256, 256, example_neon, "example_neon" }
};

static int glob_match(char const *pattern, char const *name)
{
    if (!*pattern && !*name)
        return 1;
    if (*pattern == '*')
        return glob_match(pattern + 1, name) ||
            (*name && glob_match(pattern, name + 1));
    if (*pattern == '?')
        return *name && glob_match(pattern + 1, name + 1);
    return *name == *pattern && glob_match(pattern + 1, name + 1);
}

static void base64_decode(char const *input, ci_byte_buf_t *output)
{
    size_t len = strlen(input);
    size_t cap = (len * 3) / 4 + 4;
    int idx = 0;
    int data = 0;
    int held = 0;
    int symbol;
    output->data = (unsigned char *)malloc(cap);
    if (!output->data) { output->size = 0; return; }
    output->size = 0;
    while ((symbol = input[idx++]) != 0)
    {
        int value;
        if (symbol == '=')
            break;
        value = ('A' <= symbol && symbol <= 'Z' ? symbol - 'A' :
                 'a' <= symbol && symbol <= 'z' ? symbol - 'a' + 26 :
                 '0' <= symbol && symbol <= '9' ? symbol - '0' + 52 :
                 symbol == '+' ? 62 :
                 symbol == '/' ? 63 :
                 0);
        data = data << 6 | value;
        held += 6;
        if (held >= 8)
        {
            held -= 8;
            output->data[output->size++] = (unsigned char)((data >> held) & 0xff);
            data &= (1 << held) - 1;
        }
    }
}

static double get_seconds(void)
{
#if defined( __linux__ )
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (double)(now.tv_sec) + (double)(now.tv_nsec) * 1.0e-9;
#elif defined( _WIN32 )
    static double rate = 0.0;
    if (rate == 0.0)
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        rate = 1.0 / frequency.QuadPart;
    }
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return now.QuadPart * rate;
    }
#elif defined( __MACH__ )
    static double rate = 0.0;
    if (rate == 0.0)
    {
        mach_timebase_info_data_t frequency;
        mach_timebase_info(&frequency);
        rate = frequency.numer * 1.0e-9 / frequency.denom;
    }
    return mach_absolute_time() * rate;
#else
    struct timeval now;
    gettimeofday(&now, 0);
    return now.tv_sec + now.tv_usec * 1.0e-6;
#endif
}

static unsigned hash_image(
    unsigned char const *image,
    int width,
    int height)
{
    unsigned hash = 0;
    unsigned state = ~0u;
    int y, x, channel;
    for (y = 0; y < height; ++y)
        for (x = 0; x < width; ++x)
            for (channel = 0; channel < 4; ++channel)
            {
                int next_x = (x + 1) % width;
                int next_y = (y + 1) % height;
                int current = image[(y * width + x) * 4 + channel];
                int down = image[(next_y * width + x) * 4 + channel];
                int right_val = image[(y * width + next_x) * 4 + channel];
                int threshold = 8;
                unsigned edges;
                unsigned roll;
                if (channel < 3)
                {
                    current *= image[(y * width + x) * 4 + 3];
                    down *= image[(next_y * width + x) * 4 + 3];
                    right_val *= image[(y * width + next_x) * 4 + 3];
                    threshold *= 255;
                }
                edges =
                    ((unsigned)(current - down  > threshold * 16) * 128u) |
                    ((unsigned)(current - down  > threshold)      *  64u) |
                    ((unsigned)(down - current  > threshold * 16) *  32u) |
                    ((unsigned)(down - current  > threshold)      *  16u) |
                    ((unsigned)(current - right_val > threshold * 16) *   8u) |
                    ((unsigned)(current - right_val > threshold)      *   4u) |
                    ((unsigned)(right_val - current > threshold * 16) *   2u) |
                    ((unsigned)(right_val - current > threshold)      *   1u);
                state ^= (state & 0x7ffffu) << 13;
                state ^= state >> 17;
                state ^= (state & 0x7ffffffu) << 5;
                roll = state >> 27;
                if (roll)
                    edges = (edges & (~0u >> roll)) << roll |
                        edges >> (32 - roll);
                hash ^= edges;
            }
    return hash;
}

static void write_png(
    char const *filename,
    unsigned char const *image,
    int width,
    int height)
{
    FILE *fp;
    unsigned table[256];
    unsigned idx;
    int idat_size;
    unsigned char header[56];
    unsigned crc;
    int check_1, check_2, row_size, y;
    unsigned char footer[20];

    fp = fopen(filename, "wb");
    if (!fp) return;

    for (idx = 0; idx < 256; ++idx)
    {
        unsigned value = idx;
        int step;
        for (step = 0; step < 8; ++step)
            value = (value & 1 ? 0xedb88320u : 0u) ^ (value >> 1);
        table[idx] = value;
    }
    idat_size = 6 + height * (6 + width * 4);
    /* Signature */
    header[0]=137; header[1]=80; header[2]=78; header[3]=71;
    header[4]=13; header[5]=10; header[6]=26; header[7]=10;
    /* IHDR chunk length */
    header[8]=0; header[9]=0; header[10]=0; header[11]=13;
    header[12]=73; header[13]=72; header[14]=68; header[15]=82;
    header[16]=(unsigned char)(width >> 24);
    header[17]=(unsigned char)(width >> 16);
    header[18]=(unsigned char)(width >> 8);
    header[19]=(unsigned char)(width >> 0);
    header[20]=(unsigned char)(height >> 24);
    header[21]=(unsigned char)(height >> 16);
    header[22]=(unsigned char)(height >> 8);
    header[23]=(unsigned char)(height >> 0);
    header[24]=8; header[25]=6; header[26]=0; header[27]=0; header[28]=0;
    header[29]=0; header[30]=0; header[31]=0; header[32]=0;
    /* sRGB */
    header[33]=0; header[34]=0; header[35]=0; header[36]=1;
    header[37]=115; header[38]=82; header[39]=71; header[40]=66;
    header[41]=0;
    header[42]=174; header[43]=206; header[44]=28; header[45]=233;
    /* IDAT */
    header[46]=(unsigned char)(idat_size >> 24);
    header[47]=(unsigned char)(idat_size >> 16);
    header[48]=(unsigned char)(idat_size >> 8);
    header[49]=(unsigned char)(idat_size >> 0);
    header[50]=73; header[51]=68; header[52]=65; header[53]=84;
    header[54]=120; header[55]=1;

    crc = ~0u;
    {
        int i;
        for (i = 12; i < 29; ++i)
            crc = table[(crc ^ header[i]) & 0xff] ^ (crc >> 8);
    }
    header[29] = (unsigned char)(~crc >> 24);
    header[30] = (unsigned char)(~crc >> 16);
    header[31] = (unsigned char)(~crc >> 8);
    header[32] = (unsigned char)(~crc >> 0);
    fwrite(header, 1, 56, fp);

    crc = ~0u;
    {
        int i;
        for (i = 50; i < 56; ++i)
            crc = table[(crc ^ header[i]) & 0xff] ^ (crc >> 8);
    }
    check_1 = 1;
    check_2 = 0;
    row_size = 1 + width * 4;
    for (y = 0; y < height; ++y, image += width * 4)
    {
        unsigned char prefix[6];
        int i;
        prefix[0] = (unsigned char)(y + 1 == height);
        prefix[1] = (unsigned char)((row_size >> 0));
        prefix[2] = (unsigned char)((row_size >> 8));
        prefix[3] = (unsigned char)(~(row_size >> 0));
        prefix[4] = (unsigned char)(~(row_size >> 8));
        prefix[5] = 0;
        fwrite(prefix, 1, 6, fp);
        for (i = 0; i < 6; ++i)
            crc = table[(crc ^ prefix[i]) & 0xff] ^ (crc >> 8);
        fwrite(image, 1, (size_t)(width * 4), fp);
        check_2 = (check_2 + check_1) % 65521;
        for (i = 0; i < width * 4; ++i)
        {
            check_1 = (check_1 + image[i]) % 65521;
            check_2 = (check_2 + check_1) % 65521;
            crc = table[(crc ^ image[i]) & 0xff] ^ (crc >> 8);
        }
    }
    footer[0] = (unsigned char)(check_2 >> 8);
    footer[1] = (unsigned char)(check_2 >> 0);
    footer[2] = (unsigned char)(check_1 >> 8);
    footer[3] = (unsigned char)(check_1 >> 0);
    footer[4]=0; footer[5]=0; footer[6]=0; footer[7]=0;
    footer[8]=0; footer[9]=0; footer[10]=0; footer[11]=0;
    footer[12]=73; footer[13]=69; footer[14]=78; footer[15]=68;
    footer[16]=174; footer[17]=66; footer[18]=96; footer[19]=130;
    {
        int i;
        for (i = 0; i < 4; ++i)
            crc = table[(crc ^ footer[i]) & 0xff] ^ (crc >> 8);
    }
    footer[4] = (unsigned char)(~crc >> 24);
    footer[5] = (unsigned char)(~crc >> 16);
    footer[6] = (unsigned char)(~crc >> 8);
    footer[7] = (unsigned char)(~crc >> 0);
    fwrite(footer, 1, 20, fp);
    fclose(fp);
}

/* ======== MAIN ======== */

int main(int argc, char **argv)
{
    char const *subset = "";
    int plain = 0;
    int do_table = 0;
    int pngs = 0;
    char const *suffix = "";
    int fails = 0;
    int bench = 1;
    int count, total, failed, done, index;
    double geo;
    char png_name[512];

    for (index = 1; index < argc; ++index)
    {
        char const *option = argv[index];
        if (strcmp(option, "--subset") == 0 && index < argc - 1)
            subset = argv[++index];
        else if (strcmp(option, "--plain") == 0)
            plain = 1;
        else if (strcmp(option, "--table") == 0)
            do_table = 1;
        else if (strcmp(option, "--pngs") == 0)
            pngs = 1;
        else if (strcmp(option, "--suffix") == 0 && index < argc - 1)
            suffix = argv[++index];
        else if (strcmp(option, "--fails") == 0)
            fails = 1;
        else if (strcmp(option, "--bench") == 0 && index < argc - 1)
        {
            int val = atoi(argv[++index]);
            bench = val > 1 ? val : 1;
        }
        else
        {
            printf("Usage: %s [options...]\n"
                   "Options:\n"
                   "  --subset <str> : Only run tests with names globbing <str>\n"
                   "  --plain        : Do not colorize output or use term codes\n"
                   "  --table        : Regenerate the code for table of tests\n"
                   "  --pngs         : Write PNG images showing output of tests\n"
                   "  --suffix <str> : Append <str> to the filenames of PNGs\n"
                   "  --fails        : Generate output only for failures\n"
                   "  --bench <int>  : Run each test <int> times, show fastest\n",
                   argv[0]);
            return 1;
        }
    }
#ifdef _WIN32
    {
        HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (out == INVALID_HANDLE_VALUE ||
            !GetConsoleMode(out, &mode) ||
            !SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            plain = 1;
    }
#else
    if (!isatty(fileno(stdout)))
        plain = 1;
#endif
    count = (int)(sizeof(tests) / sizeof(tests[0]));
    total = count;
    if (subset[0] != '\0')
    {
        total = 0;
        for (index = 0; index < count; ++index)
            if (glob_match(subset, tests[index].name))
                ++total;
    }
    base64_decode(font_a_base64, &font_a);
    base64_decode(font_b_base64, &font_b);
    base64_decode(font_c_base64, &font_c);
    base64_decode(font_d_base64, &font_d);
    base64_decode(font_e_base64, &font_e);
    base64_decode(font_f_base64, &font_f);
    base64_decode(font_g_base64, &font_g);
    failed = 0;
    done = 0;
    geo = 0.0;
    for (index = 0; index < count; ++index)
    {
        test_entry const *entry = &tests[index];
        int w, h, run, distance, passed;
        unsigned char *image;
        double best;
        unsigned hash;
        unsigned bits;
        if (subset[0] != '\0' && !glob_match(subset, entry->name))
            continue;
        ++done;
        if (!fails && !do_table && !plain)
            printf("\33[90m%3d/%d \33[33m[RUN ] \33[0;90m???????? ?????.??ms\33[m %s\n",
                   done, total, entry->name);
        w = entry->width;
        h = entry->height;
        image = (unsigned char *)malloc((size_t)(4 * w * h));
        if (!image)
        {
            fprintf(stderr, "Failed to allocate image for test %s\n", entry->name);
            continue;
        }
        best = 1.0e100;
        for (run = 0; run < bench; ++run)
        {
            ci_canvas_t *subject = ci_canvas_create(w, h);
            double start_time, end_time, elapsed;
            if (!subject)
            {
                fprintf(stderr, "Failed to create canvas for test %s\n", entry->name);
                break;
            }
            start_time = get_seconds();
            entry->call(subject, (float)(w), (float)(h));
            end_time = get_seconds();
            elapsed = end_time - start_time;
            if (elapsed < best) best = elapsed;
            if (run == 0)
                ci_canvas_get_image_data(subject, image, w, h, 4 * w, 0, 0);
            ci_canvas_destroy(subject);
        }
        geo += log(best);
        hash = hash_image(image, w, h);
        distance = 0;
        for (bits = hash ^ entry->hash; bits; bits &= bits - 1)
            ++distance;
        passed = distance <= 5;
        if (!passed)
            ++failed;
        else if (fails)
        {
            free(image);
            continue;
        }
        if (do_table)
            printf("    { 0x%08x, %d, %d, %s, \"%s\" },\n",
                   hash, w, h, entry->name, entry->name);
        else if (plain)
            printf("%3d/%d [%s] %08x %8.2fms %s\n",
                   done, total, (passed ? "PASS" : "FAIL"),
                   hash, best * 1000.0, entry->name);
        else
            printf("%s\33[90m%3d/%d \33[%sm %08x %8.2fms\33[m %s\n",
                   (fails ? "" : "\33[A"),
                   done, total,
                   (passed ? "32m[PASS]\33[0;90m" : "31;1m[FAIL]\33[0;90m"),
                   hash, best * 1000.0, entry->name);
        if (pngs)
        {
            sprintf(png_name, "%s%s.png", entry->name, suffix);
            write_png(png_name, image, w, h);
        }
        free(image);
    }
    geo = done ? exp(geo / done) : 0.0;
    if (!do_table && (!fails || failed))
        printf("%d failed, %.3fms geo mean\n", failed, geo * 1000.0);

    free(font_a.data);
    free(font_b.data);
    free(font_c.data);
    free(font_d.data);
    free(font_e.data);
    free(font_f.data);
    free(font_g.data);

    return !!failed;
}
