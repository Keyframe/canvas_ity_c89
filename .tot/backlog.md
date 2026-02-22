# canvas-ity-c89 — Work Item Backlog

## Format

```
- [ ] WI-NNN [type] (priority) Description
  Scope: dir1/,dir2/
  Criteria: testable acceptance criteria
```

Types: `bug`, `feature`, `refactor`, `chore`
Priorities: `P0` (critical), `P1` (high), `P2` (medium), `P3` (low)

---

## Backlog

### Phase 1: C89 Port

- [x] WI-001 [refactor] (P0) Port canvas_ity.hpp to strict C89 single-header .h
  Scope: src/
  Criteria: Compiles with gcc -std=c89 -Wall -Wextra -Wpedantic -Werror; all C++ removed (no namespaces, classes, templates, std::vector, operator overloads); single-header with implementation guard; functionally equivalent output to original C++ version
  Dependencies: none

- [ ] WI-002 [refactor] (P0) Add backend abstraction layer
  Scope: src/
  Criteria: CPU rasterizer behind function pointer/vtable abstraction; clear interface for future Vulkan/GPU swap-in; no functional regression
  Dependencies: WI-001

- [x] WI-003 [refactor] (P0) Port test suite to C89
  Scope: test/
  Criteria: test.c compiles with gcc -std=c89; all original test cases preserved; hash-based comparison works; all tests pass
  Dependencies: WI-001

- [x] WI-004 [chore] (P0) Replace CMake with Makefile
  Scope: Makefile
  Criteria: make, make test, make clean all work; sanitizer support; no CMake dependency
  Dependencies: WI-001, WI-003

### Phase 2: WHATWG Canvas Extensions

- [ ] WI-005 [feature] (P1) Add alternate fill rules (evenodd)
  Scope: src/
  Criteria: evenodd fill rule implemented per WHATWG spec; tests added and pass
  Dependencies: WI-004

- [ ] WI-006 [feature] (P1) Add conic gradients
  Scope: src/
  Criteria: createConicGradient equivalent implemented per WHATWG spec; tests added and pass
  Dependencies: WI-004

- [ ] WI-007 [feature] (P1) Add elliptical arcs
  Scope: src/
  Criteria: ellipse() path method per WHATWG spec; tests added and pass
  Dependencies: WI-004

- [ ] WI-008 [feature] (P1) Add round rectangles
  Scope: src/
  Criteria: roundRect() equivalent per WHATWG spec; tests added and pass
  Dependencies: WI-004

- [ ] WI-009 [feature] (P1) Add text kerning
  Scope: src/
  Criteria: TTF kern table parsed and applied; measurable improvement in text spacing; tests added and pass
  Dependencies: WI-004

### Phase 3: NanoVG Backend

- [ ] WI-010 [feature] (P2) Implement canvas_ity_c89 as NanoVG rendering backend
  Scope: src/
  Criteria: canvas_ity_c89 conforms to NanoVG NVGparams backend interface; NanoVG examples render correctly through our CPU rasterizer; future NanoVG fork
  Dependencies: WI-002, WI-004

---

## Notes

- Work items are worked in priority order unless dependencies dictate otherwise.
- The AI agent may propose new WIs marked `[proposed]` — these require human approval.
- To add a WI: edit this file or use `make -f Makefile.tot tot-add-wi` for format reference.
