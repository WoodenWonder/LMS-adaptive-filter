TARGET ?= lms

MAJOR_VERSION ?= 1
MINOR_VERSION ?= 0
PATCH_VERSION ?= 0

SOURCE_DIR ?= source
INCLUDE_DIR ?= include
BUILD_DIR ?= build

SOURCES := $(shell find $(SOURCE_DIR) -name "*.c" -or -name "*.s")
OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDE_DIR := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))

CC ?= gcc
CFLAGS ?= $(INCLUDE_FLAGS) -O0 -MMD -MP -W -Wall -Wextra
CFLAGS += -DMAJOR_VERSION=$(MAJOR_VERSION)
CFLAGS += -DMINOR_VERSION=$(MINOR_VERSION)
CFLAGS += -DPATCH_VERSION=$(PATCH_VERSION)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) -lm

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPENDENCIES)

MKDIR_P ?= mkdir -p