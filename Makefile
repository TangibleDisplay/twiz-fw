C_SOURCE_FILES += main.c
C_SOURCE_FILES += leds.c
C_SOURCE_FILES += timers.c
C_SOURCE_FILES += helpers.c
C_SOURCE_FILES += printf.c snprintf.c sprintf.c format.c
C_SOURCE_FILES += softdevice_handler.c

# nRF51822 Source
C_SOURCE_FILES += app_fifo.c
C_SOURCE_FILES += app_uart_fifo.c
C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += app_gpiote.c
C_SOURCE_FILES += nrf_delay.c

# MPU9150 Source

# startup files
C_SOURCE_FILES += system_$(DEVICESERIES).c
ASSEMBLER_SOURCE_FILES += gcc_startup_$(DEVICESERIES).s

# nRF51822 Paths
SDK_PATH = lib/nrf51822/sdk_nrf51822_5.2.0/
SDK_SOURCE_PATH = $(SDK_PATH)Source/
SDK_INCLUDE_PATH = $(SDK_PATH)Include/

# MPU9150 Paths
MPU_PATH = lib/mpu9150/

LIBRARIES += $(MPU_PATH)eMD6/core/mpl/libmpllib.a
LIBRARIES += -lm

SOFTDEVICE := lib/nrf51822/s110_nrf51822_6.0.0/s110_nrf51822_6.0.0_softdevice.hex

OUTPUT_FILENAME 		= twi

OBJECT_DIRECTORY 		= obj/
OUTPUT_PATH				= build/

HEX 	= $(OUTPUT_PATH)$(OUTPUT_FILENAME).hex
ELF 	= $(OUTPUT_PATH)$(OUTPUT_FILENAME).elf
BIN 	= $(OUTPUT_PATH)$(OUTPUT_FILENAME).bin


DEVICE := NRF51
DEVICESERIES := nrf51
CPU := cortex-m0

GDB_PORT_NUMBER := 2331

# Toolchain
GNU_PREFIX 		:= arm-none-eabi
CC       		:= $(GNU_PREFIX)-gcc
AS       		:= $(GNU_PREFIX)-as
AR       		:= $(GNU_PREFIX)-ar -r
LD       		:= $(GNU_PREFIX)-gcc
NM       		:= $(GNU_PREFIX)-nm
OBJDUMP  		:= $(GNU_PREFIX)-objdump
OBJCOPY  		:= $(GNU_PREFIX)-objcopy
GDB       		:= $(GNU_PREFIX)-gdb
SIZE       		:= $(GNU_PREFIX)-size

TERMINAL 		?= gnome-terminal -e

MK 				:= mkdir -p
RM 				:= rm -rf

# Programmer
JLINK = -JLinkExe
JLINKGDBSERVER = JLinkGDBServer

# nRF51822 Source Paths
C_SOURCE_PATHS += src
C_SOURCE_PATHS += src/startup
C_SOURCE_PATHS += src/printf
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)nrf_delay
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)app_common
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)simple_uart
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)sd_common
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)ble
C_SOURCE_PATHS += $(SDK_SOURCE_PATH)ble/ble_services
ASSEMBLER_SOURCE_PATHS = src/startup

# MPU9150 Source Paths
C_SOURCE_PATHS += $(MPU_PATH)eMD6/core/driver/eMPL/
C_SOURCE_PATHS += $(MPU_PATH)eMD6/core/mllite/
C_SOURCE_PATHS += $(MPU_PATH)eMD6/core/eMPL-hal/
C_SOURCE_PATHS += $(MPU_PATH)eMD6/core/driver/stm32L/

# nRF51822 Include Paths
INCLUDEPATHS += -Isrc
INCLUDEPATHS += -Isrc/printf
INCLUDEPATHS += -I$(SDK_PATH)Include
INCLUDEPATHS += -I$(SDK_PATH)Include/app_common
INCLUDEPATHS += -I$(SDK_PATH)Include/sd_common
INCLUDEPATHS += -I$(SDK_PATH)Include/s110
INCLUDEPATHS += -I$(SDK_PATH)Include/ble
INCLUDEPATHS += -I$(SDK_PATH)Include/ble/ble_services
INCLUDEPATHS += -I$(SDK_PATH)Include/gcc

# MPU9150 Include Paths
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/driver/eMPL/
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/driver/include/
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/mllite/
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/mpl/
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/eMPL-hal/
INCLUDEPATHS += -I$(MPU_PATH)eMD6/core/driver/stm32L/


# Compiler flags
CFLAGS += -Os
CFLAGS += -g3 -MD -c
CFLAGS += -mcpu=$(CPU) -mthumb -march=armv6-m -D$(DEVICE) --std=gnu99
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DMPU9150 -DEMPL -DUSE_DMP
CFLAGS += -DMPL_LOG_NDEBUG=1 -DNDEBUG -DREMOVE_LOGGING
CFLAGS += -flto -fno-builtin # https://plus.google.com/+AndreyYurovsky/posts/XUr9VBPFDn7
CFLAGS += -Wall
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wno-unused-local-typedefs -Wno-old-style-declaration -Wno-unused-parameter

