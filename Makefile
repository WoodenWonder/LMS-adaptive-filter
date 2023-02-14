TARGET ?= lms

SOURCE_DIR ?= source
INCLUDE_DIR ?= include
BUILD_DIR ?= build

SOURCES := $(shell find $(SOURCE_DIR) -name "*.c" -or -name "*.s")
OBJECTS := $(SOURCES:%=$(BUILD_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDE_DIR := $(shell find $(INCLUDE_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIR))

CFLAGS ?= $(INCLUDE_FLAGS) -O0 -MMD -MP -W -Wall

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPENDENCIES)

MKDIR_P ?= mkdir -p