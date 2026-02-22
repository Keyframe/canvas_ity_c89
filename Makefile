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

.PHONY: all test clean valgrind analyze size sanitize lint

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

size:
	$(ECHO) "  SIZE"
	$(Q)$(CC) -Os -fno-math-errno -fno-asynchronous-unwind-tables \
		-fmerge-all-constants -ffunction-sections -fdata-sections \
		-c -o $(BUILDDIR)/canvas_ity.o -x c $(CSTD) \
		-DCANVAS_ITY_IMPLEMENTATION $(HEADER)
	$(Q)size $(BUILDDIR)/canvas_ity.o
