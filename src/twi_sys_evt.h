#ifndef TWI_SYS_EVT_H
#define TWI_SYS_EVT_H

#include <stdint.h>

// Persistent storage system event handler
void pstorage_sys_event_handler (uint32_t p_evt);

/*
 * Function for dispatching a system event to interested modules.
 * This function is called from the System event interrupt handler after a system
 * event has been received.
 * sys_evt   System stack event.
 */
void sys_evt_dispatch(uint32_t sys_evt);


#endif
