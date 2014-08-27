C_SOURCE_FILES += main.c

# nRF51822 Source
C_SOURCE_FILES += hardware.c
C_SOURCE_FILES += ble_uart.c
C_SOURCE_FILES += ble_nus.c
C_SOURCE_FILES += mpu.c
C_SOURCE_FILES += nrf_delay.c
C_SOURCE_FILES += ble_srv_common.c
C_SOURCE_FILES += ble_advdata.c
C_SOURCE_FILES += ble_conn_params.c
C_SOURCE_FILES += softdevice_handler.c
C_SOURCE_FILES += simple_uart.c
C_SOURCE_FILES += app_timer.c
C_SOURCE_FILES += twi_hw_master.c

# MPU9150 Source
C_SOURCE_FILES += inv_mpu.c
C_SOURCE_FILES += inv_mpu_dmp_motion_driver.c
C_SOURCE_FILES += mpl.c
C_SOURCE_FILES += storage_manager.c
C_SOURCE_FILES += start_manager.c
C_SOURCE_FILES += data_builder.c
C_SOURCE_FILES += results_holder.c
C_SOURCE_FILES += ml_math_func.c
C_SOURCE_FILES += mlmath.c
C_SOURCE_FILES += eMPL_outputs.c
C_SOURCE_FILES += log_stm32l.c
C_SOURCE_FILES += message_layer.c
C_SOURCE_FILES += hal_outputs.c


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

OBJECT_DIRECTORY := obj
LISTING_DIRECTORY := bin
OUTPUT_BINARY_DIRECTORY := build
OUTPUT_FILENAME := twi
ELF := $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

DEVICE := NRF51
DEVICESERIES := nrf51
CPU := cortex-m0

GDB_PORT_NUMBER := 2331

# Toolchain
GNU_INSTALL_ROOT := /usr
GNU_VERSION := 4.8.3
GNU_PREFIX := arm-none-eabi
CC       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as"
AR       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld"
NM       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY  		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy"
GDB       		:= "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gdb"
CGDB            := "/usr/local/bin/cgdb"

MK 				:= mkdir
RM 				:= rm -rf

# Programmer
JLINK = -JLinkExe
JLINKGDBSERVER = JLinkGDBServer

# nRF51822 Source Paths
C_SOURCE_PATHS += src
C_SOURCE_PATHS += src/startup
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
CFLAGS += -mcpu=$(CPU) -mthumb -mabi=aapcs -D$(DEVICE) --std=gnu99
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DMPU9150 -DEMPL -DUSE_DMP
CFLAGS += -DMPL_LOG_NDEBUG=0 # TODO !?
CFLAGS += -Os
#CFLAGS += -Wall# -Werror
CFLAGS += -ffunction-sections -fdata-sections # split bin in little sections...

# Linker flags
CONFIG_PATH += config/
LINKER_SCRIPT = gcc_$(DEVICESERIES)_s110.ld
LDFLAGS += -L"$(GNU_INSTALL_ROOT)/arm-none-eabi/lib/armv6-m"
LDFLAGS += -L"$(GNU_INSTALL_ROOT)/lib/gcc/arm-none-eabi/$(GNU_VERSION)/armv6-m"
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mcpu=$(CPU) -mthumb -mabi=aapcs
LDFLAGS += -L$(CONFIG_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -Wl,--gc-sections # remove unused sections (separated thanks to the last CFLAGS)


FLASH_START_ADDRESS = 0x14000

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )


####################################################################
# Rules                                                            #
####################################################################

C_SOURCE_FILENAMES = $(notdir $(C_SOURCE_FILES) )
ASSEMBLER_SOURCE_FILENAMES = $(notdir $(ASSEMBLER_SOURCE_FILES) )

C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILENAMES:.c=.o) )
ASSEMBLER_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASSEMBLER_SOURCE_FILENAMES:.s=.o) )

# Set source lookup paths
vpath %.c $(C_SOURCE_PATHS)
vpath %.s $(ASSEMBLER_SOURCE_PATHS)

# Include automatically previously generated dependencies
-include $(addprefix $(OBJECT_DIRECTORY)/, $(COBJS:.o=.d))

## Default build target
.PHONY: all
all: release

