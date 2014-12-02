/**
  ******************************************************************************
  * @file    stm32_tsl_checkconfig.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file checks the configuration
  *          file options.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSL_CHECKCONFIG_H
#define __TSL_CHECKCONFIG_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

//------------------------------------------------------------------------------
// MCU check and Standard Peripheral Library selection
//------------------------------------------------------------------------------

#if defined(STM32L15XX8_64K) && defined(STM32L15XXB_128K)
#error "Wrong device selection (select only one device)."
#endif

#if !defined(STM32L15XX8_64K) && !defined(STM32L15XXB_128K)
#error "No device selected (STM32L15XX8_64K or STM32L15XXB_128K must be selected)."
#endif

#if defined(STM32L15XX8_64K) || defined(STM32L15XXB_128K)
#define STM32L15XX8B (1) // Generic name used in the library
#include "stm32l1xx.h" // Standard Peripherals Library
#endif

//------------------------------------------------------------------------------
// Single channel keys check and definitions
//------------------------------------------------------------------------------

#if (SCKEY_P1_KEY_COUNT < 0) || (SCKEY_P1_KEY_COUNT > 10)
#error "Wrong number of single channel keys on port1. Must be in the [0..10] range."
#endif

#if (SCKEY_P2_KEY_COUNT < 0) || (SCKEY_P2_KEY_COUNT > 10)
#error "Wrong number of single channel keys on port2. Must be in the [0..10] range."
#endif

#if (SCKEY_P3_KEY_COUNT < 0) || (SCKEY_P3_KEY_COUNT > 10)
#error "Wrong number of single channel keys on port3. Must be in the [0..10] range."
#endif

#if ((SCKEY_P1_KEY_COUNT == 0) && ((SCKEY_P2_KEY_COUNT > 0) || (SCKEY_P3_KEY_COUNT > 0)))
#error "Key(s) defined on PORT2 or PORT3 while no key seems defined on PORT1. PORT1 must be used before using PORT2 or PORT3"
#endif

#if ((SCKEY_P2_KEY_COUNT == 0) && (SCKEY_P3_KEY_COUNT > 0))
#error "Key(s) defined on PORT3 while no key seems defined on PORT2. PORT2 must be used before using PORT3"
#endif

// Calculates the total number of single channel keys
#define NUMBER_OF_SINGLE_CHANNEL_KEYS (SCKEY_P1_KEY_COUNT + SCKEY_P2_KEY_COUNT + SCKEY_P3_KEY_COUNT)

// Define the number of single channel "ports" used
#if (SCKEY_P1_KEY_COUNT == 0)
#define NUMBER_OF_SINGLE_CHANNEL_PORTS (0)
#else // SCKEY_P1_KEY_COUNT > 0
#if ((SCKEY_P2_KEY_COUNT == 0) && (SCKEY_P3_KEY_COUNT == 0))
#define NUMBER_OF_SINGLE_CHANNEL_PORTS (1)
#endif
#if ((SCKEY_P2_KEY_COUNT > 0) && (SCKEY_P3_KEY_COUNT == 0))
#define NUMBER_OF_SINGLE_CHANNEL_PORTS (2)
#endif
#if ((SCKEY_P2_KEY_COUNT > 0) && (SCKEY_P3_KEY_COUNT > 0))
#define NUMBER_OF_SINGLE_CHANNEL_PORTS (3)
#endif
#if ((SCKEY_P2_KEY_COUNT == 0) && (SCKEY_P3_KEY_COUNT > 0))
#error "SCKEY PORT2 is not defined (mandatory if PORT3 is defined)."
#endif
#endif // SCKEY_P1_KEY_COUNT > 0

// Remove channels assignation if any
#if (SCKEY_P1_KEY_COUNT == 0)
#undef SCKEY_P1_CH
#if NUMBER_OF_MULTI_CHANNEL_KEYS == 0
#define SCKEY_P1_CH (0)
#elif NUMBER_OF_MULTI_CHANNEL_KEYS > 0
#if ((MCKEY1_A_CH == CH1) && (MCKEY1_B_CH == CH1) && (MCKEY1_C_CH == CH1))
#define SCKEY_P1_CH (CH1)
#elif ((MCKEY1_A_CH == CH2) && (MCKEY1_B_CH == CH2) && (MCKEY1_C_CH == CH2))
#define SCKEY_P1_CH (CH2)
#elif ((MCKEY1_A_CH == CH3) && (MCKEY1_B_CH == CH3) && (MCKEY1_C_CH == CH3))
#define SCKEY_P1_CH (CH3)
#elif ((MCKEY1_A_CH == CH4) && (MCKEY1_B_CH == CH4) && (MCKEY1_C_CH == CH4))
#define SCKEY_P1_CH (CH4)
#else
#define SCKEY_P1_CH  (MCKEY1_A_CH)
#endif
#endif
#endif

#if (SCKEY_P2_KEY_COUNT == 0)
#undef SCKEY_P2_CH
#if NUMBER_OF_MULTI_CHANNEL_KEYS < 2
#define SCKEY_P2_CH (0)
#elif NUMBER_OF_MULTI_CHANNEL_KEYS > 1
#if ((MCKEY2_A_CH == CH1) && (MCKEY2_B_CH == CH1) && (MCKEY2_C_CH == CH1))
#define SCKEY_P2_CH (CH1)
#elif ((MCKEY2_A_CH == CH2) && (MCKEY2_B_CH == CH2) && (MCKEY2_C_CH == CH2))
#define SCKEY_P2_CH (CH2)
#elif ((MCKEY2_A_CH == CH3) && (MCKEY2_B_CH == CH3) && (MCKEY2_C_CH == CH3))
#define SCKEY_P2_CH (CH3)
#elif ((MCKEY2_A_CH == CH4) && (MCKEY2_B_CH == CH4) && (MCKEY2_C_CH == CH4))
#define SCKEY_P2_CH (CH4)
#else
#define SCKEY_P2_CH (MCKEY1_B_CH)
#endif
#endif
#endif

#if (SCKEY_P3_KEY_COUNT == 0)
#undef SCKEY_P3_CH
#define SCKEY_P3_CH (0)
#endif

// Define acquisition "ports"
#define SCKEY_P1_ACQ  (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J)

#define SCKEY_P2_ACQ  (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J)

#define SCKEY_P3_ACQ  (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J)


//------------------------------------------------------------------------------
// Multi channel keys check and definitions
//------------------------------------------------------------------------------

// Check number of MCkeys
#if (NUMBER_OF_MULTI_CHANNEL_KEYS < 0) || (NUMBER_OF_MULTI_CHANNEL_KEYS > 2)
#error "Wrong number of multi channel keys. Must be in the [0..2] range."
#endif

// Check MCKeys type
#if (NUMBER_OF_MULTI_CHANNEL_KEYS > 0)
#if (MCKEY1_TYPE != 0) && (MCKEY1_TYPE != 1)
#error "Wrong MCKEY1 type. Must be equal to 0 (wheel) or 1 (slider)."
#endif
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0

#if (NUMBER_OF_MULTI_CHANNEL_KEYS > 1)
#if (MCKEY2_TYPE != 0) && (MCKEY2_TYPE != 1)
#error "Wrong MCKEY2 type. Must be equal to 0 (wheel) or 1 (slider)."
#endif
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 1

// PORT MASK definition
#if NUMBER_OF_MULTI_CHANNEL_KEYS == 0

#define MKEY_CH1_MASK (0)
#define MKEY_CH2_MASK (0)
#define MKEY_CH3_MASK (0)

#elif NUMBER_OF_MULTI_CHANNEL_KEYS == 1

#define MKEY_CH1_MASK \
  ( ((MCKEY1_A_CH == CH1)? MCKEY1_A:0)|((MCKEY1_B_CH == CH1)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH1)? MCKEY1_C:0) )
#define MKEY_CH2_MASK \
  ( ((MCKEY1_A_CH == CH2)? MCKEY1_A:0)|((MCKEY1_B_CH == CH2)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH2)? MCKEY1_C:0) )
#define MKEY_CH3_MASK \
  ( ((MCKEY1_A_CH == CH3)? MCKEY1_A:0)|((MCKEY1_B_CH == CH3)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH3)? MCKEY1_C:0) )
#define MKEY_CH4_MASK \
  ( ((MCKEY1_A_CH == CH4)? MCKEY1_A:0)|((MCKEY1_B_CH == CH4)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH4)? MCKEY1_C:0) )

#elif NUMBER_OF_MULTI_CHANNEL_KEYS == 2

#define MKEY_CH1_MASK \
  ( ((MCKEY1_A_CH == CH1)? MCKEY1_A:0)|((MCKEY1_B_CH == CH1)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH1)? MCKEY1_C:0)|\
    ((MCKEY2_A_CH == CH1)? MCKEY2_A:0)|((MCKEY2_B_CH == CH1)? MCKEY2_B:0)|\
    ((MCKEY2_C_CH == CH1)? MCKEY2_C:0) )
#define MKEY_CH2_MASK \
  ( ((MCKEY1_A_CH == CH2)? MCKEY1_A:0)|((MCKEY1_B_CH == CH2)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH2)? MCKEY1_C:0)|\
    ((MCKEY2_A_CH == CH2)? MCKEY2_A:0)|((MCKEY2_B_CH == CH2)? MCKEY2_B:0)|\
    ((MCKEY2_C_CH == CH2)? MCKEY2_C:0) )
#define MKEY_CH3_MASK \
  ( ((MCKEY1_A_CH == CH3)? MCKEY1_A:0)|((MCKEY1_B_CH == CH3)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH3)? MCKEY1_C:0)|\
    ((MCKEY2_A_CH == CH3)? MCKEY2_A:0)|((MCKEY2_B_CH == CH3)? MCKEY2_B:0)|\
    ((MCKEY2_C_CH == CH3)? MCKEY2_C:0) )
#define MKEY_CH4_MASK \
  ( ((MCKEY1_A_CH == CH4)? MCKEY1_A:0)|((MCKEY1_B_CH == CH4)? MCKEY1_B:0)|\
    ((MCKEY1_C_CH == CH4)? MCKEY1_C:0)|\
    ((MCKEY2_A_CH == CH4)? MCKEY2_A:0)|((MCKEY2_B_CH == CH4)? MCKEY2_B:0)|\
    ((MCKEY2_C_CH == CH4)? MCKEY2_C:0) )

#endif


//------------------------------------------------------------------------------
// Assign Comparators
//------------------------------------------------------------------------------

#ifdef COMP1
#undef COMP1
#endif

#ifdef COMP2
#undef COMP2
#endif

#if ((SCKEY_P1_ACQ & GROUP1) == GROUP1) || ((SCKEY_P1_ACQ & GROUP2) == GROUP2) || ((SCKEY_P1_ACQ & GROUP3) == GROUP3) ||\
    ((SCKEY_P2_ACQ & GROUP1) == GROUP1) || ((SCKEY_P2_ACQ & GROUP2) == GROUP2) || ((SCKEY_P2_ACQ & GROUP3) == GROUP3) ||\
    ((SCKEY_P3_ACQ & GROUP1) == GROUP1) || ((SCKEY_P3_ACQ & GROUP2) == GROUP2) || ((SCKEY_P3_ACQ & GROUP3) == GROUP3) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP1) == GROUP1) || ((ACTIVE_SHIELD_GROUP & GROUP2) == GROUP2) || ((ACTIVE_SHIELD_GROUP & GROUP3) == GROUP3) ||\
    ((SCKEY_P1_ACQ & GROUP7) == GROUP7) || ((SCKEY_P1_ACQ & GROUP8) == GROUP8) || ((SCKEY_P1_ACQ & GROUP9) == GROUP9) ||\
    ((SCKEY_P2_ACQ & GROUP7) == GROUP7) || ((SCKEY_P2_ACQ & GROUP8) == GROUP8) || ((SCKEY_P2_ACQ & GROUP9) == GROUP9) ||\
    ((SCKEY_P3_ACQ & GROUP7) == GROUP7) || ((SCKEY_P3_ACQ & GROUP8) == GROUP8) || ((SCKEY_P3_ACQ & GROUP9) == GROUP9) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP7) == GROUP7) || ((ACTIVE_SHIELD_GROUP & GROUP8) == GROUP8) || ((ACTIVE_SHIELD_GROUP & GROUP9) == GROUP9) ||\
    ((MKEY_CH1_MASK & GROUP7) == GROUP7) || ((MKEY_CH1_MASK & GROUP8) == GROUP8) || ((MKEY_CH1_MASK & GROUP9) == GROUP9) ||\
    ((MKEY_CH2_MASK & GROUP7) == GROUP7) || ((MKEY_CH2_MASK & GROUP8) == GROUP8) || ((MKEY_CH2_MASK & GROUP9) == GROUP9) ||\
    ((MKEY_CH3_MASK & GROUP7) == GROUP7) || ((MKEY_CH3_MASK & GROUP8) == GROUP8) || ((MKEY_CH3_MASK & GROUP9) == GROUP9) ||\
    ((MKEY_CH4_MASK & GROUP7) == GROUP7) || ((MKEY_CH4_MASK & GROUP8) == GROUP8) || ((MKEY_CH4_MASK & GROUP9) == GROUP9)
#define COMP1 (1)
#endif

#if ((SCKEY_P1_ACQ & GROUP4) == GROUP4) || ((SCKEY_P1_ACQ & GROUP5) == GROUP5) ||\
    ((SCKEY_P2_ACQ & GROUP4) == GROUP4) || ((SCKEY_P2_ACQ & GROUP5) == GROUP5) ||\
    ((SCKEY_P3_ACQ & GROUP4) == GROUP4) || ((SCKEY_P3_ACQ & GROUP5) == GROUP5) ||\
    ((SCKEY_P1_ACQ & GROUP6) == GROUP6) || ((SCKEY_P1_ACQ & GROUP10) == GROUP10) ||\
    ((SCKEY_P2_ACQ & GROUP6) == GROUP6) || ((SCKEY_P2_ACQ & GROUP10) == GROUP10) ||\
    ((SCKEY_P3_ACQ & GROUP6) == GROUP6) || ((SCKEY_P3_ACQ & GROUP10) == GROUP10) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP4) == GROUP4) || ((ACTIVE_SHIELD_GROUP & GROUP5) == GROUP5) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP6) == GROUP6) || ((ACTIVE_SHIELD_GROUP & GROUP10) == GROUP10) ||\
    ((MKEY_CH1_MASK & GROUP4) == GROUP4) || ((MKEY_CH1_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH2_MASK & GROUP4) == GROUP4) || ((MKEY_CH2_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH3_MASK & GROUP4) == GROUP4) || ((MKEY_CH3_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH4_MASK & GROUP4) == GROUP4) || ((MKEY_CH4_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH1_MASK & GROUP6) == GROUP6) || ((MKEY_CH1_MASK & GROUP10) == GROUP10) ||\
    ((MKEY_CH2_MASK & GROUP6) == GROUP6) || ((MKEY_CH2_MASK & GROUP10) == GROUP10) ||\
    ((MKEY_CH3_MASK & GROUP6) == GROUP6) || ((MKEY_CH3_MASK & GROUP10) == GROUP10) ||\
    ((MKEY_CH4_MASK & GROUP6) == GROUP6) || ((MKEY_CH4_MASK & GROUP10) == GROUP10)
#define COMP2 (1)
#endif


//------------------------------------------------------------------------------
// Assign PORTS
//------------------------------------------------------------------------------

#ifdef PORT_A
#undef PORT_A
#endif

#ifdef PORT_B
#undef PORT_B
#endif

#ifdef PORT_C
#undef PORT_C
#endif

#if ((SCKEY_P1_ACQ & GROUP1) == GROUP1) || ((SCKEY_P1_ACQ & GROUP2) == GROUP2) ||\
    ((SCKEY_P2_ACQ & GROUP1) == GROUP1) || ((SCKEY_P2_ACQ & GROUP2) == GROUP2) ||\
    ((SCKEY_P3_ACQ & GROUP1) == GROUP1) || ((SCKEY_P3_ACQ & GROUP2) == GROUP2) ||\
    ((SCKEY_P1_ACQ & GROUP4) == GROUP4) || ((SCKEY_P1_ACQ & GROUP5) == GROUP5) ||\
    ((SCKEY_P2_ACQ & GROUP4) == GROUP4) || ((SCKEY_P2_ACQ & GROUP5) == GROUP5) ||\
    ((SCKEY_P3_ACQ & GROUP4) == GROUP4) || ((SCKEY_P3_ACQ & GROUP5) == GROUP5) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP1) == GROUP1) || ((ACTIVE_SHIELD_GROUP & GROUP2) == GROUP2) ||\
    ((ACTIVE_SHIELD_GROUP & GROUP4) == GROUP4) || ((ACTIVE_SHIELD_GROUP & GROUP5) == GROUP5) ||\
    ((MKEY_CH1_MASK & GROUP1) == GROUP1) || ((MKEY_CH1_MASK & GROUP2) == GROUP2) ||\
    ((MKEY_CH2_MASK & GROUP1) == GROUP1) || ((MKEY_CH2_MASK & GROUP2) == GROUP2) ||\
    ((MKEY_CH3_MASK & GROUP1) == GROUP1) || ((MKEY_CH3_MASK & GROUP2) == GROUP2) ||\
    ((MKEY_CH4_MASK & GROUP1) == GROUP1) || ((MKEY_CH4_MASK & GROUP2) == GROUP2) ||\
    ((MKEY_CH1_MASK & GROUP4) == GROUP4) || ((MKEY_CH1_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH2_MASK & GROUP4) == GROUP4) || ((MKEY_CH2_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH3_MASK & GROUP4) == GROUP4) || ((MKEY_CH3_MASK & GROUP5) == GROUP5) ||\
    ((MKEY_CH4_MASK & GROUP4) == GROUP4) || ((MKEY_CH4_MASK & GROUP5) == GROUP5)
#define PORT_A (1)
#endif

#if ((SCKEY_P1_ACQ & GROUP3) == GROUP3) || ((SCKEY_P1_ACQ & GROUP6) == GROUP6) || ((SCKEY_P1_ACQ & GROUP7) == GROUP7)||\
    ((SCKEY_P2_ACQ & GROUP3) == GROUP3) || ((SCKEY_P2_ACQ & GROUP6) == GROUP6) || ((SCKEY_P2_ACQ & GROUP7) == GROUP7)||\
    ((SCKEY_P3_ACQ & GROUP3) == GROUP3) || ((SCKEY_P3_ACQ & GROUP6) == GROUP6) || ((SCKEY_P3_ACQ & GROUP7) == GROUP7)||\
    ((ACTIVE_SHIELD_GROUP & GROUP3) == GROUP3) || ((ACTIVE_SHIELD_GROUP & GROUP6) == GROUP6) || ((ACTIVE_SHIELD_GROUP & GROUP7) == GROUP7) ||\
    ((MKEY_CH1_MASK & GROUP3) == GROUP3) || ((MKEY_CH1_MASK & GROUP6) == GROUP6) || ((MKEY_CH1_MASK & GROUP7) == GROUP7)||\
    ((MKEY_CH2_MASK & GROUP3) == GROUP3) || ((MKEY_CH2_MASK & GROUP6) == GROUP6) || ((MKEY_CH2_MASK & GROUP7) == GROUP7)||\
    ((MKEY_CH3_MASK & GROUP3) == GROUP3) || ((MKEY_CH3_MASK & GROUP6) == GROUP6) || ((MKEY_CH3_MASK & GROUP7) == GROUP7)||\
    ((MKEY_CH4_MASK & GROUP3) == GROUP3) || ((MKEY_CH4_MASK & GROUP6) == GROUP6) || ((MKEY_CH4_MASK & GROUP7) == GROUP7)
#define PORT_B (1)
#endif

#if ((SCKEY_P1_ACQ & GROUP8) == GROUP8) || ((SCKEY_P1_ACQ & GROUP9) == GROUP9) || ((SCKEY_P1_ACQ & GROUP10) == GROUP10)||\
    ((SCKEY_P2_ACQ & GROUP8) == GROUP8) || ((SCKEY_P2_ACQ & GROUP9) == GROUP9) || ((SCKEY_P2_ACQ & GROUP10) == GROUP10)||\
    ((SCKEY_P3_ACQ & GROUP8) == GROUP8) || ((SCKEY_P3_ACQ & GROUP9) == GROUP9) || ((SCKEY_P3_ACQ & GROUP10) == GROUP10)||\
    ((ACTIVE_SHIELD_GROUP & GROUP8) == GROUP8) || ((ACTIVE_SHIELD_GROUP & GROUP9) == GROUP9) || ((ACTIVE_SHIELD_GROUP & GROUP10) == GROUP10)||\
    ((MKEY_CH1_MASK & GROUP8) == GROUP8) || ((MKEY_CH1_MASK & GROUP9) == GROUP9) || ((MKEY_CH1_MASK & GROUP10) == GROUP10)||\
    ((MKEY_CH2_MASK & GROUP8) == GROUP8) || ((MKEY_CH2_MASK & GROUP9) == GROUP9) || ((MKEY_CH2_MASK & GROUP10) == GROUP10)||\
    ((MKEY_CH3_MASK & GROUP8) == GROUP8) || ((MKEY_CH3_MASK & GROUP9) == GROUP9) || ((MKEY_CH3_MASK & GROUP10) == GROUP10)||\
    ((MKEY_CH4_MASK & GROUP8) == GROUP8) || ((MKEY_CH4_MASK & GROUP9) == GROUP9) || ((MKEY_CH4_MASK & GROUP10) == GROUP10)
#define PORT_C (1)
#endif


//------------------------------------------------------------------------------
// Check sampling capacitor channel settings
//------------------------------------------------------------------------------

#if ((SAMP_CAP_CH != CH1) && (SAMP_CAP_CH != CH2) && (SAMP_CAP_CH != CH3) && (SAMP_CAP_CH != CH4))
#error "Wrong channel assigment: SAMP_CAP_CH must be defined to CH1, CH2, CH3 or CH4 only"
#endif

#if (SCKEY_P1_CH == SAMP_CAP_CH)
#error "Wrong channel assigment: SCKEY_P1_CH and SAMP_CAP_CH must be different"
#endif

#if (SCKEY_P2_CH == SAMP_CAP_CH)
#error "Wrong channel assigment: SCKEY_P2_CH and SAMP_CAP_CH must be different"
#endif

#if (SCKEY_P3_CH == SAMP_CAP_CH)
#error "Wrong channel assigment: SCKEY_P3_CH and SAMP_CAP_CH must be different"
#endif

//------------------------------------------------------------------------------
// Check sampling capacitor channel settings versus the pin used to shield
//------------------------------------------------------------------------------

#define SHIELD_MODER_MASK_A_OUT  (0)
#define SHIELD_MODER_MASK_A      (0)
#define SHIELD_IO_MASK_A         (0)
#define SHIELD_MODER_MASK_B_OUT  (0)
#define SHIELD_MODER_MASK_B      (0)
#define SHIELD_IO_MASK_B         (0)
#define SHIELD_MODER_MASK_C_OUT  (0)
#define SHIELD_MODER_MASK_C      (0)
#define SHIELD_IO_MASK_C         (0)
#define SHIELD_COMP1_MASK        (0)
#define SHIELD_COMP2_MASK        (0)

#if (ACTIVE_SHIELD_CHANNEL > 0)

#if (ACTIVE_SHIELD_CHANNEL != CH1) && (ACTIVE_SHIELD_CHANNEL != CH2) && (ACTIVE_SHIELD_CHANNEL != CH3) && (ACTIVE_SHIELD_CHANNEL != CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1, CH2, CH3 or CH4!"
#endif

#if (ACTIVE_SHIELD_CHANNEL == SAMP_CAP_CH)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be different with SAMP_CAP_CH!"
#endif

#if (ACTIVE_SHIELD_GROUP == GROUP1)
#undef SHIELD_MODER_MASK_A_OUT
#undef SHIELD_MODER_MASK_A
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_A
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00000001)   /* PA0 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x00000003)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000001)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000001)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00000004)   /* PA1 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x0000000C)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000002)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000002)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00000010)   /* PA2 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x00000030)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000004)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000004)
#elif (ACTIVE_SHIELD_CHANNEL == CH4)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00000040)   /* PA3 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x000000C0)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000008)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000008)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP2)
#if (ACTIVE_SHIELD_CHANNEL == CH3) && (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1 or CH2 with GROUP2!"
#endif
#undef SHIELD_MODER_MASK_A_OUT
#undef SHIELD_MODER_MASK_A
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_A
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00001000)   /* PA6 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x00003000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000040)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000040)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00004000)   /* PA7 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x0000C000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000080)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000080)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP3)
#if (ACTIVE_SHIELD_CHANNEL == CH3) && (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1 or CH2 with GROUP3!"
#endif
#undef SHIELD_MODER_MASK_B_OUT
#undef SHIELD_MODER_MASK_B
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_B
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x00000001)   /* PB0 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x00000003)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000100)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00000001)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x00000004)   /* PB1 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x0000000C)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000200)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00000002)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP4)
#if (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1, CH2 or CH3 with GROUP4!"
#endif
#undef SHIELD_MODER_MASK_A_OUT
#undef SHIELD_MODER_MASK_A
#undef SHIELD_COMP2_MASK
#undef SHIELD_IO_MASK_A
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00010000)   /* PA8 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x00030000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000200)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000100)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00040000)   /* PA9 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x000C0000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000400)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000200)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x00100000)   /* PA10 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x00300000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000800)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00000400)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP5)
#if (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1, CH2 or CH3 with GROUP5!"
#endif
#undef SHIELD_MODER_MASK_A_OUT
#undef SHIELD_MODER_MASK_A
#undef SHIELD_COMP2_MASK
#undef SHIELD_IO_MASK_A
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x04000000)   /* PA13 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x0C000000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000040)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00002000)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x10000000)   /* PA14 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0x30000000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000080)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00004000)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_A_OUT  ((uint32_t)0x40000000)   /* PA15 */
#define SHIELD_MODER_MASK_A      ((uint32_t)0xC0000000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000100)
#define SHIELD_IO_MASK_A         ((uint32_t)0x00008000)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP6)
#if (ACTIVE_SHIELD_CHANNEL == CH3) || (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1 or CH2 with GROUP6!"
#endif
#undef SHIELD_MODER_MASK_B_OUT
#undef SHIELD_MODER_MASK_B
#undef SHIELD_COMP2_MASK
#undef SHIELD_IO_MASK_B
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x00000100)   /* PB4 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x00000300)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000010)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00000010)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x00000400)   /* PB5 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x00000C00)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000020)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00000020)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP7)
#undef SHIELD_MODER_MASK_B_OUT
#undef SHIELD_MODER_MASK_B
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_B
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x01000000)   /* PB12 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x03000000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00040000)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00001000)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x04000000)   /* PB13 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x0C000000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00080000)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00002000)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x10000000)   /* PB14 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0x30000000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00100000)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00004000)
#elif (ACTIVE_SHIELD_CHANNEL == CH4)
#define SHIELD_MODER_MASK_B_OUT  ((uint32_t)0x40000000)   /* PB15 */
#define SHIELD_MODER_MASK_B      ((uint32_t)0xC0000000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00200000)
#define SHIELD_IO_MASK_B         ((uint32_t)0x00008000)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP8)
#undef SHIELD_MODER_MASK_C_OUT
#undef SHIELD_MODER_MASK_C
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_C
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000001)   /* PC0 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00000003)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000400)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000001)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000004)   /* PC1 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x0000000C)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000800)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000002)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000010)   /* PC2 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00000030)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00001000)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000004)
#elif (ACTIVE_SHIELD_CHANNEL == CH4)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000040)   /* PC3 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x000000C0)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00002000)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000008)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP9)
#if (ACTIVE_SHIELD_CHANNEL == CH3) || (ACTIVE_SHIELD_CHANNEL == CH4)
#error "Wrong channel assigment: ACTIVE_SHIELD_CHANNEL must be CH1 or CH2 with GROUP9!"
#endif
#undef SHIELD_MODER_MASK_C_OUT
#undef SHIELD_MODER_MASK_C
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_C
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000100)   /* PC4 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00000300)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00004000)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000010)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00000400)   /* PC5 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00000C00)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00008000)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000020)
#endif
#elif (ACTIVE_SHIELD_GROUP == GROUP10)
#undef SHIELD_MODER_MASK_C_OUT
#undef SHIELD_MODER_MASK_C
#undef SHIELD_COMP1_MASK
#undef SHIELD_IO_MASK_C
#if (ACTIVE_SHIELD_CHANNEL == CH1)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00001000)   /* PC6 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00003000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000001)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000040)
#elif (ACTIVE_SHIELD_CHANNEL == CH2)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00004000)   /* PC7 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x0000C000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000002)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000080)
#elif (ACTIVE_SHIELD_CHANNEL == CH3)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00010000)   /* PC8 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x00030000)
#define SHIELD_COMP1_MASK        ((uint32_t)0x00000004)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000100)
#elif (ACTIVE_SHIELD_CHANNEL == CH4)
#define SHIELD_MODER_MASK_C_OUT  ((uint32_t)0x00040000)   /* PC9 */
#define SHIELD_MODER_MASK_C      ((uint32_t)0x000C0000)
#define SHIELD_COMP2_MASK        ((uint32_t)0x00000008)
#define SHIELD_IO_MASK_C         ((uint32_t)0x00000200)
#endif
#endif /* ACTIVE_SHIELD_GROUP */
#endif /* ACTIVE_SHIELD_CHANNEL */

