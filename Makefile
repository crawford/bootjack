CONFIG = default

TARGET     = bootjack
BUILD_DIR  = build
SRC_DIR    = src
CONFIG_DIR = config

include $(CONFIG_DIR)/$(CONFIG).mk

CC      = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS  = -mmcu=$(MCU)
CFLAGS += -DF_CPU=$(F_CPU)UL
CFLAGS += -DCONFIG_APP_START_ADDR=$(APP_START)UL
CFLAGS += -DCONFIG_BOOT_START_ADDR=$(BOOT_START)UL
CFLAGS += -DCONFIG_BAUD_RATE=$(BAUD_RATE)UL
CFLAGS += $(OPTS)
CFLAGS += -funsigned-char
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -std=c99
CFLAGS += -Os
CFLAGS += -I.

LDFLAGS  = -Wl,-gc-sections
LDFLAGS += -ffreestanding
LDFLAGS += -dead_strip
LDFLAGS += -Ttext $(BOOT_START)

HEX = $(BUILD_DIR)/$(TARGET).hex
ELF = $(BUILD_DIR)/$(TARGET).elf

SRC = $(SRC_DIR)/avr109.c  \
      $(SRC_DIR)/main.c    \
      $(SRC_DIR)/menu.c    \
      $(SRC_DIR)/serial.c


OBJS = $(SRC:%.c=$(BUILD_DIR)/%.o)


all: $(HEX)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex --remove-section .eeprom --remove-section .fuse --remove-section .lock $< $@

.SECONDARY: $(ELF)
$(ELF): $(OBJS)
	$(CC) $(CFLAGS) $^ --output $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p "`echo $@ | sed 's|[^/]*\.o||')`"
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(HEX)
	rm -f $(ELF)
