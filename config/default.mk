MCU   = atmega1280
F_CPU = 8000000

APP_START  = 0x00000000
BOOT_START = 0x0001E000
BAUD_RATE  = 19200

OPTS  = -DCONFIG_USE_VT100
#OPTS  = -DCONFIG_USE_INTERRUPTS
#OPTS  = -DCONFIG_AVR109_READ_BOOT
#OPTS += -DCONFIG_AVR109_WRITE_LOCK
#OPTS += -DCONFIG_AVR109_CHIP_ERASE