clean:
	$(RM) $(OUTPUT_BINARY_DIRECTORY)/*
	$(RM) $(OBJECT_DIRECTORY)/*
	$(RM) $(LISTING_DIRECTORY)/*
	- $(RM) JLink.log
	- $(RM) .gdbinit

## Program device
#.PHONY: flash
#flash: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex
#	nrfjprog --reset --program $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

### Targets
.PHONY: debug
debug:    CFLAGS += -DDEBUG -g3 -O0
debug:    $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

.PHONY: release
release:  CFLAGS += -DNDEBUG -Os
release:  $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echostuff:
	echo $(C_OBJECTS)
	echo $(C_SOURCE_FILES)

## Create build directories
$(BUILD_DIRECTORIES):
	$(MK) $@

## Create objects from C source files
$(OBJECT_DIRECTORY)/%.o: %.c
# Build header dependencies
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -M $< -MF "$(@:.o=.d)" -MT $@
# Do the actual compilation
	$(CC) $(CFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Assemble .s files
$(OBJECT_DIRECTORY)/%.o: %.s
	$(CC) $(ASMFLAGS) $(INCLUDEPATHS) -c -o $@ $<

## Link C and assembler objects to an .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(C_OBJECTS) $(ASSEMBLER_OBJECTS)
	$(CC) $(LDFLAGS) $(C_OBJECTS) $(ASSEMBLER_OBJECTS) $(LIBRARIES) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out

## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

## Program device
flash: flash.jlink stopdebug $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin
	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/flash.jlink

flash.jlink:
	echo "device nrf51822\nspeed 1000\nr\nloadbin $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin, $(FLASH_START_ADDRESS)\nr\ng\nexit\n" > $(OUTPUT_BINARY_DIRECTORY)/flash.jlink

flash-softdevice: erase-all flash-softdevice.jlink stopdebug
ifndef SOFTDEVICE
	$(error "You need to set the SOFTDEVICE command-line parameter to a path (without spaces) to the softdevice hex-file")
endif

	# Convert from hex to binary. Split original hex in two to avoid huge (>250 MB) binary file with just 0s.
	$(OBJCOPY) -Iihex -Obinary --remove-section .sec3 $(SOFTDEVICE) $(OUTPUT_BINARY_DIRECTORY)/_mainpart.bin
	$(OBJCOPY) -Iihex -Obinary --remove-section .sec1 --remove-section .sec2 $(SOFTDEVICE) $(OUTPUT_BINARY_DIRECTORY)/_uicr.bin

	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/flash-softdevice.jlink

flash-softdevice.jlink:
	# Do magic. Write to NVMC to enable erase, do erase all and erase UICR, reset, enable writing, load mainpart bin, load uicr bin. Reset.
	# Resetting in between is needed to disable the protections.
	echo "w4 4001e504 1\nloadbin \"$(OUTPUT_BINARY_DIRECTORY)/_mainpart.bin\" 0\nloadbin \"$(OUTPUT_BINARY_DIRECTORY)/_uicr.bin\" 0x10001000\nr\ng\nexit\n" > $(OUTPUT_BINARY_DIRECTORY)/flash-softdevice.jlink
	#echo "w4 4001e504 1\nloadbin \"$(OUTPUT_BINARY_DIRECTORY)/softdevice.bin\" 0\nr\ng\nexit\n" > flash-softdevice.jlink

recover: recover.jlink erase-all.jlink pin-reset.jlink
	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/recover.jlink
	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/erase-all.jlink
	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/pin-reset.jlink

recover.jlink:
	echo "si 0\nt0\nsleep 1\ntck1\nsleep 1\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\nt0\nsleep 2\nt1\nsleep 2\ntck0\nsleep 100\nsi 1\nr\nexit\n" > $(OUTPUT_BINARY_DIRECTORY)/recover.jlink

pin-reset.jlink:
	echo "device nrf51822\nw4 4001e504 2\nw4 40000544 1\nr\nexit\n" > $(OUTPUT_BINARY_DIRECTORY)/pin-reset.jlink

erase-all: erase-all.jlink
	$(JLINK) $(OUTPUT_BINARY_DIRECTORY)/erase-all.jlink

erase-all.jlink:
	echo "device nrf51822\nw4 4001e504 2\nw4 4001e50c 1\nw4 4001e514 1\nr\nexit\n" > $(OUTPUT_BINARY_DIRECTORY)/erase-all.jlink

startdebug: stopdebug debug.jlink .gdbinit
	$(JLINKGDBSERVER) -single -if swd -speed 1000 -port $(GDB_PORT_NUMBER) &
	sleep 1
	$(GDB) $(ELF)

stopdebug:
	-killall $(JLINKGDBSERVER)

.gdbinit:
	echo "target remote localhost:$(GDB_PORT_NUMBER)\nmonitor flash download = 1\nmonitor flash device = nrf51822\nbreak main\nmon reset\n" > .gdbinit

debug.jlink:
	echo "Device nrf51822" > $(OUTPUT_BINARY_DIRECTORY)/debug.jlink

.PHONY: flash flash-softdevice erase-all startdebug stopdebug

