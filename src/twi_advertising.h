#ifndef TWI_ADVERTISING_H
#define TWI_ADVERTISING_H

#include <stdint.h>

/*
 * The advertising interval (in units of 0.625 ms. This value corresponds to 40ms).
 * Must be between 0x0020 and 0x4000.
 */
#define APP_ADV_INTERVAL                64

/*
 * The advertising timeout (in units of seconds).
 * Must be between 0x0001 and 0x3FFF.
 * Set to 0 if no timeout.
 */
#define APP_ADV_TIMEOUT_IN_SECONDS      180

/*
 * Name of device. Will be included in the advertising data.
 */
#ifndef DEVICE_NAME
#define DEVICE_NAME                     "Twi"
#endif

/*
 * Structure representing the data sent in advertising packet.
 * twi_type: code used by applications to identified the twis they can connect to
 * revision: will be used for synchronization
 * current_time: current timestamp of the twi
 */
typedef struct __attribute__ ((packed, aligned(1))) twi_advdata_s {
  uint8_t  twi_type;
  uint16_t revision;
  unsigned current_time : 24;
} twi_advdata_t;

/*
 * Function for initializing the Advertising functionality.
 * Encodes the required advertising data and passes it to the stack.
 * Also builds a structure to be passed to the stack when starting advertising.
 * New services to advertise must be added here.
 */
void advertising_init(void);


/*
 * Function for starting advertising.
 */
void advertising_start(void);


#endif
