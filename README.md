# canvas_ity_c89

[![Tests](https://github.com/Keyframe/canvas_ity_c89/actions/workflows/tests.yml/badge.svg)](https://github.com/Keyframe/canvas_ity_c89/actions/workflows/tests.yml)

A [single-header C89 library](src/canvas_ity.h) for rasterizing
immediate-mode 2D vector graphics, closely modeled on the
[W3C HTML5 2D canvas specification](https://www.w3.org/TR/2015/REC-2dcontext-20151119/).

This is a C89 rework of Andrew Kensler's original C++ library
[canvas_ity](https://github.com/a-e-k/canvas_ity). All C++ has been
removed. The library compiles with `gcc -std=c89 -Wall -Wextra -Wpedantic
-Werror` and produces output identical to the original.

The priorities for this library are high-quality rendering, ease of use, and
compact size.  Speed is important too, but secondary to the other priorities.
The library takes an opinionated approach and does not provide options for
trading off quality for speed.

Despite its small size, it supports nearly everything in the W3C HTML5 2D
canvas specification, except for hit regions and getting certain properties.
Stroke, fill, gradient, pattern, image, and font styles are specified through
C function calls rather than strings. The goal is that this library could
produce a conforming HTML5 2D canvas implementation if wrapped in a thin
layer of JavaScript bindings.

## Example

The following program builds a star path, fills and strokes it with drop
shadows, dashed lines, and a gradient shine overlay, then writes the result
to a TGA file.

<table>
<tr>
<th>canvas_ity (C89)</th>
<th>HTML5 canvas</th>
</tr>
<tr>
<td><img src=".github/example_canvas_ity.png" width="256" height="256"></td>
<td><img src=".github/example_html5.png" width="256" height="256"></td>
</tr>
<tr>
<td>
<sub>

```c
#include <stdio.h>
#include <stdlib.h>
#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

int main(void)
{
    static int const width = 256, height = 256;
    unsigned char header[] = { 0, 0, 2, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 1, 32, 40 };
    unsigned char *image;
    int pixel;
    float segments[] = {
        21.0f, 9.0f, 1.0f, 9.0f,
         7.0f, 9.0f, 1.0f, 9.0f };
    FILE *fp;
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

    /* Set up the drop shadow. */
    ci_canvas_set_shadow_blur(ctx, 8.0f);
    ctx->shadow_offset_y = 4.0f;
    ci_canvas_set_shadow_color(ctx, 0.0f, 0.0f, 0.0f, 0.5f);

    /* Fill the star with yellow. */
    ci_canvas_set_color(ctx, CI_FILL_STYLE,
        1.0f, 0.9f, 0.2f, 1.0f);
    ci_canvas_fill(ctx);

    /* Thick red stroke with rounded points. */
    ctx->line_join = CI_JOIN_ROUNDED;
    ci_canvas_set_line_width(ctx, 12.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE,
        0.9f, 0.0f, 0.5f, 1.0f);
    ci_canvas_stroke(ctx);

    /* Dashed thinner orange stroke. */
    ci_canvas_set_line_dash(ctx, segments, 8);
    ctx->line_dash_offset = 10.0f;
    ctx->line_cap = CI_CAP_CIRCLE;
    ci_canvas_set_line_width(ctx, 6.0f);
    ci_canvas_set_color(ctx, CI_STROKE_STYLE,
        0.95f, 0.65f, 0.15f, 1.0f);
    ci_canvas_stroke(ctx);

    /* Turn off the drop shadow. */
    ci_canvas_set_shadow_color(ctx,
        0.0f, 0.0f, 0.0f, 0.0f);

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
    ci_canvas_fill_rectangle(ctx,
        0.0f, 0.0f, 256.0f, 256.0f);

    /* Retrieve pixels and write a TGA file. */
    image = (unsigned char *)malloc(
        (size_t)(height * width * 4));
    ci_canvas_get_image_data(ctx,
        image, width, height, width * 4, 0, 0);
    for (pixel = 0; pixel < height * width; ++pixel) {
        unsigned char tmp = image[pixel * 4 + 0];
        image[pixel * 4 + 0] = image[pixel * 4 + 2];
        image[pixel * 4 + 2] = tmp;
    }
    fp = fopen("example.tga", "wb");
    fwrite(header, 1, sizeof(header), fp);
    fwrite(image, 1,
        (size_t)(height * width * 4), fp);
    fclose(fp);
    free(image);
    ci_canvas_destroy(ctx);
    return 0;
}
```

</sub>
</td>
<td>
<sub>

```html
<!DOCTYPE html>
<html>
<head>
  <title>Example</title>
</head>
<body>
  <canvas id="example" width="256" height="256">
  </canvas>
  <script type="text/javascript">
    const ctx =
      document.getElementById("example")
      .getContext("2d");

    // Build a star path.
    ctx.moveTo(128.0,  28.0);
    ctx.lineTo(157.0,  87.0);
    ctx.lineTo(223.0,  97.0);
    ctx.lineTo(175.0, 143.0);
    ctx.lineTo(186.0, 208.0);
    ctx.lineTo(128.0, 178.0);
    ctx.lineTo( 69.0, 208.0);
    ctx.lineTo( 80.0, 143.0);
    ctx.lineTo( 32.0,  97.0);
    ctx.lineTo( 98.0,  87.0);
    ctx.closePath();

    // Set up the drop shadow.
    ctx.shadowBlur = 8.0;
    ctx.shadowOffsetY = 4.0;
    ctx.shadowColor = "rgba(0,0,0,0.5)";

    // Fill the star with yellow.
    ctx.fillStyle = "#ffe633";
    ctx.fill();

    // Thick red stroke, rounded points.
    ctx.lineJoin = "round";
    ctx.lineWidth = 12.0;
    ctx.strokeStyle = "#e60080";
    ctx.stroke();

    // Dashed thinner orange stroke.
    const segs = [
      21.0, 9.0, 1.0, 9.0,
       7.0, 9.0, 1.0, 9.0];
    ctx.setLineDash(segs);
    ctx.lineDashOffset = 10.0;
    ctx.lineCap = "round";
    ctx.lineWidth = 6.0;
    ctx.strokeStyle = "#f2a626";
    ctx.stroke();

    // Turn off the drop shadow.
    ctx.shadowColor = "rgba(0,0,0,0.0)";

    // Add a shine layer over the star.
    let g = ctx.createLinearGradient(
      64.0, 0.0, 192.0, 256.0);
    g.addColorStop(0.30,
      "rgba(255,255,255,0.0)");
    g.addColorStop(0.35,
      "rgba(255,255,255,0.8)");
    g.addColorStop(0.45,
      "rgba(255,255,255,0.8)");
    g.addColorStop(0.50,
      "rgba(255,255,255,0.0)");
    ctx.fillStyle = g;
    ctx.globalCompositeOperation =
      "source-atop";
    ctx.fillRect(0.0, 0.0, 256.0, 256.0);
  </script>
</body>
</html>
```

</sub>
</td>
</tr>
</table>

## Features

### High-quality rendering

- Trapezoidal area antialiasing provides very smooth antialiasing, even when
    lines are nearly horizontal or vertical.
- Gamma-correct blending, interpolation, and resampling throughout.
    All colors are linearized and alpha-premultiplied on input and converted
    back to unpremultiplied sRGB on output.  This reduces muddiness on many
    gradients (e.g., red to green), makes line thicknesses more perceptually
    uniform, and avoids dark fringes when interpolating opacity.
- Bicubic convolution resampling for patterns and images.  Smoothly
    interpolates with less blockiness when magnifying, and antialiases well
    when minifying.  Can simultaneously magnify and minify along different axes.
- Ordered dithering on output reduces banding on subtle gradients while
    remaining compression-friendly.
- High curvature is handled carefully in line joins.  Thick lines are drawn
    correctly as though tracing with a wide pen nib, even where the lines
    curve sharply.

### Ease of use

- Single-header library with no dependencies beyond the C standard library.
    Nothing to link besides `-lm`.  Includes built-in binary parsing for
    TrueType font (TTF) files.  Pure CPU code, no GPU required.
- Compiles as strict C89 with `-Wall -Wextra -Wpedantic -Werror`.
- Shares no internal pointers, nor holds any external pointers.
- Uses no static or global variables.  Threads may safely work with
    different canvas instances concurrently without locking.
- Pluggable backend abstraction (`ci_backend_t`) allows swapping the
    CPU rasterizer for a GPU implementation via function pointers.

### Compact size

- The library source is roughly 3000 lines including comments.
- Object code can be less than 36 KiB on x86-64 with appropriate
    compiler settings for size.
- The accompanying automated test suite achieves 100% line coverage.

## Limitations

- Trapezoidal antialiasing overestimates coverage where paths self-intersect
    within a single pixel.  Where inner joins are visible, this can lead to
    a "grittier" appearance due to the extra windings used.
- Clipping uses an antialiased sparse pixel mask rather than geometrically
    intersecting paths.  Therefore, it is not subpixel-accurate.
- Text rendering is basic and mainly for convenience.  It only
    supports left-to-right text and does not do hinting, kerning,
    ligatures, text shaping, or text layout.
- **TrueType font parsing is not secure.** It does some basic validity
    checking, but should only be used with known-good or sanitized fonts.
- Parameter checking does not test for non-finite floating-point values.
- Rendering is single-threaded, not explicitly vectorized, and not
    GPU-accelerated.
- The library does no I/O on its own.  You provide it with buffers to
    copy into or out of.

## Usage

This is a [single-header library](src/canvas_ity.h).  Include it freely
in any of your C source files.  In exactly one file, define
```c
#define CANVAS_ITY_IMPLEMENTATION
```
before including the header to get the implementation.

Create a canvas with `ci_canvas_create()`, draw into it using the
`ci_canvas_*` functions, retrieve pixels with `ci_canvas_get_image_data()`,
and clean up with `ci_canvas_destroy()`.

See the [automated test suite](test/test.c) for examples of every
public API function.

### Building

```
make          # build the test runner
make test     # build and run all 76 tests
make clean    # remove build artifacts
make lint     # C89 syntax check on the header
make valgrind # run under valgrind
make sanitize # build and run with ASan/UBSan/LSan
```

Or compile directly:
```
gcc -std=c89 -O2 -I src -o test_runner test/test.c -lm
```

## Acknowledgments

This library is a C89 rework of the original C++ [canvas_ity](https://github.com/a-e-k/canvas_ity)
by [Andrew Kensler](https://github.com/a-e-k).  The rendering algorithms,
architecture, and test suite design are his work.  This fork ports
everything to strict C89, adds a backend abstraction layer, and replaces
CMake with a plain Makefile.

## License

[ISC](LICENSE.txt) -- same as the original.
