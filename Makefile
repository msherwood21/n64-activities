emulate: CFLAGS += -DDEBUG_EMULATE -Wall -Wpedantic -DD=1
emulate: oliver.z64

debug: CFLAGS += -DDEBUG_CART -Wall -Wpedantic -DD=1
debug: oliver.z64

release: CFLAGS += -DNDEBUG -Wall -Wpedantic
release: oliver.z64

BUILD_DIR = build
SOURCE_DIR = src
include $(N64_INST)/include/n64.mk

OBJS = $(BUILD_DIR)/action.o \
	$(BUILD_DIR)/clock.o \
	$(BUILD_DIR)/log.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/object.o \
	$(BUILD_DIR)/objectLayout.o \
	$(BUILD_DIR)/peripheral.o \
	$(BUILD_DIR)/render.o

oliver.z64: N64_ROM_TITLE = "Oliver"

$(BUILD_DIR)/oliver.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) *.z64
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
