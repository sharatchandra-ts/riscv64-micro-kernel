-include config.mk

# Compiler & Toolchain
CC        := riscv64-elf-gcc
CFLAGS    := -nostdlib -Wall -Wextra -MMD -MP
LDFLAGS   := -T linker.ld \
             -Wl,--defsym=RAM_START_ADDR=$(RAM_START_ADDR) \
             -Wl,--defsym=RAM_SIZE=$(RAM_SIZE)

# Directories & Targets
SRC_DIR   := src
BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
TARGET    := $(BUILD_DIR)/kernel.elf

# Source, Object, and Dependency Files
SRCS      := $(wildcard $(SRC_DIR)/*.c)
OBJS      := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS      := $(OBJS:.o=.d)

# Pretty printing / Verbosity control
# Run `make V=1` to see the full raw commands
ifneq ($(V),1)
Q := @
endif

# Default Rule
all: $(TARGET)

# Link object files into final ELF binary
$(TARGET): $(OBJS) | $(BUILD_DIR)
	@echo "  LD      $@"
	$(Q)$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Compile src/*.c into build/obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "  CC      $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

# Create output directories quietly
$(OBJ_DIR) $(BUILD_DIR):
	$(Q)mkdir -p $@

# Clean build artifacts
clean:
	@echo "  CLEAN   $(BUILD_DIR)"
	$(Q)rm -rf $(BUILD_DIR)

# Include generated dependency files (.d)
-include $(DEPS)

.PHONY: all clean
