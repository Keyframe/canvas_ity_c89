# Makefile for canvas_ity examples
#
# Targets:
#   all               Build all examples
#   canvas_ity_example
#   nanovg_example
#   nanosvg_example
#   tiger_example
#   clean             Remove build artifacts

CC       ?= gcc
CSTD     := -std=c89
CWARN    := -Wall -Wextra -Wpedantic -Werror
COPT     ?= -O2
CINC     := -I../src
LDLIBS   := -lm

ALL_CFLAGS  := $(CSTD) $(CWARN) $(COPT) $(CINC) $(CFLAGS)
ALL_LDFLAGS := $(LDFLAGS)

BUILDDIR := build

ifndef V
Q    := @
ECHO := @echo
else
Q    :=
ECHO := @echo
endif

.PHONY: all clean

all: $(BUILDDIR)/canvas_ity_example $(BUILDDIR)/nanovg_example $(BUILDDIR)/nanosvg_example $(BUILDDIR)/tiger_example

$(BUILDDIR):
	$(Q)mkdir -p $(BUILDDIR)

$(BUILDDIR)/canvas_ity_example: canvas_ity_example.c ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ canvas_ity_example.c $(ALL_LDFLAGS) $(LDLIBS)

$(BUILDDIR)/nanovg_example: nanovg_example.c ../nanovg/nanovg_ci.h ../nanovg/nanovg.h ../nanovg/nanovg.c ../nanovg/fontstash.h ../nanovg/stb_truetype.h ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -DNVG_NO_STB -o $@ nanovg_example.c $(ALL_LDFLAGS) $(LDLIBS)

$(BUILDDIR)/nanosvg_example: nanosvg_example.c ../nanovg/nanosvg.h ../nanovg/nanosvgrast.h ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ nanosvg_example.c $(ALL_LDFLAGS) $(LDLIBS)

$(BUILDDIR)/tiger_example: tiger_example.c ../nanovg/nanosvg.h ../nanovg/nanosvgrast.h ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ tiger_example.c $(ALL_LDFLAGS) $(LDLIBS)

canvas_ity_example: $(BUILDDIR)/canvas_ity_example
nanovg_example: $(BUILDDIR)/nanovg_example
nanosvg_example: $(BUILDDIR)/nanosvg_example
tiger_example: $(BUILDDIR)/tiger_example

clean:
	$(ECHO) "  CLEAN"
	$(Q)rm -rf $(BUILDDIR) *.tga
