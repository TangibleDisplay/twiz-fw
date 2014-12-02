#include "twi_sys_evt.h"

void sys_evt_dispatch(uint32_t sys_evt) {
  pstorage_sys_event_handler(sys_evt);
}
