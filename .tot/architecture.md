# canvas-ity-c89 — Architecture

## Overview

Single-header C++ 2D rasterizer library (to be ported to C89) with W3C HTML5 canvas-like API. ~3469 lines in a single file (`src/canvas_ity.hpp`, target: `canvas_ity.h`).

Internal rendering pipeline:
1. **Path building** — polybezier construction (`move_to`, `line_to`, `bezier_curve_to`, etc.)
2. **Tessellation** — adaptive polybezier-to-polyline (`path_to_lines`)
3. **Dashing** — optional dash pattern application (`dash_lines`)
4. **Stroke expansion** — optional polyline widening with joins/caps (`stroke_lines`)
5. **Scan conversion** — sparse fractional pixel coverage (`lines_to_runs`)
6. **Shadow render** — optional offscreen blur + blend (`render_shadow`)
7. **Main render** — paint + blend onto canvas (`render_main`)

API sections: Lifecycle, Transforms, Compositing, Shadows, Line Styles, Fill/Stroke Styles, Path Building, Drawing Paths, Drawing Rectangles, Drawing Text, Drawing Images, Pixel Manipulation, Canvas State.

Key internals:
- Trapezoidal area antialiasing
- Gamma-correct (linear) blending throughout
- Bicubic convolution resampling for patterns/images
- Ordered dithering on output
- Built-in TrueType font (TTF) parser (NOT SECURE — known limitation)
- No dynamic alloc past high-water mark (flat std::vector in C++, to become dynamic arrays in C89)

Backend abstraction planned (WI-002) to allow future Vulkan/GPU swap-in.

## Module Map

Single file, single compilation unit. No module boundaries.

- `src/canvas_ity.hpp` — the library (target: `canvas_ity.h`)
- `test/test.cpp` — test harness (target: `test.c`)
- `demos/tiger/` — PostScript tiger rendering demo
- `.github/workflows/tests.yml` — CI (multi-platform + sanitizers)

## Forbidden Zones

These areas should NOT be modified without explicit human approval, regardless
of WI scope declarations:

_None identified._

## Technical Debt

- TTF font parser is explicitly noted as NOT SECURE. Basic validity checking only. Use known-good or sanitized fonts only.
- Text rendering is basic: left-to-right only, no hinting, no kerning (WI-009 addresses kerning), no ligatures, no text shaping, no text layout.

## Dependencies

- C++ standard library only (current): `<cstddef>`, `<vector>`, `<algorithm>`, `<cmath>`, `<numeric>`
- C89 target: C standard library only (`<stddef.h>`, `<stdlib.h>`, `<string.h>`, `<math.h>`)
