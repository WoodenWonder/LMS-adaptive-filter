PROJECT = lms

SRCDIR = source
INCDIR = include
BUILDDIR = build

MAJOR_VERSION ?= 1
MINOR_VERSION ?= 0
PATCH_VERSION ?= 0

TARGET = $(BUILDDIR)/$(PROJECT)

CC = gcc
CFLAGS = -Wall -Wextra -W -g -MMD -MP -O0
CFLAGS += -DMAJOR_VERSION=$(MAJOR_VERSION)
CFLAGS += -DMINOR_VERSION=$(MINOR_VERSION)
CFLAGS += -DPATCH_VERSION=$(PATCH_VERSION)
LDFLAGS = -lm

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
ASSEMBLY := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.s,$(SOURCES))

.PHONY: all clean

all: $(TARGET) $(ASSEMBLY)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(BUILDDIR)/%.s: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -S -masm=intel $< -o $@

clean:
	rm -rf $(BUILDDIR)