//----------------------------------------------------------------------------
// Acquisition slot selection
//----------------------------------------------------------------------------

#if (NUMBER_OF_MULTI_CHANNEL_KEYS == 0)
#define MCKEY1_USE_P1 (0)
#define MCKEY1_USE_P2 (0)
#define MCKEY1_USE_P3 (0)
#define MCKEY2_USE_P1 (0)
#define MCKEY2_USE_P2 (0)
#define MCKEY2_USE_P3 (0)
#endif

#if (NUMBER_OF_MULTI_CHANNEL_KEYS > 0)
#define MCKEY1_USE_P1 (((MCKEY1_A_CH == SCKEY_P1_CH)?1:0) | ((MCKEY1_B_CH == SCKEY_P1_CH)?1:0) | ((MCKEY1_C_CH == SCKEY_P1_CH)?1:0))
#define MCKEY1_USE_P2 (((MCKEY1_A_CH == SCKEY_P2_CH)?1:0) | ((MCKEY1_B_CH == SCKEY_P2_CH)?1:0) | ((MCKEY1_C_CH == SCKEY_P2_CH)?1:0))
#define MCKEY1_USE_P3 (((MCKEY1_A_CH == SCKEY_P3_CH)?1:0) | ((MCKEY1_B_CH == SCKEY_P3_CH)?1:0) | ((MCKEY1_C_CH == SCKEY_P3_CH)?1:0))
#if (NUMBER_OF_MULTI_CHANNEL_KEYS > 1)
#define MCKEY2_USE_P1 (((MCKEY2_A_CH == SCKEY_P1_CH)?1:0) | ((MCKEY2_B_CH == SCKEY_P1_CH)?1:0) | ((MCKEY2_C_CH == SCKEY_P1_CH)?1:0))
#define MCKEY2_USE_P2 (((MCKEY2_A_CH == SCKEY_P2_CH)?1:0) | ((MCKEY2_B_CH == SCKEY_P2_CH)?1:0) | ((MCKEY2_C_CH == SCKEY_P2_CH)?1:0))
#define MCKEY2_USE_P3 (((MCKEY2_A_CH == SCKEY_P3_CH)?1:0) | ((MCKEY2_B_CH == SCKEY_P3_CH)?1:0) | ((MCKEY2_C_CH == SCKEY_P3_CH)?1:0))
#else
#define MCKEY2_USE_P1 (0)
#define MCKEY2_USE_P2 (0)
#define MCKEY2_USE_P3 (0)
#endif
#endif // (NUMBER_OF_MULTI_CHANNEL_KEYS > 0)

