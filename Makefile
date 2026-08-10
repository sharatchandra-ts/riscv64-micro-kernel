-include config.mk

# Compiler & Toolchain
CC        := riscv64-elf-gcc
CFLAGS    := -Iinclude -nostdlib -ffreestanding -Wall -Wextra -MMD -MP -mcmodel=medany -msmall-data-limit=0
ASFLAGS   := -nostdlib -ffreestanding -MMD -MP -mcmodel=medany
LDFLAGS   := -T linker.ld \
             -Wl,--defsym=RAM_START_ADDR=$(RAM_START_ADDR) \
             -Wl,--defsym=RAM_SIZE=$(RAM_SIZE)

# Directories & Targets
SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
TARGET    := $(BUILD_DIR)/kernel.elf

# Source, Object, and Dependency Files
SRCS        := $(shell find $(SRC_DIR) -type f \( -name '*.c' -o -name '*.s' -o -name '*.S' \))
OBJS        := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(basename $(SRCS)))
DEPS        := $(OBJS:.o=.d)

# Extract unique object directory paths (including nested folders like modules/)
OBJ_SUBDIRS := $(sort $(dir $(OBJS)))

# Pretty printing / Verbosity control
ifneq ($(V),1)
Q := @
endif

# Default Rule
all: $(TARGET)

# Link object files into final ELF binary
$(TARGET): $(OBJS) | $(BUILD_DIR)
	@echo "  LD      $@"
	$(Q)$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Compile C sources (uses dynamic directory creation via $(dir $@))
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Assemble raw assembly (.s)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@echo "  AS      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

# Assemble preprocessing assembly (.S)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.S
	@echo "  AS      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(ASFLAGS) -c $< -o $@

# Create target build directory
$(BUILD_DIR):
	$(Q)mkdir -p $@

# Clean build artifacts
clean:
	@echo "  CLEAN   $(BUILD_DIR)"
	$(Q)rm -rf $(BUILD_DIR)

# Include generated dependency files (.d)
-include $(DEPS)

.PHONY: all clean
