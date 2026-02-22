# Makefile for nanovg + canvas_ity integration
#
# Targets:
#   all           Build example and tests
#   test          Build and run the test suite
#   example       Build the example demo
#   screenshot    Build and run the example (generates screenshot.png)
#   clean         Remove build artifacts
#
# Variables:
#   CC        C compiler            (default: gcc)
#   CFLAGS    Additional compiler flags
#   V         Verbose build output  (set to 1)

CC       ?= gcc
CSTD     := -std=c89
CWARN    := -Wall -Wextra -Wpedantic -Werror
COPT     ?= -O2
CINC     := -I../src
LDLIBS   := -lm

ALL_CFLAGS  := $(CSTD) $(CWARN) $(COPT) $(CINC) -DNVG_NO_STB $(CFLAGS)
ALL_LDFLAGS := $(LDFLAGS)

BUILDDIR := build

# Quiet by default, V=1 for verbose
ifndef V
Q    := @
ECHO := @echo
else
Q    :=
ECHO := @echo
endif

.PHONY: all test example screenshot clean

all: $(BUILDDIR)/example $(BUILDDIR)/test_nanovg

$(BUILDDIR):
	$(Q)mkdir -p $(BUILDDIR)

$(BUILDDIR)/example: example.c nanovg_ci.h nanovg.h nanovg.c fontstash.h stb_truetype.h stb_image_write.h ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ example.c $(ALL_LDFLAGS) $(LDLIBS)

$(BUILDDIR)/test_nanovg: test_nanovg.c nanovg_ci.h nanovg.h nanovg.c fontstash.h stb_truetype.h ../src/canvas_ity.h | $(BUILDDIR)
	$(ECHO) "  CC    $@"
	$(Q)$(CC) $(ALL_CFLAGS) -o $@ test_nanovg.c $(ALL_LDFLAGS) $(LDLIBS)

example: $(BUILDDIR)/example

test: $(BUILDDIR)/test_nanovg
	$(ECHO) "  TEST  $(BUILDDIR)/test_nanovg"
	$(Q)$(BUILDDIR)/test_nanovg

screenshot: $(BUILDDIR)/example
	$(ECHO) "  RUN   $(BUILDDIR)/example"
	$(Q)cd $(BUILDDIR) && ./example
	$(Q)mv -f $(BUILDDIR)/screenshot.png . 2>/dev/null || true
	$(ECHO) "  =>    screenshot.png"

sanitize: clean
	$(Q)$(MAKE) --no-print-directory test \
		COPT="-O1 -g" \
		CFLAGS="-fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer" \
		LDFLAGS="-fsanitize=undefined -fsanitize=address"

clean:
	$(ECHO) "  CLEAN"
	$(Q)rm -rf $(BUILDDIR) screenshot.png
