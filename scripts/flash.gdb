define erase-all
  dont-repeat
  mon halt
  set {int} 0x4001e504 = 2
  set {int} 0x4001e50c = 1
  set {int} 0x40010514 = 1
  set {int} 0x10001014 = 0xFfffFfff
  mon reset
end

# Usage: flash-all softdevice elf
define flash-all
  dont-repeat
  erase-all
  shell sleep 1
  file $arg0
  load
  file $arg1
  load
  set {int} 0x10001014 = 0xFfffFfff
  mon reset
  mon go
end

# Usage: flash elf
define flash
  dont-repeat
  file $arg0
  load
  set {int} 0x10001014 = 0xFfffFfff
  mon reset
  mon go
end

# interface with asm, regs and cmd windows
define split
  layout split
  layout asm
  layout regs
  focus cmd
end
