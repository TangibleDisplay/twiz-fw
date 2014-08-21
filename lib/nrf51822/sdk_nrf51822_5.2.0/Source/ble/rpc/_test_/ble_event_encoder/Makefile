TRUNK_ROOT = ../../../../../../..
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

OBJ  = $(BUILD_FOLDER)/ble_rpc_event_encoder.o
OBJ  += $(BUILD_FOLDER)/ble_rpc_event_encoder_gap.o
OBJ  += $(BUILD_FOLDER)/ble_rpc_event_encoder_gatts.o
OBJ  += $(MOCK_HCI_TRANSPORT)
OBJ  += $(OBJ_CUNIT) 
OBJ  += $(TEST_OBJ_EXT) 
BIN  = cunit_test.exe

MINGW_INCS = -IC:/MinGW/include -IC:/MinGW/lib/gcc/mingw32/4.7.2/include -IC:/MinGW/lib/gcc/mingw32/4.7.2/include-fixed

INCS = $(CUNIT_INC)
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/include/ble/rpc
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/app_common
INCS += -I$(TRUNK_ROOT)/Nordic/nrf51/Include/S110
INCS += $(MINGW_INCS)


UVPROJ_FILES = `find . -name '*.uvproj'`
 
CFLAGS = $(INCS) -DBOARD_NRF6310 -DNRF51 -Wall -g3 -DSVCALL_AS_NORMAL_FUNCTION

include $(abspath $(TRUNK_ROOT)/framework/cunit/Makefile.mk)

all: folders cunit_test test

folders:
	-$(MD) $(BUILD_FOLDER)
	
$(MOCK_HCI_TRANSPORT): $(TRUNK_ROOT)/Nordic/nrf51/Source/app_common/mock/hci_transport/hci_transport_mock.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/ble_rpc_event_encoder.o: ../../ble_rpc_event_encoder.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_FOLDER)/ble_rpc_event_encoder_gap.o: ../../ble_rpc_event_encoder_gap.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/ble_rpc_event_encoder_gatts.o: ../../ble_rpc_event_encoder_gatts.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/cunit_test_gap.o: cunit_test_gap.c
	$(CC) -c $< -o $@ $(CFLAGS)
	
$(BUILD_FOLDER)/cunit_test_gatts.o: cunit_test_gatts.c
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
