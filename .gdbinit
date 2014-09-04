target remote :2331
monitor flash download = 1
monitor flash device = nrf51822

define flash
  load
  monitor reset
  continue
end
