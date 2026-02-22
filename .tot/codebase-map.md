# canvas-ity-c89 — Codebase Map

## Language

C (C89 target)

## Directory Structure

```
canvas_ity/
├── demos/
│   └── tiger/
│       ├── CMakeLists.txt
│       └── tiger.cpp
├── .github/
│   ├── example_canvas_ity.png
│   ├── example_html5.png
│   ├── icon.png
│   ├── README.md
│   └── workflows/
│       └── tests.yml
├── LICENSE.txt
├── src/
│   └── canvas_ity.hpp
└── test/
    ├── CMakeLists.txt
    ├── test.cpp
    └── test.html
```

## Key Files

- `src/canvas_ity.hpp` — single-header library (3469 lines). Public API + implementation behind `CANVAS_ITY_IMPLEMENTATION` guard.
- `test/test.cpp` — comprehensive test suite with hash-based image comparison. 100% line coverage.
- `test/test.html` — HTML5 canvas port of test suite for browser comparison.
- `demos/tiger/tiger.cpp` — PostScript tiger rendering demo/benchmark.
- `test/CMakeLists.txt` — CMake build with extensive warnings, sanitizers, coverage, static analysis targets.

## Build System

- **Build command**: `make`
- **Test command**: `make test`
- **Lint command**: `gcc -std=c89 -Wall -Wextra -Wpedantic -Werror -fsyntax-only src/canvas_ity.h`

_Update this file as the codebase evolves._
