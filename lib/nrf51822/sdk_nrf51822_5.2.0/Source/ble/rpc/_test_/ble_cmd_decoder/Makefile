TRUNK_ROOT = ../../../../../../../
BUILD_FOLDER = _build

MINGW_BIN=c:\\MinGW\\bin

export PATH:=$(MINGW_BIN);$(PATH)

CC   = c:/MinGW/bin/gcc.exe -D__DEBUG__
CPP  = c:/MinGW/bin/gcc.exe -D__DEBUG__
MD   = mkdir
LINT = c:/lint/lint-nt.exe
UV4  = c:/keil/UV4/uv4.exe

MOCK_HCI_TRANSPORT = $(BUILD_FOLDER)/hci_transport_mock.o

TEST_OBJ_EXT =  $(BUILD_FOLDER)/cunit_test_gap.o
TEST_OBJ_EXT += $(BUILD_FOLDER)/cunit_test_gatts.o
TEST_OBJ_EXT += $(BUILD_FOLDER)/cunit_test_misc.o

OBJ  = $(BUILD_FOLDER)/ble_rpc_cmd_decoder.o
OBJ  += $(BUILD_FOLDER)/ble_rpc_cmd_decoder_gap.o
OBJ  += $(BUILD_FOLDER)/ble_rpc_cmd_decoder_gatts.o
OBJ  += $(BUILD_FOLDER)/ble_stack_gap_mock.o
OBJ  += $(BUILD_FOLDER)/ble_stack_mock.o
OBJ  += $(BUILD_FOLDER)/ble_stack_gatts_mock.o
OBJ  += $(MOCK_HCI_TRANSPORT)
OBJ  += $(OBJ_CUNIT) 
OBJ  += $(TEST_OBJ_EXT) 
BIN  = cunit_test.exe

INCS = -I$(TRUNK_ROOT)/Nordic/nrf51/Include/ble/rpc	
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/ble/ble_services
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/S110
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/app_common
INCS += $(CUNIT_INC)

UVPROJ_FILES = `find . -name '*.uvproj'`
DEFINES = -DBOARD_NRF6310 -DSVCALL_AS_NORMAL_FUNCTION  
 
CFLAGS = $(INCS) $(DEFINES) -Wall -g3 

include $(abspath $(TRUNK_ROOT)/framework/cunit/Makefile.mk)

all: folders cunit_test

buildandtest: folders cunit_test test
	
	
folders:
	-$(MD) $(BUILD_FOLDER)
	
$(BUILD_FOLDER)/ble_rpc_cmd_decoder.o: ../../ble_rpc_cmd_decoder.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/ble_rpc_cmd_decoder_gap.o: ../../ble_rpc_cmd_decoder_gap.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/ble_rpc_cmd_decoder_gatts.o: ../../ble_rpc_cmd_decoder_gatts.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/ble_stack_gatts_mock.o: ../mock/S110/ble_stack_gatts_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/ble_stack_gap_mock.o: ../mock/S110/ble_stack_gap_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/ble_stack_mock.o: ../mock/S110/ble_stack_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(MOCK_HCI_TRANSPORT): $(TRUNK_ROOT)/Nordic/nrf51/Source/app_common/mock/hci_transport/hci_transport_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/cunit_test_gap.o: cunit_test_gap.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/cunit_test_gatts.o: cunit_test_gatts.c
	$(CC) -c $< -o $@ $(CFLAGS)		
	
$(BUILD_FOLDER)/cunit_test_misc.o: cunit_test_misc.c
	$(CC) -c $< -o $@ $(CFLAGS)		
	
.PHONY: clean lint keil-compile keil-debug release

clean:
	$(RM) $(OBJ) $(BIN)

lint:
	$(LINT) -v _build\input.lnt
	
keil-compile:
	-@for file in ${UVPROJ_FILES}; do echo Compiling $$file ; $(UV4) -b $$file -o $(BUILD_FOLDER)/errors.txt ; done
	@cat $(BUILD_FOLDER)/errors.txt

keil-test: 
	@rm -f $(BUILD_FOLDER)/../cunit.log	
	$(UV4) -r $(BUILD_FOLDER)/../*.uvproj 
	$(UV4) -d $(BUILD_FOLDER)/../*.uvproj
	@cat $(BUILD_FOLDER)/../cunit.log	
		
release:
	@(echo "Not implemented")
