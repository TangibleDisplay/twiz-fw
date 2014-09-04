/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256k
  RAM (rwx) :  ORIGIN = 0x20000000, LENGTH = 16k
}


INCLUDE "gcc_nrf51_common.ld"
