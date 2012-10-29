TARGET = tlmm-tests
OBJS = tests/arithmetic.o \
	 tests/basic.o \
	 tests/equations.o \
	 tests/trigonometry.o \
	 tests/tlmm-tests.o

INCDIR = include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

ifeq ($(config),debug)
LIBDIR = build/debug
endif

ifeq ($(config),release)
LIBDIR = build/release
endif
LDFLAGS =
LIBS= -ltlmm -lm -lstdc++

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = tlmm test suite

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