#if (SCKEY_P2_KEY_COUNT > 0) || (MCKEY1_USE_P2 > 0) || (MCKEY2_USE_P2 > 0)
#if (SCKEY_P3_KEY_COUNT > 0) || (MCKEY1_USE_P3 > 0) || (MCKEY2_USE_P3 > 0)
#define NUMBER_OF_ACQUISITION_PORTS (3)
#else
#define NUMBER_OF_ACQUISITION_PORTS (2)
#endif
#else
#define NUMBER_OF_ACQUISITION_PORTS (1)  // always a minimum of one "port" is used
#endif

#if (NUMBER_OF_SINGLE_CHANNEL_KEYS == 0) && (NUMBER_OF_MULTI_CHANNEL_KEYS == 0)
#error "At least one key must be defined !"
#endif


//==============================================================================
// TSL parameters check
//==============================================================================

#if !defined(NEGDETECT_AUTOCAL)
#error "Please define NEGDETECT_AUTOCAL (with value at 0 or 1)"
#endif

#if !defined(SPREAD_SPECTRUM)
#error "Please define SPREAD_SPECTRUM (with value at 0 or 1)"
#endif

#ifdef SW_SPREAD_SPECTRUM
#if !SPREAD_SPECTRUM && SW_SPREAD_SPECTRUM
#error "SW_SPREAD_SPECTRUM can only be enabled if SPREAD_SPECTRUM is enabled !"
#endif
#endif

