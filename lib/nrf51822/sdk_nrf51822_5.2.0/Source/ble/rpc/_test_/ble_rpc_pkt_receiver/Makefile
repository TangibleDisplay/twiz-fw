TRUNK_ROOT = ../../../../../../..
BUILD_FOLDER = _build

MINGW_BIN=c:\\MinGW\\bin

export PATH:=$(MINGW_BIN);$(PATH)

CC   = c:/MinGW/bin/gcc.exe -D__DEBUG__
CPP  = c:/MinGW/bin/gcc.exe -D__DEBUG__
MD   = mkdir -p
LINT = c:/lint/lint-nt.exe
UV4  = c:/keil/UV4/uv4.exe

MOCK_TRANSPORT    = $(BUILD_FOLDER)/hci_transport.o
MOCK_SCHEDULER    = $(BUILD_FOLDER)/app_scheduler.o
MOCK_EVT_DECODER  = $(BUILD_FOLDER)/ble_rpc_evt_decoder.o
MOCK_CMD_ENCODER  = $(BUILD_FOLDER)/ble_rpc_cmd_encoder.o
MOCK_STACK_HANDLER = $(BUILD_FOLDER)/ble_stack_handler.o

OBJ  = $(BUILD_FOLDER)/ble_rpc_pkt_receiver.o
OBJ  += $(MOCK_TRANSPORT)
OBJ  += $(MOCK_SCHEDULER)
OBJ  += $(MOCK_EVT_DECODER)
OBJ  += $(MOCK_CMD_ENCODER)
OBJ  += $(OBJ_CUNIT) 
OBJ  += $(TEST_OBJ_EXT) 
OBJ  += $(MOCK_STACK_HANDLER)
BIN  = cunit_test.exe

MINGW_INCS = -IC:/MinGW/include -IC:/MinGW/lib/gcc/mingw32/4.6.2/include -IC:/MinGW/lib/gcc/mingw32/4.6.2/include-fixed

INCS = $(CUNIT_INC)
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/include
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/include/ble
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/include/ble/rpc
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/app_common
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/S110
INCS += $(MINGW_INCS)


UVPROJ_FILES = `find . -name '*.uvproj'`
 
CFLAGS = $(INCS) -DBOARD_NRF6310 -DNRF51 -Wall -g3 -DSVCALL_AS_NORMAL_FUNCTION 

include $(abspath $(TRUNK_ROOT)/framework/cunit/Makefile.mk)

all: folders cunit_test test

folders:
	$(MD) $(BUILD_FOLDER)

$(MOCK_TRANSPORT): $(TRUNK_ROOT)/Nordic/nrf51/Source/app_common/mock/hci_transport/hci_transport_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(MOCK_SCHEDULER): $(TRUNK_ROOT)/Nordic/nrf51/Source/app_common/mock/app_scheduler/app_scheduler_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(MOCK_EVT_DECODER): $(TRUNK_ROOT)/Nordic/nrf51/Source/ble/rpc/mock/ble_rpc_evt_decoder_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(MOCK_CMD_ENCODER): $(TRUNK_ROOT)/Nordic/nrf51/Source/ble/rpc/mock/ble_rpc_cmd_encoder_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/ble_rpc_pkt_receiver.o: ../../ble_rpc_pkt_receiver.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/ble_stack_handler.o: $(TRUNK_ROOT)/Nordic/nrf51/Source/ble/test/mock/ble_stack_handler_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
			
.PHONY: clean lint keil-compile keil-debug release

clean:
	$(RM) $(OBJ) $(BIN)

lint:
	$(LINT) -v _build\input.lnt
	
keil-compile:
	-@for file in ${UVPROJ_FILES}; do echo Compiling $$file ; $(UV4) -r $$file -o $(BUILD_FOLDER)/errors.txt ; done
	@cat $(BUILD_FOLDER)/errors.txt

keil-test: 
	@rm -f $(BUILD_FOLDER)/../cunit.log	
	$(UV4) -r $(BUILD_FOLDER)/../*.uvproj 
	$(UV4) -d $(BUILD_FOLDER)/../*.uvproj
	@cat $(BUILD_FOLDER)/../cunit.log	
		
		
release:	
	@(echo "Not implemented")
