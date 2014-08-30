/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00014000, LENGTH = 0x14400 /* = 0x28400 - 0x14000 <=> Dual Bank Layout */
  RAM (rwx) :  ORIGIN = 0x20002000, LENGTH = 0x2000
}

INCLUDE "gcc_nrf51_common.ld"