# Linker flags
CONFIG_PATH += config/
LINKER_SCRIPT = gcc_$(DEVICESERIES)_s110.ld
LDFLAGS += -Xlinker -Map=$(OUTPUT_PATH)$(OUTPUT_FILENAME).map
LDFLAGS += -mcpu=$(CPU) -mthumb -march=armv6-m
LDFLAGS += -L$(CONFIG_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections
LDFLAGS += --specs=nano.specs

# Assembly flags
ASFLAGS += -D__HEAP_SIZE=0
ASFLAGS +=  -x assembler-with-cpp

FLASH_START_ADDRESS = $(shell $(OBJDUMP) -h $(ELF) -j .text | grep .text | awk '{print $$4}')

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_PATH) )


####################################################################
# Rules                                                            #
####################################################################

C_SOURCE_FILENAMES = $(notdir $(C_SOURCE_FILES) )
ASSEMBLER_SOURCE_FILENAMES = $(notdir $(ASSEMBLER_SOURCE_FILES) )

C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY), $(C_SOURCE_FILENAMES:.c=.o) )
ASSEMBLER_OBJECTS = $(addprefix $(OBJECT_DIRECTORY), $(ASSEMBLER_SOURCE_FILENAMES:.s=.o) )

# Set source lookup paths
vpath %.c $(C_SOURCE_PATHS)
vpath %.s $(ASSEMBLER_SOURCE_PATHS)

## Default build target
.PHONY: all
all: $(ELF) $(HEX) $(BIN)

clean:
	$(RM) $(OUTPUT_PATH)*
	$(RM) $(OBJECT_DIRECTORY)*
	- $(RM) JLink.log

### Targets
echostuff:
	echo $(C_OBJECTS)
	echo $(C_SOURCE_FILES)

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

## Create objects from C source files
$(OBJECT_DIRECTORY)%.o: %.c | $(BUILD_DIRECTORIES)
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Assemble .s files
$(OBJECT_DIRECTORY)%.o: %.s | $(BUILD_DIRECTORIES)
	$(CC) $(ASFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Link C and assembler objects to an .out file
$(ELF): $(C_OBJECTS) $(ASSEMBLER_OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LIBRARIES) -o $@
	$(SIZE) $@

## Create binary .bin file from the .elf file
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $(ELF) $@

# Include automatically previously generated dependencies
-include $(C_OBJECTS:.o=.d)

## Program device
flash: flash.jlink stopgdbserver $(BIN)
	$(JLINK) $(OUTPUT_PATH)flash.jlink

flash.jlink:
	printf "device nrf51822\nspeed 1000\nr\nloadbin $(BIN), $(FLASH_START_ADDRESS)\nr\ng\nexit\n" > $(OUTPUT_PATH)flash.jlink

flash-softdevice: erase-all flash-softdevice.jlink stopgdbserver
ifndef SOFTDEVICE
	$(error "You need to set the SOFTDEVICE command-line parameter to a path (without spaces) to the softdevice hex-file")
endif

	# Convert from hex to binary. Split original hex in two to avoid huge (>250 MB) binary file with just 0s.
	$(OBJCOPY) -Iihex -Obinary --remove-section .sec3 $(SOFTDEVICE) $(OUTPUT_PATH)_mainpart.bin
	$(OBJCOPY) -Iihex -Obinary --remove-section .sec1 --remove-section .sec2 $(SOFTDEVICE) $(OUTPUT_PATH)_uicr.bin

	$(JLINK) $(OUTPUT_PATH)flash-softdevice.jlink

flash-softdevice.jlink:
	# Do magic. Write to NVMC to enable erase, do erase all and erase UICR, reset, enable writing, load mainpart bin, load uicr bin. Reset.
	# Resetting in between is needed to disable the protections.
	printf "w4 4001e504 1\nloadbin \"$(OUTPUT_PATH)_mainpart.bin\" 0\nloadbin \"$(OUTPUT_PATH)_uicr.bin\" 0x10001000\nr\ng\nexit\n" > $(OUTPUT_PATH)flash-softdevice.jlink
	#printf "w4 4001e504 1\nloadbin \"$(OUTPUT_PATH)softdevice.bin\" 0\nr\ng\nexit\n" > flash-softdevice.jlink

recover: recover.jlink erase-all.jlink pin-reset.jlink
	$(JLINK) $(OUTPUT_PATH)recover.jlink
	$(JLINK) $(OUTPUT_PATH)erase-all.jlink
	$(JLINK) $(OUTPUT_PATH)pin-reset.jlink

recover.jlink:
	printf "si 0\nt0\nsleep 1\ntck1\nsleep 1\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\ntck0\nsleep 100\nsi 1\nr\nexit\n" > $(OUTPUT_PATH)recover.jlink

pin-reset.jlink:
	printf "device nrf51822\nw4 4001e504 2\nw4 40000544 1\nr\nexit\n" > $(OUTPUT_PATH)pin-reset.jlink

erase-all: erase-all.jlink
	$(JLINK) $(OUTPUT_PATH)erase-all.jlink

erase-all.jlink:
	printf "device nrf51822\nw4 4001e504 2\nw4 4001e50c 1\nw4 4001e514 1\nr\nexit\n" > $(OUTPUT_PATH)erase-all.jlink

startgdbserver: stopgdbserver $(ELF)
	$(TERMINAL) "$(JLINKGDBSERVER) -single -if swd -speed 1000 -port $(GDB_PORT_NUMBER)"
	sleep 1

debug: startgdbserver $(ELF)
	$(GDB) $(ELF)

stopgdbserver:
	-@killall $(JLINKGDBSERVER)

.PHONY: flash flash-softdevice erase-all startgdbserver stopgdbserver debug
