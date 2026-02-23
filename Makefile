# Makefile for canvas_ity — ISC license
# Copyright (c) 2022 Andrew Kensler
#
# Targets:
#   all / test_runner   Build the test program
#   test                Build and run the test suite
#   clean               Remove build artifacts
#   valgrind            Run under valgrind
#   analyze             Run clang static analyzer
#   size                Measure compiled code size
#   sanitize            Build and run with sanitizers
#
# Variables:
#   CC        C compiler                    (default: gcc)
#   CFLAGS    Additional compiler flags
#   SANITIZE  Enable sanitizers             (set to 1)
#   V         Verbose build output          (set to 1)

CC       ?= gcc
CSTD     := -std=c89
CWARN    := -Wall -Wextra -Wpedantic -Werror
COPT     ?= -O2
CINC     := -I src
LDLIBS   := -lm

SRCDIR   := src
TESTDIR  := test
BUILDDIR := build

HEADER   := $(SRCDIR)/canvas_ity.h
TESTSRC  := $(TESTDIR)/test.c
TESTBIN  := $(BUILDDIR)/test_runner

# Sanitizer flags
ifdef SANITIZE
  SAN_CFLAGS  := -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
  SAN_LDFLAGS := -fsanitize=undefined -fsanitize=address
  # GCC and non-Apple Clang support leak sanitizer
  SAN_LEAK := $(shell $(CC) -fsanitize=leak -x c -c /dev/null -o /dev/null 2>/dev/null && echo -fsanitize=leak)
  SAN_CFLAGS  += $(SAN_LEAK)
  SAN_LDFLAGS += $(SAN_LEAK)
  COPT := -O1 -g
endif

ALL_CFLAGS := $(CSTD) $(CWARN) $(COPT) $(CINC) $(SAN_CFLAGS) $(CFLAGS)
ALL_LDFLAGS := $(SAN_LDFLAGS) $(LDFLAGS)

# Quiet by default
$(V)Q := @
$(V)ECHO := @echo

.PHONY: all test clean valgrind analyze size sanitize lint wasm

all: $(TESTBIN)

$(BUILDDIR):
	$(Q)mkdir -p $(BUILDDIR)

$(TESTBIN): $(TESTSRC) $(HEADER) | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ $(TESTSRC) $(ALL_LDFLAGS) $(LDLIBS)

test: $(TESTBIN)
	$(ECHO) "  TEST  $(TESTBIN)"
	$(Q)$(TESTBIN)

clean:
	$(ECHO) "  CLEAN"
	$(Q)rm -rf $(BUILDDIR)

lint:
	$(ECHO) "  LINT  $(HEADER)"
	$(Q)$(CC) $(CSTD) $(CWARN) -fsyntax-only $(HEADER)

sanitize: clean
	$(Q)$(MAKE) --no-print-directory test SANITIZE=1

valgrind: $(TESTBIN)
	$(ECHO) "  VALGRIND"
	$(Q)valgrind --tool=memcheck --leak-check=full --error-exitcode=1 $(TESTBIN)

analyze:
	$(ECHO) "  ANALYZE"
	$(Q)clang --analyze -Xanalyzer -analyzer-output=text $(CSTD) $(CINC) $(TESTSRC)

WASM_SRC  := $(TESTDIR)/canvas_ity_wasm.c
WASM_OUT  := $(TESTDIR)/canvas_ity.js

WASM_EXPORTS := '[\
	"_ci_canvas_create","_ci_canvas_destroy",\
	"_ci_canvas_scale","_ci_canvas_rotate","_ci_canvas_translate",\
	"_ci_canvas_transform","_ci_canvas_set_transform",\
	"_ci_canvas_set_global_alpha",\
	"_ci_canvas_set_shadow_color","_ci_canvas_set_shadow_blur",\
	"_ci_canvas_set_line_width","_ci_canvas_set_miter_limit",\
	"_ci_canvas_set_line_dash",\
	"_ci_canvas_set_color","_ci_canvas_set_linear_gradient",\
	"_ci_canvas_set_radial_gradient","_ci_canvas_set_conic_gradient",\
	"_ci_canvas_add_color_stop","_ci_canvas_set_pattern",\
	"_ci_canvas_begin_path","_ci_canvas_move_to","_ci_canvas_close_path",\
	"_ci_canvas_line_to","_ci_canvas_quadratic_curve_to",\
	"_ci_canvas_bezier_curve_to","_ci_canvas_arc_to",\
	"_ci_canvas_arc","_ci_canvas_ellipse",\
	"_ci_canvas_rectangle","_ci_canvas_round_rectangle",\
	"_ci_canvas_set_fill_rule","_ci_canvas_fill","_ci_canvas_stroke",\
	"_ci_canvas_clip","_ci_canvas_is_point_in_path",\
	"_ci_canvas_clear_rectangle","_ci_canvas_fill_rectangle",\
	"_ci_canvas_stroke_rectangle",\
	"_ci_canvas_set_font","_ci_canvas_fill_text",\
	"_ci_canvas_stroke_text","_ci_canvas_measure_text",\
	"_ci_canvas_draw_image","_ci_canvas_get_image_data",\
	"_ci_canvas_put_image_data",\
	"_ci_canvas_save","_ci_canvas_restore",\
	"_ci_wasm_set_line_cap","_ci_wasm_set_line_join",\
	"_ci_wasm_set_shadow_offset_x","_ci_wasm_set_shadow_offset_y",\
	"_ci_wasm_set_line_dash_offset","_ci_wasm_set_composite_op",\
	"_ci_wasm_set_text_align","_ci_wasm_set_text_baseline",\
	"_ci_wasm_character_to_glyph",\
	"_malloc","_free"]'

wasm: $(WASM_OUT)

$(WASM_OUT): $(WASM_SRC) $(HEADER)
	$(ECHO) "  EMCC  $@"
	$(Q)emcc -std=c89 -O2 -I src \
		-s WASM=1 \
		-s EXPORTED_FUNCTIONS=$(WASM_EXPORTS) \
		-s EXPORTED_RUNTIME_METHODS='["cwrap","HEAPU8","HEAPF32"]' \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s MODULARIZE=1 \
		-s EXPORT_NAME=CanvasIty \
		-o $@ \
		$(WASM_SRC) $(LDLIBS)

size:
	$(ECHO) "  SIZE"
	$(Q)$(CC) -Os -fno-math-errno -fno-asynchronous-unwind-tables \
		-fmerge-all-constants -ffunction-sections -fdata-sections \
		-c -o $(BUILDDIR)/canvas_ity.o -x c $(CSTD) \
		-DCANVAS_ITY_IMPLEMENTATION $(HEADER)
	$(Q)size $(BUILDDIR)/canvas_ity.o
