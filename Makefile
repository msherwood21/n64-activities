all: oliver.z64
.PHONY: all

BUILD_DIR = build
SOURCE_DIR = src
include $(N64_INST)/include/n64.mk

OBJS = $(BUILD_DIR)/main.o \
	$(BUILD_DIR)/render.o

oliver.z64: N64_ROM_TITLE = "Oliver"

$(BUILD_DIR)/oliver.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
