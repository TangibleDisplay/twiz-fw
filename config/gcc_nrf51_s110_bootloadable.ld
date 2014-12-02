/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00014000, LENGTH = 81k /* allow the use of dual bank bootloader */
  RAM (rwx) :  ORIGIN = 0x20002000, LENGTH = 8k
}

INCLUDE "gcc_nrf51_common.ld"
