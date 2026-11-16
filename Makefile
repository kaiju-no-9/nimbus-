CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic -D_FORTIFY_SOURCE=2 -fstack-protector-strong
HOST_OS := $(shell uname -s)
ifeq ($(HOST_OS),Linux)
PIE_FLAGS ?= -fPIE -pie
LDFLAGS ?= -Wl,-z,relro,-z,now
else
PIE_FLAGS ?= -fPIE
LDFLAGS ?=
endif

.PHONY: all clean run
all: tiny

tiny: tiny.c
	$(CC) $(CFLAGS) $(PIE_FLAGS) $(LDFLAGS) -o $@ $<

run: tiny
	./tiny

clean:
	rm -f tiny tiny-asan
