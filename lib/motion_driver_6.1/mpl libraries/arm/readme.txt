ARM MPL Libaries -

1. All libaries compiled using GCC NONE-ARM-EABI 4.7.2 compiler
2. M3 Compilers Options -

 -Os -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mcpu=cortex-m3 -mthumb -g3 -gdwarf-2

3. M4 Compiler Options -

-Os -ffunction-sections -fdata-sections -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=vfpv4 -g3 -gdwarf-2