#if (SPREAD_SPECTRUM && defined(STM32L15XX8B))
#if !SPREAD_COUNTER_MIN
#error " The SPREAD_COUNTER_MIN value must not be null !"
#endif
#endif

#if SPREAD_SPECTRUM
#if (SPREAD_COUNTER_MAX > 255) || (SPREAD_COUNTER_MAX <= SPREAD_COUNTER_MIN)
#error " The SPREAD_COUNTER_MAX value must be a 8-bit value in the range of [2 - 255] !"
#endif
#endif

#if (ACTIVE_SHIELD_GROUP != 0)
#if (ACTIVE_SHIELD_GROUP == SCKEY_P1_A) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_A) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_A) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_B) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_B) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_B) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_C) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_C) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_C) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_D) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_D) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_D) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_E) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_E) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_E) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_F) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_F) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_F) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_G) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_G) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_G) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_H) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_H) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_H) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_I) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_I) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_I) || \
    (ACTIVE_SHIELD_GROUP == SCKEY_P1_J) || (ACTIVE_SHIELD_GROUP == SCKEY_P2_J) || (ACTIVE_SHIELD_GROUP == SCKEY_P3_J) || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_A)   || (ACTIVE_SHIELD_GROUP == MCKEY2_A)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_B)   || (ACTIVE_SHIELD_GROUP == MCKEY2_B)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_C)   || (ACTIVE_SHIELD_GROUP == MCKEY2_C)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_D)   || (ACTIVE_SHIELD_GROUP == MCKEY2_D)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_E)   || (ACTIVE_SHIELD_GROUP == MCKEY2_E)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_F)   || (ACTIVE_SHIELD_GROUP == MCKEY2_F)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_G)   || (ACTIVE_SHIELD_GROUP == MCKEY2_G)   || \
 (ACTIVE_SHIELD_GROUP == MCKEY1_H)   || (ACTIVE_SHIELD_GROUP == MCKEY2_H)
#error "ACTIVE_SHIELD_GROUP cannot be a group used for a key !"
#endif
#endif

#endif /* __TSL_CHECKCONFIG_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

