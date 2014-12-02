target remote localhost:2331
monitor flash download = 1
monitor flash device = nrf51822
mon reset

# Usage: flash (elf should be loaded on commande line)
define flash
  dont-repeat
  mon halt
  load
  mon reset
  cont
end

# interface with asm, regs and cmd windows
define split
  layout split
  layout asm
  layout regs
  focus cmd
end

