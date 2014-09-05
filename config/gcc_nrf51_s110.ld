/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00014000, LENGTH = 176k /* 256k - 80k for SD */
  RAM (rwx) :  ORIGIN = 0x20002000, LENGTH = 8k   /* 16k - 8k for SD */
}

INCLUDE "gcc_nrf51_common.ld"
