CC_FUSE   := `pkg-config fuse --cflags`
LD_FUSE   := `pkg-config fuse --libs`

CC_FLAGS  := -c -Wall -Wno-parentheses -Wno-pointer-arith \
             -Wno-unused-variable -Wno-unused-but-set-variable \
	     -g $(CC_FUSE)
LD_FLAGS  := $(LD_FUSE)
CC        := gcc
LD        := gcc

TARGET    := mfs
SOURCES   := $(wildcard *.c)
OBJECTS   := $(subst .c,.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $^ $(LD_FLAGS)

%.o: %.c
	$(CC) $(CC_FLAGS) -o $@ $<

dep:
	@set -e
	@rm -f Makefile.dep
	@echo $(SOURCES) | xargs -n 1 cpp $(CC_FUSE) -MM >>Makefile.dep

clean:
	@rm -f $(TARGET) *.o Makefile.dep tags core

start:
	./mfs -v /tmp/test && cat /tmp/test/hello

stop:
	fusermount -u /tmp/test

restart:
	fusermount -u /tmp/test
	./mfs /tmp/test && cat /tmp/test/hello

ifeq (Makefile.dep, $(wildcard Makefile.dep))
    include Makefile.dep
endif
