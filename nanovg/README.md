# NanoVG + canvas_ity Integration

NanoVG's 2D vector API backed by canvas_ity's trapezoidal coverage rasterizer.
Optional NanoSVG parser included for SVG support.

## Architecture

```
NanoSVG (SVG parser, OPTIONAL)
    |
    v
NanoVG API (nanovg.h/nanovg.c)     <-- user-facing 2D vector API
    |  NVGparams callbacks
    v
nanovg_ci.h                         <-- bridge layer
    |  ci_canvas_* API calls
    v
canvas_ity.h                        <-- rasterizer
    |  ci_backend_t vtable
    v
CPU backend (now) / GPU (future)
```

## Quick Start

```c
#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.h"

#include "nanovg.h"
#include "nanovg.c"

#define NANOVG_CI_IMPLEMENTATION
#include "nanovg_ci.h"

int main(void) {
    NVGcontext *vg = nvgCreateCI(800, 600, NVG_ANTIALIAS);

    nvgBeginFrame(vg, 800, 600, 1.0f);

    nvgBeginPath(vg);
    nvgRoundedRect(vg, 10, 10, 200, 50, 8);
    nvgFillColor(vg, nvgRGBA(80, 140, 240, 255));
    nvgFill(vg);

    nvgEndFrame(vg);

    /* Read pixels */
    unsigned char pixels[800 * 600 * 4];
    nvgciReadPixels(vg, pixels, 800, 600, 800 * 4);

    nvgDeleteCI(vg);
    return 0;
}
```

## Building

```
make            # build example and tests
make test       # build and run test suite
make screenshot # generate screenshot.png
make clean      # remove build artifacts
```

Or compile directly:
```
gcc -std=c89 -O2 -I../src -DNVG_NO_STB -o example example.c -lm
```

## API

### Backend Functions

| Function | Description |
|----------|-------------|
| `nvgCreateCI(w, h, flags)` | Create NanoVG context with canvas_ity backend |
| `nvgDeleteCI(ctx)` | Destroy context and free resources |
| `nvgciReadPixels(ctx, dst, w, h, stride)` | Copy rendered pixels to buffer |

### NVGcolor Access (C89)

The original NVGcolor uses an anonymous struct inside a union (C11).
The C89 port names the members:

```c
NVGcolor c = nvgRGBA(255, 0, 0, 255);
float r = c.u.col.r;       /* or NVG_COLOR_R(c) */
float g = c.u.col.g;       /* or NVG_COLOR_G(c) */
c.u.rgba[0] = 1.0f;        /* array access */
```

## Files

| File | Description |
|------|-------------|
| `nanovg_ci.h` | Bridge layer (our code) |
| `example.c` | Demo rendering shapes + gradients to PNG |
| `test_nanovg.c` | 11 hash-based regression tests |
| `nanovg.h` | NanoVG header (vendored, C89-ported) |
| `nanovg.c` | NanoVG implementation (vendored, C89-ported) |
| `fontstash.h` | Font atlas (vendored, C89-ported) |
| `stb_truetype.h` | TTF parser (vendored, C89-ported) |
| `stb_image_write.h` | PNG writer (vendored, C89-ported) |
| `nanosvg.h` | SVG parser (vendored, C89-ported, optional) |
| `nanosvgrast.h` | SVG rasterizer (vendored, C89-ported, optional) |

## Paint Mapping

| NanoVG Paint | canvas_ity Mapping |
|-------------|-------------------|
| Solid color | `ci_canvas_set_color()` (zero overhead) |
| Linear gradient | Pre-rendered SDF buffer via `ci_canvas_set_pattern()` |
| Radial gradient | Pre-rendered SDF buffer via `ci_canvas_set_pattern()` |
| Box gradient | Pre-rendered SDF buffer via `ci_canvas_set_pattern()` |
| Image pattern | Stored texture via `ci_canvas_set_pattern()` |

## License

Bridge code (`nanovg_ci.h`, `example.c`, `test_nanovg.c`): ISC (same as canvas_ity).
NanoVG/fontstash: zlib license (Mikko Mononen).
NanoSVG: zlib license (Mikko Mononen).
stb libraries: MIT/public domain (Sean Barrett).
