/**
  ******************************************************************************
  * @file    stm32l15x_tsl_ct_acquisition.c
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file provides all the functions
  *         to manage the Charge-Transfer acquisition based on STM32L15x devices
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l15x_tsl_ct_acquisition.h"
#include "stm32_tsl_services.h"
#include "stm32l1xx_conf.h"

/* This file must be used with STM32L15xx8 (64kb) or STM32L15xxB (128kb) devices only */

#if defined(STM32L15XX8B)

#if defined ( __CC_ARM ) || defined( __GNUC__ ) // KEIL
#define TSL_NOP   __NOP();
#elif defined ( __ICCARM__ ) // IAR
#define TSL_NOP   __ASM("nop\n");
#endif

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Define the 1st Channel mask */
#define SCKEY_P1_MASK (0)

#if (SCKEY_P1_CH == CH1)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH1_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P1_CH == CH1)

#if (SCKEY_P1_CH == CH2)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH2_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P1_CH == CH2)

#if (SCKEY_P1_CH == CH3)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH3_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P1_CH == CH3)

#if (SCKEY_P1_CH == CH4)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH4_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P1_CH == CH4)

/* Define the 2nd Channel mask */
#if (NUMBER_OF_ACQUISITION_PORTS > 1)

#define SCKEY_P2_MASK (0)

#if (SCKEY_P2_CH == CH1)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH1_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH1)

#if (SCKEY_P2_CH == CH2)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH2_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH2)

#if (SCKEY_P2_CH == CH3)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH3_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH3)

#if (SCKEY_P2_CH == CH4)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH4_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH4)

#endif
//# (NUMBER_OF_ACQUISITION_PORTS > 1)

/* Define the 3rd Channel mask */
#if (NUMBER_OF_ACQUISITION_PORTS > 2)

#define SCKEY_P3_MASK (0)

#if (SCKEY_P3_CH == CH1)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH1_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH1)

#if (SCKEY_P3_CH == CH2)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH2_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH2)

#if (SCKEY_P3_CH == CH3)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH3_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH3)

#if (SCKEY_P3_CH == CH4)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH4_MASK |\
                       ACTIVE_SHIELD_GROUP)
#endif
//# (SCKEY_P2_CH == CH4)

#endif
//# (NUMBER_OF_ACQUISITION_PORTS > 2)

/* Define all channels mask */
#define SCKEY_MASK (SCKEY_P1_MASK | SCKEY_P2_MASK | SCKEY_P3_MASK)
#define SCKEY_MASK_G1 (SCKEY_MASK & GROUP1)
#define SCKEY_MASK_G2 (SCKEY_MASK & GROUP2)
#define SCKEY_MASK_G3 (SCKEY_MASK & GROUP3)
#define SCKEY_MASK_G4 (SCKEY_MASK & GROUP4)
#define SCKEY_MASK_G5 (SCKEY_MASK & GROUP5)
#define SCKEY_MASK_G6 (SCKEY_MASK & GROUP6)
#define SCKEY_MASK_G7 (SCKEY_MASK & GROUP7)
#define SCKEY_MASK_G8 (SCKEY_MASK & GROUP8)
#define SCKEY_MASK_G9 (SCKEY_MASK & GROUP9)
#define SCKEY_MASK_G10 (SCKEY_MASK & GROUP10)

/* Initialize the SAMPLING capacitor COMP1 masks & COMP2 masks */
#if !defined(PORT_A)
#define SAMP_CAP_COMP1_MASK_A ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MASK_A ((uint32_t)0x00000000)
#endif
//# !defined(PORT_A)

#if !defined(PORT_B)
#define SAMP_CAP_COMP1_MASK_B ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MASK_B ((uint32_t)0x00000000)
#endif
//# !defined(PORT_B)

#if !defined(PORT_C)
#define SAMP_CAP_COMP1_MASK_C ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MASK_C ((uint32_t)0x00000000)
#endif
//# !defined(PORT_C)

/* Define the SAMPLING capacitor comparator channel mask */
#if (SAMP_CAP_CH == CH1)

#ifdef PORT_A
#define SAMP_CAP_PORT_A     (GPIOA)
#define SAMP_CAP_IO_MASK_1  ((uint32_t)0x00000001)   /* PA0 */
#define SAMP_CAP_IO_MASK_2  ((uint32_t)0x00000040)   /* PA6 */
#define SAMP_CAP_IO_MASK_4  ((uint32_t)0x00000100)   /* PA8 */
#define SAMP_CAP_IO_MASK_5  ((uint32_t)0x00002000)   /* PA13 */
#ifdef COMP1
#if(SCKEY_MASK_G1==GROUP1)&&(SCKEY_MASK_G2==GROUP2)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000041)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000041)  /* PA0 and PA6 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00003003)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00001001)
#elif(SCKEY_MASK_G1==GROUP1)&&(SCKEY_MASK_G2==0)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000001)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000001)  /* PA0 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000003)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000001)
#elif (SCKEY_MASK_G1==0)&&(SCKEY_MASK_G2==GROUP2)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000040)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000040)  /* PA6 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00003000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00001000)
#else
#define SAMP_CAP_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#if(SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000240)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00002100) /* PA8 and PA13 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x0C030000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x04010000)
#elif (SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==0)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000200)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000100) /* PA8 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00030000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00010000)
#elif (SCKEY_MASK_G4==0)&&(SCKEY_MASK_G5==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000040)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00002000) /* PA13 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x0C000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x04000000)
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_A         ((SAMP_CAP_COMP1_IO_MASK_A)|(SAMP_CAP_COMP2_IO_MASK_A))
#define SAMP_CAP_MODER_MASK_A      ((SAMP_CAP_COMP1_MODER_MASK_A)|(SAMP_CAP_COMP2_MODER_MASK_A))
#define SAMP_CAP_MODER_MASK_A_OUT  ((SAMP_CAP_COMP1_MODER_MASK_A_OUT)|(SAMP_CAP_COMP2_MODER_MASK_A_OUT))
#endif
//# PORT_A

#ifdef PORT_B
#define SAMP_CAP_PORT_B     (GPIOB)
#define SAMP_CAP_IO_MASK_3  ((uint32_t)0x00000001)   /* PB0 */
#define SAMP_CAP_IO_MASK_6  ((uint32_t)0x00000010)   /* PB4 */
#define SAMP_CAP_IO_MASK_7  ((uint32_t)0x00001000)   /* PB12 */
#ifdef COMP1
#if (SCKEY_MASK_G3==GROUP3)&&(SCKEY_MASK_G7==GROUP7)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00040100)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00001001)  /* PB0 and PB12 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x03000003)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x01000001)
#elif (SCKEY_MASK_G3==GROUP3)&&(SCKEY_MASK_G7==0)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00000100)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000001)  /* PB0 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x00000003)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000001)
#elif (SCKEY_MASK_G3==0)&&(SCKEY_MASK_G7==GROUP7)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00040000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00001000)  /* PB12 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x03000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x01000000)
#else
#define SAMP_CAP_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#if (SCKEY_MASK_G6==GROUP6)
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000010)
#define SAMP_CAP_COMP2_IO_MASK_B        ((uint32_t)0x00000010) /* PB4 */
#define SAMP_CAP_COMP2_MODER_MASK_B     ((uint32_t)0x00000300)
#define SAMP_CAP_COMP2_MODER_MASK_B_OUT ((uint32_t)0x00000100)
#else
#define SAMP_CAP_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_B         ((SAMP_CAP_COMP1_IO_MASK_B)|(SAMP_CAP_COMP2_IO_MASK_B))
#define SAMP_CAP_MODER_MASK_B      ((SAMP_CAP_COMP1_MODER_MASK_B)|(SAMP_CAP_COMP2_MODER_MASK_B))
#define SAMP_CAP_MODER_MASK_B_OUT  ((SAMP_CAP_COMP1_MODER_MASK_B_OUT)|(SAMP_CAP_COMP2_MODER_MASK_B_OUT))
#endif
//# PORT_B

#ifdef PORT_C
#define SAMP_CAP_PORT_C     (GPIOC)
#define SAMP_CAP_IO_MASK_8  ((uint32_t)0x00000001)   /* PC0 */
#define SAMP_CAP_IO_MASK_9  ((uint32_t)0x00000010)  /* PC4 */
#define SAMP_CAP_IO_MASK_10 ((uint32_t)0x00000040) /* PC6 */
#ifdef COMP1
#if (SCKEY_MASK_G8==GROUP8)&&(SCKEY_MASK_G9==GROUP9)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00004400)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000011)  /* PC0 and PC4 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000303)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000101)
#elif (SCKEY_MASK_G8==GROUP8)&&(SCKEY_MASK_G9==0)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00004000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000001)  /* PC0 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000003)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000001)
#elif (SCKEY_MASK_G8==0)&&(SCKEY_MASK_G9==GROUP9)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00004000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000010)  /* PC4 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000300)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000100)
#else
#define SAMP_CAP_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#if (SCKEY_MASK_G10==GROUP10)
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000001)
#define SAMP_CAP_COMP2_IO_MASK_C        ((uint32_t)0x00000040) /* PC6 */
#define SAMP_CAP_COMP2_MODER_MASK_C     ((uint32_t)0x00003000)
#define SAMP_CAP_COMP2_MODER_MASK_C_OUT ((uint32_t)0x00001000)
#else
#define SAMP_CAP_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_C         ((SAMP_CAP_COMP1_IO_MASK_C)|(SAMP_CAP_COMP2_IO_MASK_C))
#define SAMP_CAP_MODER_MASK_C      ((SAMP_CAP_COMP1_MODER_MASK_C)|(SAMP_CAP_COMP2_MODER_MASK_C))
#define SAMP_CAP_MODER_MASK_C_OUT  ((SAMP_CAP_COMP1_MODER_MASK_C_OUT)|(SAMP_CAP_COMP2_MODER_MASK_C_OUT))
#endif
//# PORT_C

#endif
//# (SAMP_CAP_CH == CH1)

#if (SAMP_CAP_CH == CH2)

#ifdef PORT_A
#define SAMP_CAP_PORT_A     (GPIOA)
#define SAMP_CAP_IO_MASK_1  ((uint32_t)0x00000002)   /* PA1 */
#define SAMP_CAP_IO_MASK_2  ((uint32_t)0x00000080)   /* PA7 */
#define SAMP_CAP_IO_MASK_4  ((uint32_t)0x00000200)   /* PA9 */
#define SAMP_CAP_IO_MASK_5  ((uint32_t)0x00004000)   /* PA14 */
#ifdef COMP1
#if (SCKEY_MASK_G1==GROUP1)&&(SCKEY_MASK_G2==GROUP2)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000082)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000082)  /*PA1 and PA7*/
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x0000C00C)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00004004)
#elif (SCKEY_MASK_G1==GROUP1)&&(SCKEY_MASK_G2==0)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000002)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000002)  /* PA1 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x0000000C)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000004)
#elif (SCKEY_MASK_G1==0)&&(SCKEY_MASK_G2==GROUP2)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000080)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000080)  /* PA7 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x0000C000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00004000)
#else
#define SAMP_CAP_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#if (SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000480)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00004200) /* PA9 and PA14 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x300C0000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x10040000)
#elif (SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==0)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000080)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000200) /* PA9 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x000C0000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00040000)
#elif (SCKEY_MASK_G4==0)&&(SCKEY_MASK_G2==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000080)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00004200) /* PA14 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x30000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x10000000)
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_A         ((SAMP_CAP_COMP1_IO_MASK_A)|(SAMP_CAP_COMP2_IO_MASK_A))
#define SAMP_CAP_MODER_MASK_A      ((SAMP_CAP_COMP1_MODER_MASK_A)|(SAMP_CAP_COMP2_MODER_MASK_A))
#define SAMP_CAP_MODER_MASK_A_OUT  ((SAMP_CAP_COMP1_MODER_MASK_A_OUT)|(SAMP_CAP_COMP2_MODER_MASK_A_OUT))
#endif
//# PORT_A

#ifdef PORT_B
#define SAMP_CAP_PORT_B     (GPIOB)
#define SAMP_CAP_IO_MASK_3  ((uint32_t)0x00000002)   /* PB1 */
#define SAMP_CAP_IO_MASK_7  ((uint32_t)0x00002000)   /* PB13 */
#define SAMP_CAP_IO_MASK_6  ((uint32_t)0x00000020)   /* PB5 */
#ifdef COMP1
#if (SCKEY_MASK_G3==GROUP3)&&(SCKEY_MASK_G7==GROUP7)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00080200)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00002002)  /* PB1 and PB13 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x0C00000C)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x04000004)
#elif (SCKEY_MASK_G3==GROUP3)&&(SCKEY_MASK_G7==0)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00000200)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000002)  /* PB1 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x0000000C)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000004)
#elif (SCKEY_MASK_G3==0)&&(SCKEY_MASK_G7==GROUP7)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00080000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00002000)  /* PB13 */
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x0C000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x04000000)
#else
#define SAMP_CAP_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000020)
#define SAMP_CAP_COMP2_IO_MASK_B        ((uint32_t)0x00000020) /* PB5 */
#define SAMP_CAP_COMP2_MODER_MASK_B     ((uint32_t)0x00000C00)
#define SAMP_CAP_COMP2_MODER_MASK_B_OUT ((uint32_t)0x00000400)
#else
#define SAMP_CAP_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_B         ((SAMP_CAP_COMP1_IO_MASK_B)|(SAMP_CAP_COMP2_IO_MASK_B))
#define SAMP_CAP_MODER_MASK_B      ((SAMP_CAP_COMP1_MODER_MASK_B)|(SAMP_CAP_COMP2_MODER_MASK_B))
#define SAMP_CAP_MODER_MASK_B_OUT  ((SAMP_CAP_COMP1_MODER_MASK_B_OUT)|(SAMP_CAP_COMP2_MODER_MASK_B_OUT))
#endif
//# PORT_B

#ifdef PORT_C
#define SAMP_CAP_PORT_C     (GPIOC)
#define SAMP_CAP_IO_MASK_8  ((uint32_t)0x00000002)   /* PC1 */
#define SAMP_CAP_IO_MASK_9  ((uint32_t)0x00000020)  /* PC5 */
#define SAMP_CAP_IO_MASK_10 ((uint32_t)0x00000080) /* PC7 */

#ifdef COMP1
#if (SCKEY_MASK_G8==GROUP8)&&(SCKEY_MASK_G9==GROUP9)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00008800)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000022)  /* PC1 and PC5 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000C0C)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000404)
#elif (SCKEY_MASK_G8==GROUP8)&&(SCKEY_MASK_G9==0)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00000800)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000002)  /* PC1 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x0000000C)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000004)
#elif (SCKEY_MASK_G8==0)&&(SCKEY_MASK_G9==GROUP9)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00008000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000020)  /* PC5 */
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000C00)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000400)
#else
#define SAMP_CAP_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000002)
#define SAMP_CAP_COMP2_IO_MASK_C        ((uint32_t)0x00000080) /* PC7 */
#define SAMP_CAP_COMP2_MODER_MASK_C     ((uint32_t)0x0000C000)
#define SAMP_CAP_COMP2_MODER_MASK_C_OUT ((uint32_t)0x00004000)
#else
#define SAMP_CAP_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_C        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_C_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_C         ((SAMP_CAP_COMP1_IO_MASK_C)|(SAMP_CAP_COMP2_IO_MASK_C))
#define SAMP_CAP_MODER_MASK_C      ((SAMP_CAP_COMP1_MODER_MASK_C)|(SAMP_CAP_COMP2_MODER_MASK_C))
#define SAMP_CAP_MODER_MASK_C_OUT  ((SAMP_CAP_COMP1_MODER_MASK_C_OUT)|(SAMP_CAP_COMP2_MODER_MASK_C_OUT))
#endif
//# PORT_C

#endif
//# (SAMP_CAP_CH == CH2)

#if (SAMP_CAP_CH == CH3)

#ifdef PORT_A
#define SAMP_CAP_PORT_A     (GPIOA)
#define SAMP_CAP_IO_MASK_1  ((uint32_t)0x00000004)   /* PA2 */
#define SAMP_CAP_IO_MASK_4  ((uint32_t)0x00000400)   /* PA10 */
#define SAMP_CAP_IO_MASK_5  ((uint32_t)0x00008000)   /* PA15 */
#ifdef COMP1
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000004)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000004)  /* PA2 */
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000030)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000010)
#else
#define SAMP_CAP_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP1_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#ifdef COMP2
#if (SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000900)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00008400) /* PA10 and PA15 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0xC0300000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x40100000)
#elif (SCKEY_MASK_G4==GROUP4)&&(SCKEY_MASK_G5==0)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000800)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000400) /* PA10 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00300000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00100000)
#elif (SCKEY_MASK_G4==0)&&(SCKEY_MASK_G5==GROUP5)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000100)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00008000) /* PA15 */
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0xC0000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x40000000)
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#else
#define SAMP_CAP_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_COMP2_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#endif
#define SAMP_CAP_IO_MASK_A         ((SAMP_CAP_COMP1_IO_MASK_A)|(SAMP_CAP_COMP2_IO_MASK_A))
#define SAMP_CAP_MODER_MASK_A      ((SAMP_CAP_COMP1_MODER_MASK_A)|(SAMP_CAP_COMP2_MODER_MASK_A))
#define SAMP_CAP_MODER_MASK_A_OUT  ((SAMP_CAP_COMP1_MODER_MASK_A_OUT)|(SAMP_CAP_COMP2_MODER_MASK_A_OUT))
#endif
//# PORT_A

#ifdef PORT_B
#define SAMP_CAP_PORT_B     (GPIOB)
#define SAMP_CAP_IO_MASK_7  ((uint32_t)0x00004000)   /* PB14 */
/* Both comparators are used */
#if defined (COMP1) && defined (COMP2)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00100000)
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00004000)  /* PB14 */
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0x30000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x10000000)
/* Only the comparator 1 is used */
#elif defined (COMP1)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00100000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00004000)  /* PB14 */
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0x30000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x10000000)
/* Only the comparator 2 is used */
#elif defined (COMP2)
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#else
#error "COMPARATOR definition wrong"
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define SAMP_CAP_PORT_C     (GPIOC)
#define SAMP_CAP_IO_MASK_8  ((uint32_t)0x00000004)   /* PC2 */
#define SAMP_CAP_IO_MASK_10 ((uint32_t)0x00000100) /* PC8 */
/* Both comparators are used */
#if defined (COMP1) && defined (COMP2)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00001000)
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000004)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000104)  /* PC2, PC8 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x00030030)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00010010)
/* Only the comparator 1 is used */
#elif defined (COMP1)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00001000)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000004)  /* PC2 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x00000030)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00000010)
/* Only the comparator 2 is used */
#elif defined (COMP2)
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000004)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000100) /* PC8 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x00030000)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00010000)
#else
#error "COMPARATOR definition wrong"
#endif
#endif
//# PORT_C

#endif
//# (SAMP_CAP_CH == CH3)

#if (SAMP_CAP_CH == CH4)

#ifdef PORT_A
#define SAMP_CAP_PORT_A     (GPIOA)
#define SAMP_CAP_IO_MASK_1  ((uint32_t)0x00000008)   /* PA3 */
/* Both comparators are used */
#if defined (COMP1) && defined (COMP2)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000008)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_A        ((uint32_t)0x00000008)  /* PA3*/
#define SAMP_CAP_MODER_MASK_A     ((uint32_t)0x000000C0)
#define SAMP_CAP_MODER_MASK_A_OUT ((uint32_t)0x00000040)
/* Only the comparator 1 is used */
#elif defined (COMP1)
#define SAMP_CAP_COMP1_MASK_A     ((uint32_t)0x00000008)
#define SAMP_CAP_IO_MASK_A        ((uint32_t)0x00000008)  /* PA3 */
#define SAMP_CAP_MODER_MASK_A     ((uint32_t)0x000000C0)
#define SAMP_CAP_MODER_MASK_A_OUT ((uint32_t)0x00000040)
/* Only the comparator 2 is used*/
#elif defined (COMP2)
#define SAMP_CAP_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_A        ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_A     ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_A_OUT ((uint32_t)0x00000000)
#else
#error "COMPARATOR definition wrong"
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define SAMP_CAP_PORT_B     (GPIOB)
#define SAMP_CAP_IO_MASK_7  ((uint32_t)0x00200000)   /* PB15 */
/* Both comparators are used */
#if defined (COMP1) && defined (COMP2)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00200000)
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00008000)  /* PB15 */
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0xC0000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x40000000)
/* Only the comparator 1 is used */
#elif defined (COMP1)
#define SAMP_CAP_COMP1_MASK_B     ((uint32_t)0x00200000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00008000)  /* PB15 */
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0xC0000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x40000000)
/* Only the comparator 2 is used */
#elif defined (COMP2)
#define SAMP_CAP_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_IO_MASK_B        ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_B     ((uint32_t)0x00000000)
#define SAMP_CAP_MODER_MASK_B_OUT ((uint32_t)0x00000000)
#else
#error "COMPARATOR definition wrong"
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define SAMP_CAP_PORT_C     (GPIOC)
#define SAMP_CAP_IO_MASK_8  ((uint32_t)0x00000008)   /* PC3 */
#define SAMP_CAP_IO_MASK_10 ((uint32_t)0x00000200) /* PC9 */
/* Both comparators are used */
#if defined (COMP1) && defined (COMP2)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00002000)
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000008)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000208)  /* PC3, PC9 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x000C00C0)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00040040)
/* Only the comparator 1 is used */
#elif defined (COMP1)
#define SAMP_CAP_COMP1_MASK_C     ((uint32_t)0x00002000)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000008)  /* PC3 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x000000C0)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00000040)
/* Only the comparator 2 is used */
#elif defined (COMP2)
#define SAMP_CAP_COMP2_MASK_C     ((uint32_t)0x00000008)
#define SAMP_CAP_IO_MASK_C        ((uint32_t)0x00000200) /* PC9 */
#define SAMP_CAP_MODER_MASK_C     ((uint32_t)0x000C0000)
#define SAMP_CAP_MODER_MASK_C_OUT ((uint32_t)0x00040000)
#else
#error "COMPARATOR definition wrong"
#endif
#endif
//# PORT_C

#endif
//# (SAMP_CAP_CH == CH4)

#define SAMP_CAP_COMP1_MASK (uint32_t)(SAMP_CAP_COMP1_MASK_A | SAMP_CAP_COMP1_MASK_B | SAMP_CAP_COMP1_MASK_C)
#define SAMP_CAP_COMP2_MASK (uint32_t)(SAMP_CAP_COMP2_MASK_A | SAMP_CAP_COMP2_MASK_B | SAMP_CAP_COMP2_MASK_C)

#if (ACTIVE_SHIELD_CHANNEL > 0)

/* RE-Define the 1st Channel mask without ACTIVE_SHIELD_GROUP */

#if (SCKEY_P1_CH == CH1)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH1_MASK)
#endif
//# (SCKEY_P1_CH == CH1)

#if (SCKEY_P1_CH == CH2)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH2_MASK)
#endif
//# (SCKEY_P1_CH == CH2)

#if (SCKEY_P1_CH == CH3)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH3_MASK)
#endif
//# (SCKEY_P1_CH == CH3)

#if (SCKEY_P1_CH == CH4)
#undef SCKEY_P1_MASK
#define SCKEY_P1_MASK (SCKEY_P1_A | SCKEY_P1_B | SCKEY_P1_C | SCKEY_P1_D |\
                       SCKEY_P1_E | SCKEY_P1_F | SCKEY_P1_G | SCKEY_P1_H |\
                       SCKEY_P1_I | SCKEY_P1_J | MKEY_CH4_MASK)
#endif
//# (SCKEY_P1_CH == CH4)

/* RE-Define the 2nd Channel mask without ACTIVE_SHIELD_GROUP */
#if (NUMBER_OF_ACQUISITION_PORTS > 1)


#if (SCKEY_P2_CH == CH1)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH1_MASK)
#endif
//# (SCKEY_P2_CH == CH1)

#if (SCKEY_P2_CH == CH2)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH2_MASK)
#endif
//# (SCKEY_P2_CH == CH2)

#if (SCKEY_P2_CH == CH3)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH3_MASK)
#endif
//# (SCKEY_P2_CH == CH3)

#if (SCKEY_P2_CH == CH4)
#undef SCKEY_P2_MASK
#define SCKEY_P2_MASK (SCKEY_P2_A | SCKEY_P2_B | SCKEY_P2_C | SCKEY_P2_D |\
                       SCKEY_P2_E | SCKEY_P2_F | SCKEY_P2_G | SCKEY_P2_H |\
                       SCKEY_P2_I | SCKEY_P2_J | MKEY_CH4_MASK)
#endif
//# (SCKEY_P2_CH == CH4)

#endif
//# (NUMBER_OF_ACQUISITION_PORTS > 1)

/* RE-Define the 3rd Channel mask without ACTIVE_SHIELD_GROUP */
#if (NUMBER_OF_ACQUISITION_PORTS > 2)


#if (SCKEY_P3_CH == CH1)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH1_MASK)
#endif
//# (SCKEY_P2_CH == CH1)

#if (SCKEY_P3_CH == CH2)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH2_MASK)
#endif
//# (SCKEY_P2_CH == CH2)

#if (SCKEY_P3_CH == CH3)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH3_MASK)
#endif
//# (SCKEY_P2_CH == CH3)

#if (SCKEY_P3_CH == CH4)
#undef SCKEY_P3_MASK
#define SCKEY_P3_MASK (SCKEY_P3_A | SCKEY_P3_B | SCKEY_P3_C | SCKEY_P3_D |\
                       SCKEY_P3_E | SCKEY_P3_F | SCKEY_P3_G | SCKEY_P3_H |\
                       SCKEY_P3_I | SCKEY_P3_J | MKEY_CH4_MASK)
#endif
//# (SCKEY_P2_CH == CH4)

#endif
//# (NUMBER_OF_ACQUISITION_PORTS > 2)

#endif
//# (ACTIVE_SHIELD_CHANNEL > 0)

/* Initialize the SCKEY COMP1 masks */
#define SCKEY_P1_COMP1_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1 masks */
#define SCKEY_P1_MODER_COMP1_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1_OUT masks */
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP1 masks */
#define SCKEY_P1_IO_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY COMP2 masks */
#define SCKEY_P1_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  STATE COMP2 masks */
#define SCKEY_P1_STATE_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2 masks */
#define SCKEY_P1_MODER_COMP2_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2_OUT masks */
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP2 masks */
#define SCKEY_P1_IO_COMP2_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P1_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P1_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P1_COMP2_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C ((uint16_t)0x0000)
#define SCKEY_P1_MODER_COMP2_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C ((uint32_t)0x00000000)

/* Define the PORT1 comparator channel mask */
#if (SCKEY_P1_CH == CH1)

#ifdef PORT_A
#define P1_PORT_A              (GPIOA)
#define P1_IO_MASK_1           ((uint32_t)0x00000001)   /* PA0 */
#define P1_IO_MASK_2           ((uint32_t)0x00000040)   /* PA6 */
#define P1_IO_MASK_4           ((uint32_t)0x00000100)   /* PA8 */
#define P1_IO_MASK_5           ((uint32_t)0x00002000)   /* PA13 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_A
#undef SCKEY_P1_IO_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_A
#if (((SCKEY_P1_MASK& GROUP1)== GROUP1) && ((SCKEY_P1_MASK&GROUP2)== GROUP2))
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000041)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000041) /* PA0 and PA6 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x00003003)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001001)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0003)
#elif ((SCKEY_P1_MASK& GROUP1)== GROUP1)
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000001)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000001) /* PA0 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x00000003)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000001)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#elif ((SCKEY_P1_MASK&GROUP2)== GROUP2)
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P1_IO_COMP1_MASK_A       ((uint32_t)0x00000040) /* PA6 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x00003000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001000)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P1_COMP1_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A     (0x0000)
#endif
#endif

/* Only the comparator 2 is used */
#ifdef  COMP2
#undef SCKEY_P1_COMP2_MASK_A
#undef SCKEY_P1_IO_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_A
#if (((SCKEY_P1_MASK & GROUP4) == GROUP4) && ((SCKEY_P1_MASK & GROUP5)== GROUP5))
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000240)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00002100) /* PA8 and PA13 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x0C030000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04010000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P1_MASK & GROUP4) == GROUP4)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000200)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00000100) /* PA8 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x00030000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00010000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P1_MASK & GROUP5)== GROUP5)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00002000) /* PA13 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x0C000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P1_COMP2_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     (0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P1_PORT_B              (GPIOB)
#define P1_IO_MASK_3           ((uint32_t)0x00000001)   /* PB0 */
#define P1_IO_MASK_7           ((uint32_t)0x00001000)   /* PB12 */
#define P1_IO_MASK_6           ((uint32_t)0x00000010)   /* PB4 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_B
#undef SCKEY_P1_IO_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_B
#if (((SCKEY_P1_MASK & GROUP3) == GROUP3) && ((SCKEY_P1_MASK & GROUP7)== GROUP7))
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00040100)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00001001) /* PB0 and PB12 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x03000003)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000001)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0044)
#elif ((SCKEY_P1_MASK & GROUP3) == GROUP3)
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00000100)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00000001) /* PB0 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x00000003)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000001)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif ((SCKEY_P1_MASK & GROUP7)== GROUP7)
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00040000)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00001000) /* PB12 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x03000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P1_COMP1_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     (0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_B
#undef SCKEY_P1_IO_COMP2_MASK_B
#undef SCKEY_P1_MODER_COMP2_MASK_B
#undef SCKEY_P1_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_B
#if ((SCKEY_P1_MASK & GROUP6)== GROUP6)
#define SCKEY_P1_COMP2_MASK_B           ((uint32_t)0x00000010)
#define SCKEY_P1_IO_COMP2_MASK_B        ((uint32_t)0x00000010) /* PB4 */
#define SCKEY_P1_MODER_COMP2_MASK_B     ((uint32_t)0x00000300)
#define SCKEY_P1_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000100)
#define SCKEY_P1_STATE_COMP2_MASK_B     ((uint16_t)0x0020)
#else
#define SCKEY_P1_COMP2_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_B     (0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P1_PORT_C              (GPIOC)
#define P1_IO_MASK_8           ((uint32_t)0x00000001)   /* PC0 */
#define P1_IO_MASK_9           ((uint32_t)0x00000010)  /* PC4 */
#define P1_IO_MASK_10          ((uint32_t)0x00000040) /* PC6 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_C
#undef SCKEY_P1_IO_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_C
#if (((SCKEY_P1_MASK&GROUP8)== GROUP8) && ((SCKEY_P1_MASK&GROUP9)== GROUP9))
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00004400)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000011)  /* PC0 and PC4 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000303)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000101)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P1_MASK&GROUP8)== GROUP8)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00000400)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000001) /* PC0 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000003)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000001)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P1_MASK&GROUP9)== GROUP9)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00004000)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000010) /* PC4 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000300)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000100)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P1_COMP1_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C     (0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_C
#undef SCKEY_P1_IO_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_C
#if ((SCKEY_P1_MASK & GROUP10) == GROUP10)
#define SCKEY_P1_COMP2_MASK_C           ((uint32_t)0x00000001)
#define SCKEY_P1_IO_COMP2_MASK_C        ((uint32_t)0x00000040) /* PC6 */
#define SCKEY_P1_MODER_COMP2_MASK_C     ((uint32_t)0x00003000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00001000)
#define SCKEY_P1_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P1_COMP2_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C     (0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P1_CH == CH1)

#if (SCKEY_P1_CH == CH2)

#ifdef PORT_A
#define P1_IO_PORT_A     (GPIOA)
#define P1_IO_MASK_1     ((uint32_t)0x00000002)   /* PA1 */
#define P1_IO_MASK_2     ((uint32_t)0x00000080)   /* PA7 */
#define P1_IO_MASK_4     ((uint32_t)0x00000200)   /* PA9 */
#define P1_IO_MASK_5     ((uint32_t)0x00004000)   /* PA14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_A
#undef SCKEY_P1_IO_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_A
#if (((SCKEY_P1_MASK & GROUP1) == GROUP1) && ((SCKEY_P1_MASK & GROUP2)== GROUP2))
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000082)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000082)  /*PA1 and PA7*/
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x0000C00C)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004004)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0003)
#elif (((SCKEY_P1_MASK & GROUP1)== GROUP1) && ((SCKEY_P1_MASK & GROUP2)== 0))
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000002)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000002) /* PA1 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x0000000C)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000004)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#elif ((SCKEY_P1_MASK & GROUP1)== 0) && ((SCKEY_P1_MASK & GROUP2)== GROUP2)
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000080) /* PA7 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x0000C000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004000)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P1_COMP1_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A     (0x0000)
#endif
#else
#define SCKEY_P1_COMP1_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A     (0x0000)
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_A
#undef SCKEY_P1_IO_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_A
#if (((SCKEY_P1_MASK & GROUP4) == GROUP4) && ((SCKEY_P1_MASK & GROUP5)== GROUP5))
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000480)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00004200) /* PA9 and PA14 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x300C0000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10040000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif (((SCKEY_P1_MASK & GROUP4)== GROUP4) && ((SCKEY_P1_MASK & GROUP5)== 0))
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000400)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00000200) /* PA9 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x000C0000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00040000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P1_MASK & GROUP4)== 0) && ((SCKEY_P1_MASK & GROUP5)== GROUP5)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P1_IO_COMP2_MASK _A       ((uint32_t)0x00004000) /* PA14 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x30000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P1_COMP2_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     (0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P1_PORT_B              (GPIOB)
#define P1_IO_MASK_3           ((uint32_t)0x00000002)   /* PB1 */
#define P1_IO_MASK_7           ((uint32_t)0x00002000)   /* PB13 */
#define P1_IO_MASK_6           ((uint32_t)0x00000020)   /* PB5 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_B
#undef SCKEY_P1_IO_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_B
#if ((((SCKEY_P1_MASK & GROUP3)== GROUP3) && ((SCKEY_P1_MASK & GROUP7)== GROUP7)))
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00080200)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00002002)  /* PB1 and PB13 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x0C00000C)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000004)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0044)
#elif ((SCKEY_P1_MASK & GROUP3)== GROUP3)
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00000200)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00000002) /* PB1 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x0000000C)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000004)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif (SCKEY_P1_MASK & GROUP7)== GROUP7
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00080000)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00002000) /* PB13 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x0C000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P1_COMP1_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     (0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_B
#undef SCKEY_P1_IO_COMP2_MASK_B
#undef SCKEY_P1_MODER_COMP2_MASK_B
#undef SCKEY_P1_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_B
#if ((SCKEY_P1_MASK & GROUP6)== GROUP6)
#define SCKEY_P1_COMP2_MASK_B           ((uint32_t)0x00000020)
#define SCKEY_P1_IO_COMP2_MASK_B        ((uint32_t)0x00000020) /* PB5 */
#define SCKEY_P1_MODER_COMP2_MASK_B     ((uint32_t)0x00000C00)
#define SCKEY_P1_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000400)
#define SCKEY_P1_STATE_COMP2_MASK_B     ((uint16_t)0x0020)
#else
#define SCKEY_P1_COMP2_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_B     (0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P1_PORT_C              (GPIOC)
#define P1_IO_MASK_8           ((uint32_t)0x00000002)   /* PC1 */
#define P1_IO_MASK_9           ((uint32_t)0x00000020)   /* PC5 */
#define P1_IO_MASK_10          ((uint32_t)0x00000080)   /* PC7 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_C
#undef SCKEY_P1_IO_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_C
#if (((SCKEY_P1_MASK & GROUP8)== GROUP8) && ((SCKEY_P1_MASK & GROUP9)== GROUP9))
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00008800)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000022)  /* PC1 and PC5 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000C0C)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000404)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P1_MASK & GROUP8)== GROUP8)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00000800)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000002) /* PC1 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x0000000C)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000004)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P1_MASK & GROUP9)== GROUP9)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00008000)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000020) /* PC5 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000C00)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000400)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P1_COMP1_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C     (0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_C
#undef SCKEY_P1_IO_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_C
#if ((SCKEY_P1_MASK & GROUP10)== GROUP10)
#define SCKEY_P1_COMP2_MASK_C           ((uint32_t)0x00000002)
#define SCKEY_P1_IO_COMP2_MASK_C        ((uint32_t)0x00000080) /* PC7 */
#define SCKEY_P1_MODER_COMP2_MASK_C     ((uint32_t)0x0000C000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00004000)
#define SCKEY_P1_STATE_COMP2_MASK_C     ((uint32_t)0x0200)
#else
#define SCKEY_P1_COMP2_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C     (0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P1_CH == CH2)

#if (SCKEY_P1_CH == CH3)

#ifdef PORT_A
#define P1_IO_PORT_A  (GPIOA)
#define P1_IO_MASK_1  ((uint32_t)0x00000004)   /* PA2 */
#define P1_IO_MASK_4  ((uint32_t)0x00000400)   /* PA10 */
#define P1_IO_MASK_5  ((uint32_t)0x00008000)   /* PA15 */
#undef SCKEY_P1_COMP1_MASK_A
#undef SCKEY_P1_IO_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_A
#undef SCKEY_P1_COMP2_MASK_A
#undef SCKEY_P1_IO_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A
#undef SCKEY_P1_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_A
#ifdef COMP1
#if ((SCKEY_P1_MASK & GROUP1)== GROUP1)
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000004)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000004)  /* PA2 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x00000030)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000010)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
#ifdef COMP2
#if  ((SCKEY_P1_MASK & GROUP4) == GROUP4) &&  ((SCKEY_P1_MASK & GROUP5) == GROUP5)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000900)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00008400) /* PA10 and PA15 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0xC0300000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40100000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P1_MASK & GROUP4) == GROUP4) &&  ((SCKEY_P1_MASK & GROUP5) == 0)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000800)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00000400) /* PA10 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x00300000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00100000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P1_MASK & GROUP4) == 0) &&  ((SCKEY_P1_MASK & GROUP5) == GROUP5)
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000100)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00008000) /* PA15 */
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0xC0000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#else
#define SCKEY_P1_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P1_PORT_B     (GPIOB)
#define P1_IO_MASK_7  ((uint32_t)0x00004000)   /* PB14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_B
#undef SCKEY_P1_IO_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_B
#if ((SCKEY_P1_MASK & GROUP7)== GROUP7)
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00100000)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00004000)  /* PB14 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0x30000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x10000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P1_COMP1_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     (0x0000)
#endif
#else
#define SCKEY_P1_COMP1_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     (0x0000)
#define SCKEY_P2_COMP1_MASK_B           (0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     (0x0000)
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P1_PORT_C     (GPIOC)
#define P1_IO_MASK_8  (0x00000004)   /* PC2 */
#define P1_IO_MASK_10 (0x00000100)  /* PC8 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_C
#undef SCKEY_P1_IO_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_C
#if ((SCKEY_P1_MASK & GROUP8) == GROUP8)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00001000)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000004)  /* PC2 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x00000030)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000010)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P1_COMP1_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C     (0x0000)
#endif
#else
#define SCKEY_P1_COMP1_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C     (0x0000)
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_C
#undef SCKEY_P1_IO_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_C
#if ((SCKEY_P1_MASK & GROUP10) == GROUP10)
#define SCKEY_P1_COMP2_MASK_C           ((uint32_t)0x00000004)
#define SCKEY_P1_IO_COMP2_MASK_C        ((uint32_t)0x00000100) /* PC8 */
#define SCKEY_P1_MODER_COMP2_MASK_C     ((uint32_t)0x00030000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00010000)
#define SCKEY_P1_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P1_COMP2_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C     (0x0000)
#endif
#else
#define SCKEY_P1_COMP2_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C     (0x0000)
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P1_CH == CH3)

#if (SCKEY_P1_CH == CH4)

#ifdef PORT_A
#define P1_PORT_A     (GPIOA)
#define P1_IO_MASK_1  (0x00000008)   /* PA3 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_A
#undef SCKEY_P1_IO_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A
#undef SCKEY_P1_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_A
#if ((SCKEY_P1_MASK & GROUP1) == GROUP1)
#define SCKEY_P1_COMP1_MASK_A           ((uint32_t)0x00000008)
#define SCKEY_P1_IO_COMP1_MASK_A        ((uint32_t)0x00000008)  /* PA3 */
#define SCKEY_P1_MODER_COMP1_MASK_A     ((uint32_t)0x000000C0)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000040)
#define SCKEY_P1_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P1_COMP1_MASK_A           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_A        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_A_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_A     (0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P1_PORT_B     (GPIOB)
#define P1_IO_MASK_7  ((uint32_t)0x00008000)   /* PB15 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_B
#undef SCKEY_P1_IO_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B
#undef SCKEY_P1_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_B
#if ((SCKEY_P1_MASK & GROUP7) == GROUP7)
#define SCKEY_P1_COMP1_MASK_B           ((uint32_t)0x00200000)
#define SCKEY_P1_IO_COMP1_MASK_B        ((uint32_t)0x00008000)  /* PB15 */
#define SCKEY_P1_MODER_COMP1_MASK_B     ((uint32_t)0xC0000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT ((uint32_t)0x40000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P1_COMP1_MASK_B           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_B        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_B_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_B     (0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P1_PORT_C           (GPIOC)
#define P1_IO_MASK_8        ((uint32_t)0x00000008)   /* PC3 */
#define P1_IO_MASK_10       ((uint32_t)0x00000200) /* PC9 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P1_COMP1_MASK_C
#undef SCKEY_P1_IO_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C
#undef SCKEY_P1_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP1_MASK_C
#if ((SCKEY_P1_MASK & GROUP8)== GROUP8)
#define SCKEY_P1_COMP1_MASK_C           ((uint32_t)0x00002000)
#define SCKEY_P1_IO_COMP1_MASK_C        ((uint32_t)0x00000008)  /* PC3 */
#define SCKEY_P1_MODER_COMP1_MASK_C     ((uint32_t)0x000000C0)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000040)
#define SCKEY_P1_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P1_COMP1_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP1_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP1_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP1_MASK_C     (0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P1_COMP2_MASK_C
#undef SCKEY_P1_IO_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C
#undef SCKEY_P1_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P1_STATE_COMP2_MASK_C
#if ((SCKEY_P1_MASK & GROUP10)== GROUP10)
#define SCKEY_P1_COMP2_MASK_C           ((uint32_t)0x00000008)
#define SCKEY_P1_IO_COMP2_MASK_C        ((uint32_t)0x00000200) /* PC9 */
#define SCKEY_P1_MODER_COMP2_MASK_C     ((uint32_t)0x000C0000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00040000)
#define SCKEY_P1_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P1_COMP2_MASK_C           (0x00000000)
#define SCKEY_P1_IO_COMP2_MASK_C        (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C     (0x00000000)
#define SCKEY_P1_MODER_COMP2_MASK_C_OUT (0x00000000)
#define SCKEY_P1_STATE_COMP2_MASK_C     (0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P1_CH == CH4)

#define SCKEY_P1_STATE_COMP1_MASK (SCKEY_P1_STATE_COMP1_MASK_A\
                                   |SCKEY_P1_STATE_COMP1_MASK_B\
                                   |SCKEY_P1_STATE_COMP1_MASK_C)

#define SCKEY_P1_STATE_COMP2_MASK (SCKEY_P1_STATE_COMP2_MASK_A\
                                   |SCKEY_P1_STATE_COMP2_MASK_B\
                                   |SCKEY_P1_STATE_COMP2_MASK_C)

#define SCKEY_P1_STATE_MASK (SCKEY_P1_STATE_COMP1_MASK | SCKEY_P1_STATE_COMP2_MASK)

#define P1_MODER_MASK_A_OUT (SCKEY_P1_MODER_COMP1_MASK_A_OUT | SCKEY_P1_MODER_COMP2_MASK_A_OUT | SHIELD_MODER_MASK_A_OUT)
#define P1_MODER_MASK_B_OUT (SCKEY_P1_MODER_COMP1_MASK_B_OUT | SCKEY_P1_MODER_COMP2_MASK_B_OUT | SHIELD_MODER_MASK_B_OUT)
#define P1_MODER_MASK_C_OUT (SCKEY_P1_MODER_COMP1_MASK_C_OUT | SCKEY_P1_MODER_COMP2_MASK_C_OUT | SHIELD_MODER_MASK_C_OUT)

#define P1_MODER_MASK_A (SCKEY_P1_MODER_COMP1_MASK_A | SCKEY_P1_MODER_COMP2_MASK_A | SHIELD_MODER_MASK_A)
#define P1_MODER_MASK_B (SCKEY_P1_MODER_COMP1_MASK_B | SCKEY_P1_MODER_COMP2_MASK_B | SHIELD_MODER_MASK_B)
#define P1_MODER_MASK_C (SCKEY_P1_MODER_COMP1_MASK_C | SCKEY_P1_MODER_COMP2_MASK_C | SHIELD_MODER_MASK_C)

#define P1_COMP1_MASK (SCKEY_P1_COMP1_MASK_A | SCKEY_P1_COMP1_MASK_B | SCKEY_P1_COMP1_MASK_C | SHIELD_COMP1_MASK)
#define P1_COMP2_MASK (SCKEY_P1_COMP2_MASK_A | SCKEY_P1_COMP2_MASK_B | SCKEY_P1_COMP2_MASK_C | SHIELD_COMP2_MASK)

#define P1_IO_MASK_A (SCKEY_P1_IO_COMP1_MASK_A | SCKEY_P1_IO_COMP2_MASK_A | SHIELD_IO_MASK_A)
#define P1_IO_MASK_B (SCKEY_P1_IO_COMP1_MASK_B | SCKEY_P1_IO_COMP2_MASK_B | SHIELD_IO_MASK_B)
#define P1_IO_MASK_C (SCKEY_P1_IO_COMP1_MASK_C | SCKEY_P1_IO_COMP2_MASK_C | SHIELD_IO_MASK_C)

/* Initialize the SCKEY COMP1 masks */
#define SCKEY_P2_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  STATE COMP1 masks */
#define SCKEY_P2_STATE_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1 masks */
#define SCKEY_P2_MODER_COMP1_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1_OUT masks */
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP1 masks */
#define SCKEY_P2_IO_COMP1_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P2_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_C ((uint32_t)0x00000000)

/* Initialize the SCKEY COMP2 masks */
#define SCKEY_P2_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  STATE COMP2 masks */
#define SCKEY_P2_STATE_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2 masks */
#define SCKEY_P2_MODER_COMP2_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2_OUT masks */
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP2 masks */
#define SCKEY_P2_IO_COMP2_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P2_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P2_COMP2_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_C ((uint16_t)0x0000)
#define SCKEY_P2_MODER_COMP2_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_C ((uint32_t)0x00000000)

#if NUMBER_OF_ACQUISITION_PORTS > 1

#if (SCKEY_P2_CH == CH1)

#ifdef PORT_A
#define P2_PORT_A              (GPIOA)
#define P2_IO_MASK_1           ((uint32_t)0x00000001)   /* PA0 */
#define P2_IO_MASK_2           ((uint32_t)0x00000040)   /* PA6 */
#define P2_IO_MASK_4           ((uint32_t)0x00000100)   /* PA8 */
#define P2_IO_MASK_5           ((uint32_t)0x00002000)   /* PA13 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_A
#undef SCKEY_P2_IO_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_A
#if (((SCKEY_P2_MASK& GROUP1)== GROUP1) && ((SCKEY_P2_MASK&GROUP2)== GROUP2))
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000041)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000041) /* PA0 and PA6 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00003003)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001001) /* */
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint32_t)0x0003)
#elif ((SCKEY_P2_MASK& GROUP1)== GROUP1)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000001)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000001) /* PA0 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000003)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000001)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#elif ((SCKEY_P2_MASK&GROUP2)== GROUP2)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P2_IO_COMP1_MASK _A       ((uint32_t)0x00000040) /* PA6 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00003000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef  COMP2
#undef SCKEY_P2_COMP2_MASK_A
#undef SCKEY_P2_IO_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_A
#if (((SCKEY_P2_MASK & GROUP4) == GROUP4) && ((SCKEY_P2_MASK & GROUP5)== GROUP5))
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000240)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00002100) /* PA8 and PA13 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x0C030000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04010000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint32_t)0x0018)
#elif ((SCKEY_P2_MASK & GROUP4) == GROUP4)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000200)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000100) /* PA8 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x00030000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00010000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint32_t)0x0008)
#elif ((SCKEY_P2_MASK & GROUP5)== GROUP5)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00002000) /* PA13 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x0C000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_A

#ifdef PORT_B
#define P2_PORT_B              (GPIOB)
#define P2_IO_MASK_3           ((uint32_t)0x00000001)   /* PB0 */
#define P2_IO_MASK_7           ((uint32_t)0x00001000)   /* PB12 */
#define P2_IO_MASK_6           ((uint32_t)0x00000010)   /* PB4 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_B
#undef SCKEY_P2_IO_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_B
#if (((SCKEY_P2_MASK & GROUP3) == GROUP3) && ((SCKEY_P2_MASK & GROUP7)== GROUP7))
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00040100)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00001001) /* PB0 and PB12 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x03000003)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000001)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0044)
#elif ((SCKEY_P2_MASK & GROUP3) == GROUP3)
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000100)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000001) /* PB0 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x00000003)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000001)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif ((SCKEY_P2_MASK & GROUP7)== GROUP7)
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00040000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00001000) /* PB12 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x03000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_B
#undef SCKEY_P2_IO_COMP2_MASK_B
#undef SCKEY_P2_MODER_COMP2_MASK_B
#undef SCKEY_P2_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_B
#if ((SCKEY_P2_MASK & GROUP6)== GROUP6)
#define SCKEY_P2_COMP2_MASK_B           ((uint32_t)0x00000010)
#define SCKEY_P2_IO_COMP2_MASK_B        ((uint32_t)0x00000010) /* PB4 */
#define SCKEY_P2_MODER_COMP2_MASK_B     ((uint32_t)0x00000300)
#define SCKEY_P2_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000100)
#define SCKEY_P2_STATE_COMP2_MASK_B     ((uint16_t)0x0020)
#else
#define SCKEY_P2_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_B     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#undef SCKEY_P2_COMP1_MASK_C
#undef SCKEY_P2_IO_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_C
#define P2_PORT_C              (GPIOC)
#define P2_IO_MASK_8           ((uint32_t)0x00000001)   /* PC0 */
#define P2_IO_MASK_9           ((uint32_t)0x00000010)  /* PC4 */
#define P2_IO_MASK_10          ((uint32_t)0x00000040) /* PC6 */
/* Only the comparator 1 is used */
#ifdef COMP1
#if (((SCKEY_P2_MASK&GROUP8)== GROUP8) && ((SCKEY_P2_MASK&GROUP9)== GROUP9))
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00004400)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000011)  /* PC0 and PC4 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000303)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000101)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P2_MASK&GROUP8)== GROUP8)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000400)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000001) /* PC0 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000003)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000001)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P2_MASK&GROUP9)== GROUP9)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00004000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000010) /* PC4 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000300)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000100)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_C
#undef SCKEY_P2_IO_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_C
#if ((SCKEY_P2_MASK & GROUP10) == GROUP10)
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000001)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000040) /* PC6 */
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00003000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00001000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P2_CH == CH1)

#if (SCKEY_P2_CH == CH2)

#ifdef PORT_A
#define P2_IO_PORT_A     (GPIOA)
#define P2_IO_MASK_1     ((uint32_t)0x00000002)   /* PA1 */
#define P2_IO_MASK_2     ((uint32_t)0x00000080)   /* PA7 */
#define P2_IO_MASK_4     ((uint32_t)0x00000200)   /* PA9 */
#define P2_IO_MASK_5     ((uint32_t)0x00004000)   /* PA14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_A
#undef SCKEY_P2_IO_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_A
#if (((SCKEY_P2_MASK & GROUP1) == GROUP1) && ((SCKEY_P2_MASK & GROUP2)== GROUP2))
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000082)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000082)  /*PA1 and PA7*/
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x0000C00C)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004004)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0003)
#elif ((SCKEY_P2_MASK & GROUP1)== GROUP1)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000002)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000002) /* PA1 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x0000000C)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000004)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#elif ((SCKEY_P2_MASK & GROUP2)== GROUP2)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000080) /* PA7 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x0000C000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_A
#undef SCKEY_P2_IO_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_A
#if (((SCKEY_P2_MASK & GROUP4) == GROUP4) && ((SCKEY_P2_MASK & GROUP5)== GROUP5))
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000480)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00004200) /* PA9 and PA14 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x300C0000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10040000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P2_MASK & GROUP4)== GROUP4)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000400)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000200) /* PA9 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x000C0000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00040000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P2_MASK & GROUP5)== GROUP5)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P2_IO_COMP2_MASK _A       ((uint32_t)0x00004000) /* PA14 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x30000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_A

#ifdef PORT_B
#define P2_PORT_B              (GPIOB)
#define P2_IO_MASK_3           ((uint32_t)0x00000002)   /* PB1 */
#define P2_IO_MASK_7           ((uint32_t)0x00002000)   /* PB13 */
#define P2_IO_MASK_6           ((uint32_t)0x00000020)   /* PB5 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_B
#undef SCKEY_P2_IO_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_B
#if ((((SCKEY_P2_MASK & GROUP3)== GROUP3) && ((SCKEY_P2_MASK & GROUP7)== GROUP7)))
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00080200)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00002002)  /* PB1 and PB13 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x0C00000C)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000004)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0044)
#elif ((SCKEY_P2_MASK & GROUP3)== GROUP3)
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000200)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000002) /* PB1 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x0000000C)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000004)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif (SCKEY_P2_MASK & GROUP7)== GROUP7
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00080000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00002000) /* PB13 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x0C000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_B
#undef SCKEY_P2_IO_COMP2_MASK_B
#undef SCKEY_P2_MODER_COMP2_MASK_B
#undef SCKEY_P2_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_B
#if ((SCKEY_P2_MASK & GROUP6)== GROUP6)
#define SCKEY_P2_COMP2_MASK_B           ((uint32_t)0x00000020)
#define SCKEY_P2_IO_COMP2_MASK_B        ((uint32_t)0x00000020) /* PB5 */
#define SCKEY_P2_MODER_COMP2_MASK_B     ((uint32_t)0x00000C00)
#define SCKEY_P2_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000400)
#define SCKEY_P2_STATE_COMP2_MASK_B     ((uint16_t)0x0020)
#else
#define SCKEY_P2_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_B     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_B

#ifdef PORT_C
#define P2_PORT_C              (GPIOC)
#define P2_IO_MASK_8           ((uint32_t)0x00000002)   /* PC1 */
#define P2_IO_MASK_9           ((uint32_t)0x00000020)   /* PC5 */
#define P2_IO_MASK_10          ((uint32_t)0x00000080)   /* PC7 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_C
#undef SCKEY_P2_IO_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_C
#if (((SCKEY_P2_MASK & GROUP8)== GROUP8) && ((SCKEY_P2_MASK & GROUP9)== GROUP9))
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00008800)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000022)  /* PC1 and PC5 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000C0C)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000404)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P2_MASK & GROUP8)== GROUP8)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000800)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000002) /* PC1 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x0000000C)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000004)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P2_MASK & GROUP9)== GROUP9)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00008000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000020) /* PC5 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000C00)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000400)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_C
#undef SCKEY_P2_IO_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_C
#if ((SCKEY_P2_MASK & GROUP10)== GROUP10)
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000002)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000080) /* PC7 */
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x0000C000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00004000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P2_CH == CH2)

#if (SCKEY_P2_CH == CH3)

#ifdef PORT_A
#define P2_IO_PORT_A  (GPIOA)
#define P2_IO_MASK_1  ((uint32_t)0x00000004)   /* PA2 */
#define P2_IO_MASK_4  ((uint32_t)0x00000400)   /* PA10 */
#define P2_IO_MASK_5  ((uint32_t)0x00008000)   /* PA15 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_A
#undef SCKEY_P2_IO_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_A
#if ((SCKEY_P2_MASK & GROUP1)== GROUP1)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000004)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000004)  /* PA2 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000030)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000010)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_A
#undef SCKEY_P2_IO_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A
#undef SCKEY_P2_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_A
#if  (((SCKEY_P2_MASK & GROUP4) == GROUP4) &&  ((SCKEY_P2_MASK & GROUP5) == GROUP5))
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000900)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00008400) /* PA10 and PA15 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0xC0300000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40100000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P2_MASK & GROUP4) == GROUP4)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000800)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000400) /* PA10 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x00300000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00100000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P2_MASK & GROUP5) == GROUP5)
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000100)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00008000) /* PA15 */
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0xC0000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P2_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P2_PORT_B     (GPIOB)
#define P2_IO_MASK_7  ((uint32_t)0x00004000)   /* PB14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_B
#undef SCKEY_P2_IO_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_B
#if ((SCKEY_P2_MASK & GROUP7)== GROUP7)
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00100000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00004000)  /* PB14 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x30000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x10000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P2_PORT_C     (GPIOC)
#define P2_IO_MASK_8  ((uint32_t)0x00000004)   /* PC2 */
#define P2_IO_MASK_10 ((uint32_t)0x00000100)  /* PC8 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_C
#undef SCKEY_P2_IO_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_C
#if ((SCKEY_P2_MASK & GROUP8) == GROUP8)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00001000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000004)  /* PC2 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000030)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000010)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_C
#undef SCKEY_P2_IO_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_C
#if ((SCKEY_P2_MASK & GROUP10) == GROUP10)
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000004)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000100) /* PC8 */
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00030000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00010000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P2_CH == CH3)

#if (SCKEY_P2_CH == CH4)

#ifdef PORT_A
#define P2_PORT_A     (GPIOA)
#define P2_IO_MASK_1  ((uint32_t)0x00000008)   /* PA3 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_A
#undef SCKEY_P2_IO_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A
#undef SCKEY_P2_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_A
#if ((SCKEY_P2_MASK & GROUP1) == GROUP1)
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000008)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000008)  /* PA3 */
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x000000C0)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000040)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P2_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P2_PORT_B     (GPIOB)
#define P2_IO_MASK_7  ((uint32_t)0x00008000)   /* PB15 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_B
#undef SCKEY_P2_IO_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B
#undef SCKEY_P2_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_B
#if ((SCKEY_P2_MASK & GROUP7) == GROUP7)
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00200000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00008000)  /* PB15 */
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0xC0000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x40000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P2_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P2_PORT_C           (GPIOC)
#define P2_IO_MASK_8        ((uint32_t)0x00000008)   /* PC3 */
#define P2_IO_MASK_10       ((uint32_t)0x00000200) /* PC9 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P2_COMP1_MASK_C
#undef SCKEY_P2_IO_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C
#undef SCKEY_P2_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP1_MASK_C
#if ((SCKEY_P2_MASK & GROUP8)== GROUP8)
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00002000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000008)  /* PC3 */
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x000000C0)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000040)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P2_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P2_COMP2_MASK_C
#undef SCKEY_P2_IO_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C
#undef SCKEY_P2_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P2_STATE_COMP2_MASK_C
#if ((SCKEY_P2_MASK & GROUP10)== GROUP10)
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000008)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000200) /* PC9 */
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x000C0000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00040000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P2_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P2_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P2_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P2_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P2_CH == CH4)

#endif
//# NUMBER_OF_ACQUISITION_PORTS > 1

#define SCKEY_P2_STATE_COMP1_MASK (SCKEY_P2_STATE_COMP1_MASK_A | SCKEY_P2_STATE_COMP1_MASK_B | SCKEY_P2_STATE_COMP1_MASK_C)
#define SCKEY_P2_STATE_COMP2_MASK (SCKEY_P2_STATE_COMP2_MASK_A | SCKEY_P2_STATE_COMP2_MASK_B | SCKEY_P2_STATE_COMP2_MASK_C)

#define SCKEY_P2_STATE_MASK (SCKEY_P2_STATE_COMP1_MASK | SCKEY_P2_STATE_COMP2_MASK)

#define P2_MODER_MASK_A_OUT (SCKEY_P2_MODER_COMP1_MASK_A_OUT | SCKEY_P2_MODER_COMP2_MASK_A_OUT | SHIELD_MODER_MASK_A_OUT)
#define P2_MODER_MASK_B_OUT (SCKEY_P2_MODER_COMP1_MASK_B_OUT | SCKEY_P2_MODER_COMP2_MASK_B_OUT | SHIELD_MODER_MASK_B_OUT)
#define P2_MODER_MASK_C_OUT (SCKEY_P2_MODER_COMP1_MASK_C_OUT | SCKEY_P2_MODER_COMP2_MASK_C_OUT | SHIELD_MODER_MASK_C_OUT)

#define P2_MODER_MASK_A (SCKEY_P2_MODER_COMP1_MASK_A | SCKEY_P2_MODER_COMP2_MASK_A | SHIELD_MODER_MASK_A)
#define P2_MODER_MASK_B (SCKEY_P2_MODER_COMP1_MASK_B | SCKEY_P2_MODER_COMP2_MASK_B | SHIELD_MODER_MASK_B)
#define P2_MODER_MASK_C (SCKEY_P2_MODER_COMP1_MASK_C | SCKEY_P2_MODER_COMP2_MASK_C | SHIELD_MODER_MASK_C)

#define P2_COMP1_MASK (SCKEY_P2_COMP1_MASK_A | SCKEY_P2_COMP1_MASK_B | SCKEY_P2_COMP1_MASK_C | SHIELD_COMP1_MASK)
#define P2_COMP2_MASK (SCKEY_P2_COMP2_MASK_A | SCKEY_P2_COMP2_MASK_B | SCKEY_P2_COMP2_MASK_C | SHIELD_COMP2_MASK)

#define P2_IO_MASK_A (SCKEY_P2_IO_COMP1_MASK_A | SCKEY_P2_IO_COMP2_MASK_A | SHIELD_IO_MASK_A)
#define P2_IO_MASK_B (SCKEY_P2_IO_COMP1_MASK_B | SCKEY_P2_IO_COMP2_MASK_B | SHIELD_IO_MASK_B)
#define P2_IO_MASK_C (SCKEY_P2_IO_COMP1_MASK_C | SCKEY_P2_IO_COMP2_MASK_C | SHIELD_IO_MASK_C)

/* Initialize the SCKEY COMP1 masks */
#define SCKEY_P3_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  STATE COMP1 masks */
#define SCKEY_P3_STATE_COMP1_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1 masks */
#define SCKEY_P3_MODER_COMP1_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP1_OUT masks */
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP1 masks */
#define SCKEY_P3_IO_COMP1_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P3_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_C ((uint32_t)0x00000000)

/* Initialize the SCKEY COMP2 masks */
#define SCKEY_P3_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  STATE COMP2 masks */
#define SCKEY_P3_STATE_COMP2_MASK_A ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2 masks */
#define SCKEY_P3_MODER_COMP2_MASK_A  ((uint32_t)0x00000000)
/* Initialize the SCKEY  MODER_COMP2_OUT masks */
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
/* Initialize the SCKEY IO_COMP2 masks */
#define SCKEY_P3_IO_COMP2_MASK_A ((uint32_t)0x00000000)
#define SCKEY_P3_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B  ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_B ((uint32_t)0x00000000)
#define SCKEY_P3_COMP2_MASK_C ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_C ((uint16_t)0x0000)
#define SCKEY_P3_MODER_COMP2_MASK_C  ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_C ((uint32_t)0x00000000)

/* Define the PORT3 comparator channel mask */
#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 2) || (NUMBER_OF_MULTI_CHANNEL_KEYS > 0)

#if (SCKEY_P3_CH == CH1)

#ifdef PORT_A
#define P3_PORT_A              (GPIOA)
#define P3_IO_MASK_1           ((uint32_t)0x00000001)   /* PA0 */
#define P3_IO_MASK_2           ((uint32_t)0x00000040)   /* PA6 */
#define P3_IO_MASK_4           ((uint32_t)0x00000100)   /* PA8 */
#define P3_IO_MASK_5           ((uint32_t)0x00002000)   /* PA13 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_A
#undef SCKEY_P3_IO_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_A
#if (((SCKEY_P3_MASK& GROUP1)== GROUP1) && ((SCKEY_P1_MASK&GROUP2)== GROUP2))
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000041)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000041) /* PA0 and PA6 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00003003)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001001)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0003)
#elif ((SCKEY_P3_MASK& GROUP1)== GROUP1)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000001)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000001) /* PA0 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000003)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000001)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint32_t)0x0001)
#elif ((SCKEY_P3_MASK&GROUP2)== GROUP2)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P3_IO_COMP1_MASK _A       ((uint32_t)0x00000040) /* PA6 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00003000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00001000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef  COMP2
#undef SCKEY_P3_COMP2_MASK_A
#undef SCKEY_P3_IO_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_A
#if (((SCKEY_P3_MASK & GROUP4) == GROUP4) && ((SCKEY_P3_MASK & GROUP5)== GROUP5))
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000240)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00002100) /* PA8 and PA13 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x0C030000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04010000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint32_t)0x0018)
#elif ((SCKEY_P3_MASK & GROUP4) == GROUP4)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000200)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000100) /* PA8 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x00030000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00010000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P3_MASK & GROUP5)== GROUP5)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000040)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00002000) /* PA13 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x0C000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x04000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_A

#ifdef PORT_B
#define P3_PORT_B              (GPIOB)
#define P3_IO_MASK_3           ((uint32_t)0x00000001)   /* PB0 */
#define P3_IO_MASK_7           ((uint32_t)0x00001000)   /* PB12 */
#define P3_IO_MASK_6           ((uint32_t)0x00000010)   /* PB4 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_B
#undef SCKEY_P3_IO_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_B
#if (((SCKEY_P3_MASK & GROUP3) == GROUP3) && ((SCKEY_P3_MASK & GROUP7)== GROUP7))
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00040100)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00001001) /* PB0 and PB12 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x03000003)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000001)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint32_t)0x0044)
#elif ((SCKEY_P3_MASK & GROUP3) == GROUP3)
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000100)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000001) /* PB0 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x00000003)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000001)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif ((SCKEY_P3_MASK & GROUP7)== GROUP7)
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00040000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00001000) /* PB12 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x03000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x01000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_B
#undef SCKEY_P3_IO_COMP2_MASK_B
#undef SCKEY_P3_MODER_COMP2_MASK_B
#undef SCKEY_P3_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_B
#if ((SCKEY_P3_MASK & GROUP6)== GROUP6)
#define SCKEY_P3_COMP2_MASK_B           ((uint32_t)0x00000010)
#define SCKEY_P3_IO_COMP2_MASK_B        ((uint32_t)0x00000010) /* PB4 */
#define SCKEY_P3_MODER_COMP2_MASK_B     ((uint32_t)0x00000300)
#define SCKEY_P3_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000100)
#define SCKEY_P3_STATE_COMP2_MASK_B     ((uint32_t)0x0020)
#else
#define SCKEY_P3_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_B     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_B

#ifdef PORT_C
#define P3_PORT_C              (GPIOC)
#define P3_IO_MASK_8           ((uint32_t)0x00000001)   /* PC0 */
#define P3_IO_MASK_9           ((uint32_t)0x00000010)  /* PC4 */
#define P3_IO_MASK_10          ((uint32_t)0x00000040) /* PC6 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_C
#undef SCKEY_P3_IO_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_C
#if (((SCKEY_P3_MASK&GROUP8)== GROUP8) && ((SCKEY_P1_MASK&GROUP9)== GROUP9))
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00004400)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000011)  /* PC0 and PC4 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000303)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000101)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P3_MASK&GROUP8)== GROUP8)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000400)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000001) /* PC0 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000003)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000001)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P3_MASK&GROUP9)== GROUP9)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00004000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000010) /* PC4 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000300)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000100)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_C
#undef SCKEY_P3_IO_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_C
#if ((SCKEY_P3_MASK & GROUP10) == GROUP10)
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000001)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000040) /* PC6 */
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00003000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00001000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_C

#endif
//# (SCKEY_P3_CH == CH1)

#if (SCKEY_P3_CH == CH2)

#ifdef PORT_A
#define P3_IO_PORT_A     (GPIOA)
#define P3_IO_MASK_1     ((uint32_t)0x00000002)   /* PA1 */
#define P3_IO_MASK_2     ((uint32_t)0x00000080)   /* PA7 */
#define P3_IO_MASK_4     ((uint32_t)0x00000200)   /* PA9 */
#define P3_IO_MASK_5     ((uint32_t)0x00004000)   /* PA14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_A
#undef SCKEY_P3_IO_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_A
#if (((SCKEY_P3_MASK & GROUP1) == GROUP1) && ((SCKEY_P3_MASK & GROUP2)== GROUP2))
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000082)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000082)  /*PA1 and PA7*/
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x0000C00C)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004004)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0003)
#elif ((SCKEY_P3_MASK & GROUP1)== GROUP1)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000002)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000002) /* PA1 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x0000000C)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000004)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#elif ((SCKEY_P3_MASK & GROUP2)== GROUP2)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000080) /* PA7 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x0000C000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00004000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0002)
#else
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_A
#undef SCKEY_P3_IO_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_A
#if (((SCKEY_P3_MASK & GROUP4) == GROUP4) && ((SCKEY_P3_MASK & GROUP5)== GROUP5))
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000480)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00004200) /* PA9 and PA14 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x300C0000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10040000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P3_MASK & GROUP4)== GROUP4)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000400)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000200) /* PA9 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x000C0000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00040000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P3_MASK & GROUP5)== GROUP5)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000080)
#define SCKEY_P3_IO_COMP2_MASK _A       ((uint32_t)0x00004000) /* PA14 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x30000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x10000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint32_t)0x0010)
#else
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_A

#ifdef PORT_B
#define P3_PORT_B              (GPIOB)
#define P3_IO_MASK_3           ((uint32_t)0x00000002)   /* PB1 */
#define P3_IO_MASK_7           ((uint32_t)0x00002000)   /* PB13 */
#define P3_IO_MASK_6           ((uint32_t)0x00000020)   /* PB5 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_B
#undef SCKEY_P3_IO_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_B
#if ((((SCKEY_P3_MASK & GROUP3)== GROUP3) && ((SCKEY_P3_MASK & GROUP7)== GROUP7)))
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00080200)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00002002)  /* PB1 and PB13 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x0C00000C)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000004)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0044)
#elif ((SCKEY_P3_MASK & GROUP3)== GROUP3)
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000200)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000002) /* PB1 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x0000000C)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000004)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0004)
#elif (SCKEY_P3_MASK & GROUP7)== GROUP7
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00080000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00002000) /* PB13 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x0C000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x04000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_B
#undef SCKEY_P3_IO_COMP2_MASK_B
#undef SCKEY_P3_MODER_COMP2_MASK_B
#undef SCKEY_P3_MODER_COMP2_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_B
#if ((SCKEY_P3_MASK & GROUP6)== GROUP6)
#define SCKEY_P3_COMP2_MASK_B           ((uint32_t)0x00000020)
#define SCKEY_P3_IO_COMP2_MASK_B        ((uint32_t)0x00000020) /* PB5 */
#define SCKEY_P3_MODER_COMP2_MASK_B     ((uint32_t)0x00000C00)
#define SCKEY_P3_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000400)
#define SCKEY_P3_STATE_COMP2_MASK_B     ((uint16_t)0x0020)
#else
#define SCKEY_P3_COMP2_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_B     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_B

#ifdef PORT_C
#define P3_PORT_C              (GPIOC)
#define P3_IO_MASK_8           ((uint32_t)0x00000002)   /* PC1 */
#define P3_IO_MASK_9           ((uint32_t)0x00000020)   /* PC5 */
#define P3_IO_MASK_10          ((uint32_t)0x00000080)   /* PC7 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_C
#undef SCKEY_P3_IO_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_C
#if (((SCKEY_P3_MASK & GROUP8)== GROUP8) && ((SCKEY_P3_MASK & GROUP9)== GROUP9))
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00008800)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000022)  /* PC1 and PC5 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000C0C)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000404)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0180)
#elif ((SCKEY_P3_MASK & GROUP8)== GROUP8)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000800)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000002) /* PC1 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x0000000C)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000004)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#elif ((SCKEY_P3_MASK & GROUP9)== GROUP9)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00008000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000020) /* PC5 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000C00)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000400)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0100)
#else
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_C
#undef SCKEY_P3_IO_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_C
#if ((SCKEY_P3_MASK & GROUP10)== GROUP10)
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000002)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000080) /* PC7 */
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x0000C000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00004000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P3_CH == CH2)

#if (SCKEY_P3_CH == CH3)

#ifdef PORT_A
#define P3_IO_PORT_A  (GPIOA)
#define P3_IO_MASK_1  ((uint32_t)0x00000004)   /* PA2 */
#define P3_IO_MASK_4  ((uint32_t)0x00000400)   /* PA10 */
#define P3_IO_MASK_5  ((uint32_t)0x00008000)   /* PA15 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_A
#undef SCKEY_P3_IO_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_A
#if ((SCKEY_P3_MASK & GROUP1)== GROUP1)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000004)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000004)  /* PA2 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000030)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000010)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_A
#undef SCKEY_P3_IO_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A
#undef SCKEY_P3_MODER_COMP2_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_A
#if  (((SCKEY_P3_MASK & GROUP4) == GROUP4) &&  ((SCKEY_P3_MASK & GROUP5) == GROUP5))
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000900)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00008400) /* PA10 and PA15 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0xC0300000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40100000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0018)
#elif ((SCKEY_P3_MASK & GROUP4) == GROUP4)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000800)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000400) /* PA10 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x00300000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00100000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0008)
#elif ((SCKEY_P3_MASK & GROUP5) == GROUP5)
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000100)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00008000) /* PA15 */
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0xC0000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x40000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0010)
#else
#define SCKEY_P3_COMP2_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_A     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_A

#ifdef PORT_B
#define P3_PORT_B     (GPIOB)
#define P3_IO_MASK_7  (0x00004000)   /* PB14 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_B
#undef SCKEY_P3_IO_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_B
#if ((SCKEY_P3_MASK & GROUP7)== GROUP7)
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00100000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00004000)  /* PB14 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x30000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x10000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_B

#ifdef PORT_C
#define P3_PORT_C     (GPIOC)
#define P3_IO_MASK_8  ((uint32_t)0x00000004)   /* PC2 */
#define P3_IO_MASK_10 ((uint32_t)0x00000100)  /* PC8 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_C
#undef SCKEY_P3_IO_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_C
#if ((SCKEY_P3_MASK & GROUP8) == GROUP8)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00001000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000004)  /* PC2 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000030)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000010)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_C
#undef SCKEY_P3_IO_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_C
#if ((SCKEY_P3_MASK & GROUP10) == GROUP10)
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000004)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000100) /* PC8 */
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00030000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00010000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif

#endif
//# PORT_C

#endif
//# (SCKEY_P3_CH == CH3)

#if (SCKEY_P3_CH == CH4)

#ifdef PORT_A
#define P3_PORT_A     (GPIOA)
#define P3_IO_MASK_1  ((uint32_t)0x00000008)   /* PA3 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_A
#undef SCKEY_P3_IO_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A
#undef SCKEY_P3_MODER_COMP1_MASK_A_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_A
#if ((SCKEY_P3_MASK & GROUP1) == GROUP1)
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000008)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000008)  /* PA3 */
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x000000C0)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000040)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0001)
#else
#define SCKEY_P3_COMP1_MASK_A           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_A        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_A_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_A     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_A

#ifdef PORT_B
#define P3_PORT_B     (GPIOB)
#define P3_IO_MASK_7  ((uint32_t)0x00008000)   /* PB15 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_B
#undef SCKEY_P3_IO_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B
#undef SCKEY_P3_MODER_COMP1_MASK_B_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_B
#if ((SCKEY_P3_MASK & GROUP7) == GROUP7)
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00200000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00008000)  /* PB15 */
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0xC0000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x40000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0040)
#else
#define SCKEY_P3_COMP1_MASK_B           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_B        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_B_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_B     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_B

#ifdef PORT_C
#define P3_PORT_C           (GPIOC)
#define P3_IO_MASK_8        ((uint32_t)0x00000008)   /* PC3 */
#define P3_IO_MASK_10       ((uint32_t)0x00000200)  /* PC9 */
/* Only the comparator 1 is used */
#ifdef COMP1
#undef SCKEY_P3_COMP1_MASK_C
#undef SCKEY_P3_IO_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C
#undef SCKEY_P3_MODER_COMP1_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP1_MASK_C
#if ((SCKEY_P3_MASK & GROUP8)== GROUP8)
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00002000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000008)  /* PC3 */
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x000000C0)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000040)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0080)
#else
#define SCKEY_P3_COMP1_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP1_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP1_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP1_MASK_C     ((uint16_t)0x0000)
#endif
#endif
/* Only the comparator 2 is used */
#ifdef COMP2
#undef SCKEY_P3_COMP2_MASK_C
#undef SCKEY_P3_IO_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C
#undef SCKEY_P3_MODER_COMP2_MASK_C_OUT
#undef SCKEY_P3_STATE_COMP2_MASK_C
#if ((SCKEY_P3_MASK & GROUP10)== GROUP10)
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000008)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000200) /* PC9 */
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x000C0000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00040000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0200)
#else
#define SCKEY_P3_COMP2_MASK_C           ((uint32_t)0x00000000)
#define SCKEY_P3_IO_COMP2_MASK_C        ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C     ((uint32_t)0x00000000)
#define SCKEY_P3_MODER_COMP2_MASK_C_OUT ((uint32_t)0x00000000)
#define SCKEY_P3_STATE_COMP2_MASK_C     ((uint16_t)0x0000)
#endif
#endif
#endif
//# PORT_C

#endif
//# (SCKEY_P3_CH == CH4)

#endif
//# (NUMBER_OF_SINGLE_CHANNEL_PORTS > 2) || (NUMBER_OF_MULTI_CHANNEL_KEYS > 0)

#define SCKEY_P3_STATE_COMP1_MASK (SCKEY_P3_STATE_COMP1_MASK_A\
                                   |SCKEY_P3_STATE_COMP1_MASK_B\
                                   |SCKEY_P3_STATE_COMP1_MASK_C)

#define SCKEY_P3_STATE_COMP2_MASK (SCKEY_P3_STATE_COMP2_MASK_A\
                                   |SCKEY_P3_STATE_COMP2_MASK_B\
                                   |SCKEY_P3_STATE_COMP2_MASK_C)

#define SCKEY_P3_STATE_MASK (SCKEY_P3_STATE_COMP1_MASK | SCKEY_P3_STATE_COMP2_MASK)

#define P3_MODER_MASK_A_OUT (SCKEY_P3_MODER_COMP1_MASK_A_OUT | SCKEY_P3_MODER_COMP2_MASK_A_OUT | SHIELD_MODER_MASK_A_OUT)
#define P3_MODER_MASK_B_OUT (SCKEY_P3_MODER_COMP1_MASK_B_OUT | SCKEY_P3_MODER_COMP2_MASK_B_OUT | SHIELD_MODER_MASK_A_OUT)
#define P3_MODER_MASK_C_OUT (SCKEY_P3_MODER_COMP1_MASK_C_OUT | SCKEY_P3_MODER_COMP2_MASK_C_OUT | SHIELD_MODER_MASK_A_OUT)


#define P3_MODER_MASK_A (SCKEY_P3_MODER_COMP1_MASK_A | SCKEY_P3_MODER_COMP2_MASK_A | SHIELD_MODER_MASK_A)
#define P3_MODER_MASK_B (SCKEY_P3_MODER_COMP1_MASK_B | SCKEY_P3_MODER_COMP2_MASK_B | SHIELD_MODER_MASK_B)
#define P3_MODER_MASK_C (SCKEY_P3_MODER_COMP1_MASK_C | SCKEY_P3_MODER_COMP2_MASK_C | SHIELD_MODER_MASK_C)

#define P3_COMP1_MASK (SCKEY_P3_COMP1_MASK_A | SCKEY_P3_COMP1_MASK_B | SCKEY_P3_COMP1_MASK_C | SHIELD_COMP1_MASK)
#define P3_COMP2_MASK (SCKEY_P3_COMP2_MASK_A | SCKEY_P3_COMP2_MASK_B | SCKEY_P3_COMP2_MASK_C | SHIELD_COMP2_MASK)

#define P3_IO_MASK_A (SCKEY_P3_IO_COMP1_MASK_A | SCKEY_P3_IO_COMP2_MASK_A | SHIELD_IO_MASK_A)
#define P3_IO_MASK_B (SCKEY_P3_IO_COMP1_MASK_B | SCKEY_P3_IO_COMP2_MASK_B | SHIELD_IO_MASK_B)
#define P3_IO_MASK_C (SCKEY_P3_IO_COMP1_MASK_C | SCKEY_P3_IO_COMP2_MASK_C | SHIELD_IO_MASK_C)


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if SPREAD_SPECTRUM
#if SW_SPREAD_SPECTRUM
__IO uint8_t SpreadCounter;
#else /* !SW_SPREAD_SPECTRUM */
#define HW_SPREAD_FREQ_MIN (0x10-(((SPREAD_COUNTER_MAX-1)>>4)+1))
#define HW_SPREAD_FREQ_MAX (0x10-((SPREAD_COUNTER_MIN-1)>>4))
#endif  //SW_SPREAD_SPECTRUM
//# SW_SPREAD_SPECTRUM
uint32_t Previous_calibration_value;
#endif //SPREAD_SPECTRUM
//# SPREAD_SPECTRUM

TOUCH_PORT_Info_T sTouchIO;

Info_Channel Channel_P1;
Info_Channel Channel_P2;
Info_Channel Channel_P3;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* This table is always defined */

#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 0) && (SCKEY_P1_KEY_COUNT > 0)
const uint8_t Table_SCKEY_P1[SCKEY_P1_KEY_COUNT] =
  {
#if   (SCKEY_P1_A == GROUP1)
    0x00
#elif (SCKEY_P1_A == GROUP2)
    0x01
#elif (SCKEY_P1_A == GROUP3)
    0x02
#elif (SCKEY_P1_A == GROUP4)
    0x03
#elif (SCKEY_P1_A == GROUP5)
    0x04
#elif (SCKEY_P1_A == GROUP6)
    0x05
#elif (SCKEY_P1_A == GROUP7)
    0x06
#elif (SCKEY_P1_A == GROUP8)
    0x07
#elif (SCKEY_P1_A == GROUP9)
    0x08
#elif (SCKEY_P1_A == GROUP10)
    0x09
#endif

#if SCKEY_P1_KEY_COUNT > 1
#if   (SCKEY_P1_B == GROUP1)
    , 0x00
#elif (SCKEY_P1_B == GROUP2)
    , 0x01
#elif (SCKEY_P1_B == GROUP3)
    , 0x02
#elif (SCKEY_P1_B == GROUP4)
    , 0x03
#elif (SCKEY_P1_B == GROUP5)
    , 0x04
#elif (SCKEY_P1_B == GROUP6)
    , 0x05
#elif (SCKEY_P1_B == GROUP7)
    , 0x06
#elif (SCKEY_P1_B == GROUP8)
    , 0x07
#elif (SCKEY_P1_B == GROUP9)
    , 0x08
#elif (SCKEY_P1_B == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 2
#if   (SCKEY_P1_C == GROUP1)
    , 0x00
#elif (SCKEY_P1_C == GROUP2)
    , 0x01
#elif (SCKEY_P1_C == GROUP3)
    , 0x02
#elif (SCKEY_P1_C == GROUP4)
    , 0x03
#elif (SCKEY_P1_C == GROUP5)
    , 0x04
#elif (SCKEY_P1_C == GROUP6)
    , 0x05
#elif (SCKEY_P1_C == GROUP7)
    , 0x06
#elif (SCKEY_P1_C == GROUP8)
    , 0x07
#elif (SCKEY_P1_C == GROUP9)
    , 0x08
#elif (SCKEY_P1_C == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 3
#if   (SCKEY_P1_D == GROUP1)
    , 0x00
#elif (SCKEY_P1_D == GROUP2)
    , 0x01
#elif (SCKEY_P1_D == GROUP3)
    , 0x02
#elif (SCKEY_P1_D == GROUP4)
    , 0x03
#elif (SCKEY_P1_D == GROUP5)
    , 0x04
#elif (SCKEY_P1_D == GROUP6)
    , 0x05
#elif (SCKEY_P1_D == GROUP7)
    , 0x06
#elif (SCKEY_P1_D == GROUP8)
    , 0x07
#elif (SCKEY_P1_D == GROUP9)
    , 0x08
#elif (SCKEY_P1_D == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 4
#if   (SCKEY_P1_E == GROUP1)
    , 0x00
#elif (SCKEY_P1_E == GROUP2)
    , 0x01
#elif (SCKEY_P1_E == GROUP3)
    , 0x02
#elif (SCKEY_P1_E == GROUP4)
    , 0x03
#elif (SCKEY_P1_E == GROUP5)
    , 0x04
#elif (SCKEY_P1_E == GROUP6)
    , 0x05
#elif (SCKEY_P1_E == GROUP7)
    , 0x06
#elif (SCKEY_P1_E == GROUP8)
    , 0x07
#elif (SCKEY_P1_E == GROUP9)
    , 0x08
#elif (SCKEY_P1_E == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 5
#if   (SCKEY_P1_F == GROUP1)
    , 0x00
#elif (SCKEY_P1_F == GROUP2)
    , 0x01
#elif (SCKEY_P1_F == GROUP3)
    , 0x02
#elif (SCKEY_P1_F == GROUP4)
    , 0x03
#elif (SCKEY_P1_F == GROUP5)
    , 0x04
#elif (SCKEY_P1_F == GROUP6)
    , 0x05
#elif (SCKEY_P1_F == GROUP7)
    , 0x06
#elif (SCKEY_P1_F == GROUP8)
    , 0x07
#elif (SCKEY_P1_F == GROUP9)
    , 0x08
#elif (SCKEY_P1_F == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 6
#if   (SCKEY_P1_G == GROUP1)
    , 0x00
#elif (SCKEY_P1_G == GROUP2)
    , 0x01
#elif (SCKEY_P1_G == GROUP3)
    , 0x02
#elif (SCKEY_P1_G == GROUP4)
    , 0x03
#elif (SCKEY_P1_G == GROUP5)
    , 0x04
#elif (SCKEY_P1_G == GROUP6)
    , 0x05
#elif (SCKEY_P1_G == GROUP7)
    , 0x06
#elif (SCKEY_P1_G == GROUP8)
    , 0x07
#elif (SCKEY_P1_G == GROUP9)
    , 0x08
#elif (SCKEY_P1_G == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 7
#if   (SCKEY_P1_H == GROUP1)
    , 0x00
#elif (SCKEY_P1_H == GROUP2)
    , 0x01
#elif (SCKEY_P1_H == GROUP3)
    , 0x02
#elif (SCKEY_P1_H == GROUP4)
    , 0x03
#elif (SCKEY_P1_H == GROUP5)
    , 0x04
#elif (SCKEY_P1_H == GROUP6)
    , 0x05
#elif (SCKEY_P1_H == GROUP7)
    , 0x06
#elif (SCKEY_P1_H == GROUP8)
    , 0x07
#elif (SCKEY_P1_H == GROUP9)
    , 0x08
#elif (SCKEY_P1_H == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 8
#if   (SCKEY_P1_I == GROUP1)
    , 0x00
#elif (SCKEY_P1_I == GROUP2)
    , 0x01
#elif (SCKEY_P1_I == GROUP3)
    , 0x02
#elif (SCKEY_P1_I == GROUP4)
    , 0x03
#elif (SCKEY_P1_I == GROUP5)
    , 0x04
#elif (SCKEY_P1_I == GROUP6)
    , 0x05
#elif (SCKEY_P1_I == GROUP7)
    , 0x06
#elif (SCKEY_P1_I == GROUP8)
    , 0x07
#elif (SCKEY_P1_I == GROUP9)
    , 0x08
#elif (SCKEY_P1_I == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P1_KEY_COUNT > 9
#if   (SCKEY_P1_J == GROUP1)
    , 0x00
#elif (SCKEY_P1_J == GROUP2)
    , 0x01
#elif (SCKEY_P1_J == GROUP3)
    , 0x02
#elif (SCKEY_P1_J== GROUP4)
    , 0x03
#elif (SCKEY_P1_J == GROUP5)
    , 0x04
#elif (SCKEY_P1_J == GROUP6)
    , 0x05
#elif (SCKEY_P1_J == GROUP7)
    , 0x06
#elif (SCKEY_P1_J == GROUP8)
    , 0x07
#elif (SCKEY_P1_J == GROUP9)
    , 0x08
#elif (SCKEY_P1_J == GROUP10)
    , 0x09
#endif
#endif
  };
#endif
//# NUMBER_OF_SINGLE_CHANNEL_PORTS > 0

#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 1
const uint8_t Table_SCKEY_P2[SCKEY_P2_KEY_COUNT] =
  {
#if   (SCKEY_P2_A == GROUP1)
    0x00
#elif (SCKEY_P2_A == GROUP2)
    0x01
#elif (SCKEY_P2_A == GROUP3)
    0x02
#elif (SCKEY_P2_A == GROUP4)
    0x03
#elif (SCKEY_P2_A == GROUP5)
    0x04
#elif (SCKEY_P2_A == GROUP6)
    0x05
#elif (SCKEY_P2_A == GROUP7)
    0x06
#elif (SCKEY_P2_A == GROUP8)
    0x07
#elif (SCKEY_P2_A == GROUP9)
    0x08
#elif (SCKEY_P2_A == GROUP10)
    0x09
#endif

#if SCKEY_P2_KEY_COUNT > 1
#if   (SCKEY_P2_B == GROUP1)
    , 0x00
#elif (SCKEY_P2_B == GROUP2)
    , 0x01
#elif (SCKEY_P2_B == GROUP3)
    , 0x02
#elif (SCKEY_P2_B == GROUP4)
    , 0x03
#elif (SCKEY_P2_B == GROUP5)
    , 0x04
#elif (SCKEY_P2_B == GROUP6)
    , 0x05
#elif (SCKEY_P2_B == GROUP7)
    , 0x06
#elif (SCKEY_P2_B == GROUP8)
    , 0x07
#elif (SCKEY_P2_B == GROUP9)
    , 0x08
#elif (SCKEY_P2_B == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 2
#if   (SCKEY_P2_C == GROUP1)
    , 0x00
#elif (SCKEY_P2_C == GROUP2)
    , 0x01
#elif (SCKEY_P2_C == GROUP3)
    , 0x02
#elif (SCKEY_P2_C == GROUP4)
    , 0x03
#elif (SCKEY_P2_C == GROUP5)
    , 0x04
#elif (SCKEY_P2_C == GROUP6)
    , 0x05
#elif (SCKEY_P2_C == GROUP7)
    , 0x06
#elif (SCKEY_P2_C == GROUP8)
    , 0x07
#elif (SCKEY_P2_C == GROUP9)
    , 0x08
#elif (SCKEY_P2_C == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 3
#if   (SCKEY_P2_D == GROUP1)
    , 0x00
#elif (SCKEY_P2_D == GROUP2)
    , 0x01
#elif (SCKEY_P2_D == GROUP3)
    , 0x02
#elif (SCKEY_P2_D == GROUP4)
    , 0x03
#elif (SCKEY_P2_D == GROUP5)
    , 0x04
#elif (SCKEY_P2_D == GROUP6)
    , 0x05
#elif (SCKEY_P2_D == GROUP7)
    , 0x06
#elif (SCKEY_P2_D == GROUP8)
    , 0x07
#elif (SCKEY_P2_D == GROUP9)
    , 0x08
#elif (SCKEY_P2_D == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 4
#if   (SCKEY_P2_E == GROUP1)
    , 0x00
#elif (SCKEY_P2_E == GROUP2)
    , 0x01
#elif (SCKEY_P2_E == GROUP3)
    , 0x02
#elif (SCKEY_P2_E == GROUP4)
    , 0x03
#elif (SCKEY_P2_E == GROUP5)
    , 0x04
#elif (SCKEY_P2_E == GROUP6)
    , 0x05
#elif (SCKEY_P2_E == GROUP7)
    , 0x06
#elif (SCKEY_P2_E == GROUP8)
    , 0x07
#elif (SCKEY_P2_E == GROUP9)
    , 0x08
#elif (SCKEY_P2_E == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 5
#if   (SCKEY_P2_F == GROUP1)
    , 0x00
#elif (SCKEY_P2_F == GROUP2)
    , 0x01
#elif (SCKEY_P2_F == GROUP3)
    , 0x02
#elif (SCKEY_P2_F == GROUP4)
    , 0x03
#elif (SCKEY_P2_F == GROUP5)
    , 0x04
#elif (SCKEY_P2_F == GROUP6)
    , 0x05
#elif (SCKEY_P2_F == GROUP7)
    , 0x06
#elif (SCKEY_P2_F == GROUP8)
    , 0x07
#elif (SCKEY_P2_F == GROUP9)
    , 0x08
#elif (SCKEY_P2_F == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 6
#if   (SCKEY_P2_G == GROUP1)
    , 0x00
#elif (SCKEY_P2_G == GROUP2)
    , 0x01
#elif (SCKEY_P2_G == GROUP3)
    , 0x02
#elif (SCKEY_P2_G == GROUP4)
    , 0x03
#elif (SCKEY_P2_G == GROUP5)
    , 0x04
#elif (SCKEY_P2_G == GROUP6)
    , 0x05
#elif (SCKEY_P2_G == GROUP7)
    , 0x06
#elif (SCKEY_P2_G == GROUP8)
    , 0x07
#elif (SCKEY_P2_G == GROUP9)
    , 0x08
#elif (SCKEY_P2_G == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 7
#if   (SCKEY_P2_H == GROUP1)
    , 0x00
#elif (SCKEY_P2_H == GROUP2)
    , 0x01
#elif (SCKEY_P2_H == GROUP3)
    , 0x02
#elif (SCKEY_P2_H == GROUP4)
    , 0x03
#elif (SCKEY_P2_H == GROUP5)
    , 0x04
#elif (SCKEY_P2_H == GROUP6)
    , 0x05
#elif (SCKEY_P2_H == GROUP7)
    , 0x06
#elif (SCKEY_P2_H == GROUP8)
    , 0x07
#elif (SCKEY_P2_H == GROUP9)
    , 0x08
#elif (SCKEY_P2_H == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 8
#if   (SCKEY_P2_I == GROUP1)
    , 0x00
#elif (SCKEY_P2_I == GROUP2)
    , 0x01
#elif (SCKEY_P2_I == GROUP3)
    , 0x02
#elif (SCKEY_P2_I == GROUP4)
    , 0x03
#elif (SCKEY_P2_I == GROUP5)
    , 0x04
#elif (SCKEY_P2_I == GROUP6)
    , 0x05
#elif (SCKEY_P2_I == GROUP7)
    , 0x06
#elif (SCKEY_P2_I == GROUP8)
    , 0x07
#elif (SCKEY_P2_I == GROUP9)
    , 0x08
#elif (SCKEY_P2_I == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P2_KEY_COUNT > 9
#if   (SCKEY_P2_J == GROUP1)
    , 0x00
#elif (SCKEY_P2_J == GROUP2)
    , 0x01
#elif (SCKEY_P2_J == GROUP3)
    , 0x02
#elif (SCKEY_P2_J == GROUP4)
    , 0x03
#elif (SCKEY_P2_J == GROUP5)
    , 0x04
#elif (SCKEY_P2_J == GROUP6)
    , 0x05
#elif (SCKEY_P2_J == GROUP7)
    , 0x06
#elif (SCKEY_P2_J == GROUP8)
    , 0x07
#elif (SCKEY_P2_J == GROUP9)
    , 0x08
#elif (SCKEY_P2_J == GROUP10)
    , 0x09
#endif
#endif
  };
#endif
//# NUMBER_OF_SINGLE_CHANNEL_PORTS > 1

#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 2

const uint8_t Table_SCKEY_P3[SCKEY_P3_KEY_COUNT] =
  {
#if   (SCKEY_P3_A == GROUP1)
    0x00
#elif (SCKEY_P3_A == GROUP2)
    0x01
#elif (SCKEY_P3_A == GROUP3)
    0x02
#elif (SCKEY_P3_A == GROUP4)
    0x03
#elif (SCKEY_P3_A == GROUP5)
    0x04
#elif (SCKEY_P3_A == GROUP6)
    0x05
#elif (SCKEY_P3_A == GROUP7)
    0x06
#elif (SCKEY_P3_A == GROUP8)
    0x07
#elif (SCKEY_P3_A == GROUP9)
    0x08
#elif (SCKEY_P3_A == GROUP10)
    0x09
#endif

#if SCKEY_P3_KEY_COUNT > 1
#if   (SCKEY_P3_B == GROUP1)
    , 0x00
#elif (SCKEY_P3_B == GROUP2)
    , 0x01
#elif (SCKEY_P3_B == GROUP3)
    , 0x02
#elif (SCKEY_P3_B == GROUP4)
    , 0x03
#elif (SCKEY_P3_B == GROUP5)
    , 0x04
#elif (SCKEY_P3_B == GROUP6)
    , 0x05
#elif (SCKEY_P3_B == GROUP7)
    , 0x06
#elif (SCKEY_P3_B == GROUP8)
    , 0x07
#elif (SCKEY_P3_B == GROUP9)
    , 0x08
#elif (SCKEY_P3_B == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 2
#if   (SCKEY_P3_C == GROUP1)
    , 0x00
#elif (SCKEY_P3_C == GROUP2)
    , 0x01
#elif (SCKEY_P3_C == GROUP3)
    , 0x02
#elif (SCKEY_P3_C == GROUP4)
    , 0x03
#elif (SCKEY_P3_C == GROUP5)
    , 0x04
#elif (SCKEY_P3_C == GROUP6)
    , 0x05
#elif (SCKEY_P3_C == GROUP7)
    , 0x06
#elif (SCKEY_P3_C == GROUP8)
    , 0x07
#elif (SCKEY_P3_C == GROUP9)
    , 0x08
#elif (SCKEY_P3_C == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 3
#if   (SCKEY_P3_D == GROUP1)
    , 0x00
#elif (SCKEY_P3_D == GROUP2)
    , 0x01
#elif (SCKEY_P3_D == GROUP3)
    , 0x02
#elif (SCKEY_P3_D == GROUP4)
    , 0x03
#elif (SCKEY_P3_D == GROUP5)
    , 0x04
#elif (SCKEY_P3_D == GROUP6)
    , 0x05
#elif (SCKEY_P3_D == GROUP7)
    , 0x06
#elif (SCKEY_P3_D == GROUP8)
    , 0x07
#elif (SCKEY_P3_D == GROUP9)
    , 0x08
#elif (SCKEY_P3_D == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 4
#if   (SCKEY_P3_E == GROUP1)
    , 0x00
#elif (SCKEY_P3_E == GROUP2)
    , 0x01
#elif (SCKEY_P3_E == GROUP3)
    , 0x02
#elif (SCKEY_P3_E == GROUP4)
    , 0x03
#elif (SCKEY_P3_E == GROUP5)
    , 0x04
#elif (SCKEY_P3_E == GROUP6)
    , 0x05
#elif (SCKEY_P3_E == GROUP7)
    , 0x06
#elif (SCKEY_P3_E == GROUP8)
    , 0x07
#elif (SCKEY_P3_E == GROUP9)
    , 0x08
#elif (SCKEY_P3_E == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 5
#if   (SCKEY_P3_F == GROUP1)
    , 0x00
#elif (SCKEY_P3_F == GROUP2)
    , 0x01
#elif (SCKEY_P3_F == GROUP3)
    , 0x02
#elif (SCKEY_P3_F == GROUP4)
    , 0x03
#elif (SCKEY_P3_F == GROUP5)
    , 0x04
#elif (SCKEY_P3_F == GROUP6)
    , 0x05
#elif (SCKEY_P3_F == GROUP7)
    , 0x06
#elif (SCKEY_P3_F == GROUP8)
    , 0x07
#elif (SCKEY_P3_F == GROUP9)
    , 0x08
#elif (SCKEY_P3_F == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 6
#if   (SCKEY_P3_G == GROUP1)
    , 0x00
#elif (SCKEY_P3_G == GROUP2)
    , 0x01
#elif (SCKEY_P3_G == GROUP3)
    , 0x02
#elif (SCKEY_P3_G == GROUP4)
    , 0x03
#elif (SCKEY_P3_G == GROUP5)
    , 0x04
#elif (SCKEY_P3_G == GROUP6)
    , 0x05
#elif (SCKEY_P3_G == GROUP7)
    , 0x06
#elif (SCKEY_P3_G == GROUP8)
    , 0x07
#elif (SCKEY_P3_G == GROUP9)
    , 0x08
#elif (SCKEY_P3_G == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 7
#if   (SCKEY_P3_H == GROUP1)
    , 0x00
#elif (SCKEY_P3_H == GROUP2)
    , 0x01
#elif (SCKEY_P3_H == GROUP3)
    , 0x02
#elif (SCKEY_P3_H == GROUP4)
    , 0x03
#elif (SCKEY_P3_H == GROUP5)
    , 0x04
#elif (SCKEY_P3_H == GROUP6)
    , 0x05
#elif (SCKEY_P3_H == GROUP7)
    , 0x06
#elif (SCKEY_P3_H == GROUP8)
    , 0x07
#elif (SCKEY_P3_H == GROUP9)
    , 0x08
#elif (SCKEY_P3_H == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 8
#if   (SCKEY_P3_I == GROUP1)
    , 0x00
#elif (SCKEY_P3_I == GROUP2)
    , 0x01
#elif (SCKEY_P3_I == GROUP3)
    , 0x02
#elif (SCKEY_P3_I == GROUP4)
    , 0x03
#elif (SCKEY_P3_I == GROUP5)
    , 0x04
#elif (SCKEY_P3_I == GROUP6)
    , 0x05
#elif (SCKEY_P3_I == GROUP7)
    , 0x06
#elif (SCKEY_P3_I == GROUP8)
    , 0x07
#elif (SCKEY_P3_I == GROUP9)
    , 0x08
#elif (SCKEY_P3_I == GROUP10)
    , 0x09
#endif
#endif
#if SCKEY_P3_KEY_COUNT > 9
#if   (SCKEY_P3_J == GROUP1)
    , 0x00
#elif (SCKEY_P3_J == GROUP2)
    , 0x01
#elif (SCKEY_P3_J == GROUP3)
    , 0x02
#elif (SCKEY_P3_J == GROUP4)
    , 0x03
#elif (SCKEY_P3_J == GROUP5)
    , 0x04
#elif (SCKEY_P3_J == GROUP6)
    , 0x05
#elif (SCKEY_P3_J == GROUP7)
    , 0x06
#elif (SCKEY_P3_J == GROUP8)
    , 0x07
#elif (SCKEY_P3_J == GROUP9)
    , 0x08
#elif (SCKEY_P3_J == GROUP10)
    , 0x09
#endif
#endif
  };
#endif
//# NUMBER_OF_SINGLE_CHANNEL_PORTS > 2

/* Table_SCKEY_BITS contents the individual masks for the port acquisitions */

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
const uint16_t Table_SCKEY_BITS[
  SCKEY_P1_KEY_COUNT
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 1
  + SCKEY_P2_KEY_COUNT
#endif
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 2
  + SCKEY_P3_KEY_COUNT
#endif
] =
  {
    SCKEY_P1_A /* Always defined */
#if SCKEY_P1_KEY_COUNT > 1
    , SCKEY_P1_B
#endif
#if SCKEY_P1_KEY_COUNT > 2
    , SCKEY_P1_C
#endif
#if SCKEY_P1_KEY_COUNT > 3
    , SCKEY_P1_D
#endif
#if SCKEY_P1_KEY_COUNT > 4
    , SCKEY_P1_E
#endif
#if SCKEY_P1_KEY_COUNT > 5
    , SCKEY_P1_F
#endif
#if SCKEY_P1_KEY_COUNT > 6
    , SCKEY_P1_G
#endif
#if SCKEY_P1_KEY_COUNT > 7
    , SCKEY_P1_H
#endif
#if SCKEY_P1_KEY_COUNT > 8
    , SCKEY_P1_I
#endif
#if SCKEY_P1_KEY_COUNT > 9
    , SCKEY_P1_J
#endif

#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 1
    , SCKEY_P2_A
#if SCKEY_P2_KEY_COUNT > 1
    , SCKEY_P2_B
#endif
#if SCKEY_P2_KEY_COUNT > 2
    , SCKEY_P2_C
#endif
#if SCKEY_P2_KEY_COUNT > 3
    , SCKEY_P2_D
#endif
#if SCKEY_P2_KEY_COUNT > 4
    , SCKEY_P2_E
#endif
#if SCKEY_P2_KEY_COUNT > 5
    , SCKEY_P2_F
#endif
#if SCKEY_P2_KEY_COUNT > 6
    , SCKEY_P2_G
#endif
#if SCKEY_P2_KEY_COUNT > 7
    , SCKEY_P2_H
#endif
#if SCKEY_P2_KEY_COUNT > 8
    , SCKEY_P2_I
#endif
#if SCKEY_P2_KEY_COUNT > 9
    , SCKEY_P2_J
#endif
#endif

#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 2
    , SCKEY_P3_A
#if SCKEY_P3_KEY_COUNT > 1
    , SCKEY_P3_B
#endif
#if SCKEY_P3_KEY_COUNT > 2
    , SCKEY_P3_C
#endif
#if SCKEY_P3_KEY_COUNT > 3
    , SCKEY_P3_D
#endif
#if SCKEY_P3_KEY_COUNT > 4
    , SCKEY_P3_E
#endif
#if SCKEY_P3_KEY_COUNT > 5
    , SCKEY_P3_F
#endif
#if SCKEY_P3_KEY_COUNT > 6
    , SCKEY_P3_G
#endif
#if SCKEY_P3_KEY_COUNT > 7
    , SCKEY_P3_H
#endif
#if SCKEY_P3_KEY_COUNT > 8
    , SCKEY_P3_I
#endif
#if SCKEY_P3_KEY_COUNT > 9
    , SCKEY_P3_J
#endif
#endif
  };
#endif
//# NUMBER_OF_SINGLE_CHANNEL_KEYS > 0

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
const uint16_t Table_MCKEY_PORTS[
#if NUMBER_OF_MULTI_CHANNEL_KEYS == 1
  CHANNEL_PER_MCKEY
#endif
#if NUMBER_OF_MULTI_CHANNEL_KEYS == 2
  CHANNEL_PER_MCKEY + CHANNEL_PER_MCKEY
#endif
] =
  {
    MCKEY1_A_CH,
    MCKEY1_B_CH,
    MCKEY1_C_CH
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
    , MCKEY2_A_CH
    , MCKEY2_B_CH
    , MCKEY2_C_CH
#endif
  };
#endif
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 0

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
const  uint16_t Table_MCKEY_BITS[
#if NUMBER_OF_MULTI_CHANNEL_KEYS == 1
  CHANNEL_PER_MCKEY
#elif NUMBER_OF_MULTI_CHANNEL_KEYS == 2
  CHANNEL_PER_MCKEY + CHANNEL_PER_MCKEY
#endif
] =
  {
    MCKEY1_A,
    MCKEY1_B,
    MCKEY1_C
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
    , MCKEY2_A
    , MCKEY2_B
    , MCKEY2_C
#endif
  };
#endif
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 0


#if defined ( __CC_ARM ) || defined( __GNUC__ )// KEIL & GNU
__INLINE void __TSL_wait_CLWHTA(void)
#elif defined ( __ICCARM__ ) // IAR
#pragma inline=forced
void __TSL_wait_CLWHTA(void)
#endif
{
#if CLWHTA > 0
  TSL_NOP
#endif
#if CLWHTA > 1
  TSL_NOP
#endif
#if CLWHTA > 2
  TSL_NOP
#endif
#if CLWHTA > 3
  TSL_NOP
#endif
#if CLWHTA > 4
  TSL_NOP
#endif
#if CLWHTA > 5
  TSL_NOP
#endif
#if CLWHTA > 6
  TSL_NOP
#endif
#if CLWHTA > 7
  TSL_NOP
#endif
#if CLWHTA > 8
  TSL_NOP
#endif
#if CLWHTA > 9
  TSL_NOP
#endif
#if CLWHTA > 10
  TSL_NOP
#endif
#if CLWHTA > 11
  TSL_NOP
#endif
#if CLWHTA > 12
  TSL_NOP
#endif
#if CLWHTA > 13
  TSL_NOP
#endif
#if CLWHTA > 14
  TSL_NOP
#endif
#if CLWHTA > 15
  TSL_NOP
#endif
#if CLWHTA > 16
  TSL_NOP
#endif
#if CLWHTA > 17
  TSL_NOP
#endif
#if CLWHTA > 18
  TSL_NOP
#endif
#if CLWHTA > 19
  TSL_NOP
#endif
#if CLWHTA > 20
  TSL_NOP
#endif
#if CLWHTA > 21
  TSL_NOP
#endif
#if CLWHTA > 22
  TSL_NOP
#endif
#if CLWHTA > 23
  TSL_NOP
#endif
#if CLWHTA > 24
  TSL_NOP
#endif
#if CLWHTA > 25
  TSL_NOP
#endif
#if CLWHTA > 26
  TSL_NOP
#endif
#if CLWHTA > 27
  TSL_NOP
#endif
#if CLWHTA > 28
  TSL_NOP
#endif
#if CLWHTA > 29
  TSL_NOP
#endif
#if CLWHTA > 30
  TSL_NOP
#endif
#if CLWHTA > 31
  TSL_NOP
#endif
#if CLWHTA > 32
  TSL_NOP
#endif
#if CLWHTA > 33
  TSL_NOP
#endif
#if CLWHTA > 34
  TSL_NOP
#endif
#if CLWHTA > 35
  TSL_NOP
#endif
#if CLWHTA > 36
  TSL_NOP
#endif
#if CLWHTA > 37
  TSL_NOP
#endif
#if CLWHTA > 38
  TSL_NOP
#endif
#if CLWHTA > 39
  TSL_NOP
#endif
#if CLWHTA > 40
  TSL_NOP
#endif
#if CLWHTA > 41
  TSL_NOP
#endif
#if CLWHTA > 42
  TSL_NOP
#endif
#if CLWHTA > 43
  TSL_NOP
#endif
#if CLWHTA > 44
  TSL_NOP
#endif
#if CLWHTA > 45
  TSL_NOP
#endif
#if CLWHTA > 46
  TSL_NOP
#endif
#if CLWHTA > 47
  TSL_NOP
#endif
#if CLWHTA > 48
  TSL_NOP
#endif
#if CLWHTA > 49
  TSL_NOP
#endif
}


#if defined ( __CC_ARM )  || defined( __GNUC__ )// KEIL
__INLINE void __TSL_wait_CLWLTA(void)
#elif defined ( __ICCARM__ ) // IAR
#pragma inline=forced
void __TSL_wait_CLWLTA(void)
#endif
{
#if CLWLTA > 0
  TSL_NOP
#endif
#if CLWLTA > 1
  TSL_NOP
#endif
#if CLWLTA > 2
  TSL_NOP
#endif
#if CLWLTA > 3
  TSL_NOP
#endif
#if CLWLTA > 4
  TSL_NOP
#endif
#if CLWLTA > 5
  TSL_NOP
#endif
#if CLWLTA > 6
  TSL_NOP
#endif
#if CLWLTA > 7
  TSL_NOP
#endif
#if CLWLTA > 8
  TSL_NOP
#endif
#if CLWLTA > 9
  TSL_NOP
#endif
#if CLWLTA > 10
  TSL_NOP
#endif
#if CLWLTA > 11
  TSL_NOP
#endif
#if CLWLTA > 12
  TSL_NOP
#endif
#if CLWLTA > 13
  TSL_NOP
#endif
#if CLWLTA > 14
  TSL_NOP
#endif
#if CLWLTA > 15
  TSL_NOP
#endif
#if CLWLTA > 16
  TSL_NOP
#endif
#if CLWLTA > 17
  TSL_NOP
#endif
#if CLWLTA > 18
  TSL_NOP
#endif
#if CLWLTA > 19
  TSL_NOP
#endif
#if CLWLTA > 20
  TSL_NOP
#endif
#if CLWLTA > 21
  TSL_NOP
#endif
#if CLWLTA > 22
  TSL_NOP
#endif
#if CLWLTA > 23
  TSL_NOP
#endif
#if CLWLTA > 24
  TSL_NOP
#endif
#if CLWLTA > 25
  TSL_NOP
#endif
#if CLWLTA > 26
  TSL_NOP
#endif
#if CLWLTA > 27
  TSL_NOP
#endif
#if CLWLTA > 28
  TSL_NOP
#endif
#if CLWLTA > 29
  TSL_NOP
#endif
#if CLWLTA > 30
  TSL_NOP
#endif
#if CLWLTA > 31
  TSL_NOP
#endif
#if CLWLTA > 32
  TSL_NOP
#endif
#if CLWLTA > 33
  TSL_NOP
#endif
#if CLWLTA > 34
  TSL_NOP
#endif
#if CLWLTA > 35
  TSL_NOP
#endif
#if CLWLTA > 36
  TSL_NOP
#endif
#if CLWLTA > 37
  TSL_NOP
#endif
#if CLWLTA > 38
  TSL_NOP
#endif
#if CLWLTA > 39
  TSL_NOP
#endif
#if CLWLTA > 40
  TSL_NOP
#endif
#if CLWLTA > 41
  TSL_NOP
#endif
#if CLWLTA > 42
  TSL_NOP
#endif
#if CLWLTA > 43
  TSL_NOP
#endif
#if CLWLTA > 44
  TSL_NOP
#endif
#if CLWLTA > 45
  TSL_NOP
#endif
#if CLWLTA > 46
  TSL_NOP
#endif
#if CLWLTA > 47
  TSL_NOP
#endif
#if CLWLTA > 48
  TSL_NOP
#endif
#if CLWLTA > 49
  TSL_NOP
#endif
}


#if (SPREAD_SPECTRUM > 0) && (SW_SPREAD_SPECTRUM > 0)

/**
  ******************************************************************************
  * @brief Spread Spectrum waiting routine. A variable delay is
  * inserted between each step.
  * @param None
  * @retval None
  ******************************************************************************
  */
#if (USE_INLINED_FUNCTIONS != 0)
__INLINE void TSL_SW_Spread_Spectrum(void)
#else
void TSL_SW_Spread_Spectrum(void)
#endif
{
  uint8_t i;

  SpreadCounter++;

  if (SpreadCounter == SPREAD_COUNTER_MAX)
  {
    SpreadCounter = SPREAD_COUNTER_MIN;
  }
  i = SpreadCounter;
  while (--i);

}

#endif
//# (SPREAD_SPECTRUM > 0) && (SW_SPREAD_SPECTRUM > 0)

/**
  ******************************************************************************
  * @brief Init for I/Os used in the application. Used for SW I/O toggling.
  * If the hardware cell is used, the responsability of the I/O configuration is
  * given to the user layer.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_IO_Init(void)
{

#ifdef PORT_A
  /* Enables GPIOA clock */
  RCC->AHBENR |= RCC_AHBPeriph_GPIOA ;
  /* Disables Hysteresis Register */
  RI->HYSCR1 |= (uint32_t)(SAMP_CAP_IO_MASK_A) | (uint32_t)(P1_IO_MASK_A) | (uint32_t)(P2_IO_MASK_A) | (uint32_t)(P3_IO_MASK_A);
#endif
#ifdef PORT_B
  /* Enables GIOB clock */
  RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
  /* Disables Hysteresis Register */
  RI->HYSCR1 |= (uint32_t)(((uint32_t)(SAMP_CAP_IO_MASK_B) | (uint32_t)(P1_IO_MASK_B) | (uint32_t)(P2_IO_MASK_B) | (uint32_t)(P3_IO_MASK_B)) << 16);
#endif
#ifdef PORT_C
  /* Enables GPIOC clock */
  RCC->AHBENR |= RCC_AHBPeriph_GPIOC ;
  /* Disables Hysteresis Register */
  RI->HYSCR2 |= (uint32_t)(SAMP_CAP_IO_MASK_C) | (uint32_t)(P1_IO_MASK_C) | (uint32_t)(P2_IO_MASK_C) | (uint32_t)(P3_IO_MASK_C);
#endif

  /* Enables the comparator interface clock */
  RCC->APB1ENR |= RCC_APB1Periph_COMP;

  /* Enable RI Switch */
  RI->ASCR1 &= (uint32_t)(~0x80000000); // ADC analog switches open !!!

#ifdef PORT_A
  GPIOA->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_A) | (uint16_t)(~P1_IO_MASK_A);  /* Output push pull config */
  GPIOA->PUPDR &= (uint32_t)(~P1_MODER_MASK_A) | (uint32_t)(~SAMP_CAP_MODER_MASK_A);
#endif
#ifdef PORT_B
  GPIOB->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_B) | (uint16_t)(~P1_IO_MASK_B);  /* Output push pull config */
  GPIOB->PUPDR &= (uint32_t)(~P1_MODER_MASK_B) | (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_C) | (uint16_t)(~P1_IO_MASK_C);  /* Output push pull config */
  GPIOC->PUPDR &= (uint32_t)(~P1_MODER_MASK_C) | (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 1)
#ifdef PORT_A
  GPIOA->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_A) | (uint16_t)(~P2_IO_MASK_A);  /* Output push pull config */
  GPIOA->PUPDR &= (uint32_t)(~P2_MODER_MASK_A) | (uint32_t)(~SAMP_CAP_MODER_MASK_A);

#endif
#ifdef PORT_B
  GPIOB->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_B) | (uint16_t)(~P2_IO_MASK_B);  /* Output push pull config */
  GPIOB->PUPDR &= (uint32_t)(~P2_MODER_MASK_B) | (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_C) | (uint16_t)(~P2_IO_MASK_C);  /* Output push pull config */
  GPIOC->PUPDR &= (uint32_t)(~P2_MODER_MASK_C) | (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#endif

#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 2)
#ifdef PORT_A
  GPIOA->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_A) | (uint16_t)(~P3_IO_MASK_A);  /* Output push pull config */
  GPIOA->PUPDR &= (uint32_t)(~P3_MODER_MASK_A) | (uint32_t)(~SAMP_CAP_MODER_MASK_A);

#endif
#ifdef PORT_B
  GPIOB->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_B) | (uint16_t)(~P3_IO_MASK_B);  /* Output push pull config */
  GPIOB->PUPDR &= (uint32_t)(~P3_MODER_MASK_B) | (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->OTYPER &= (uint16_t)(~SAMP_CAP_IO_MASK_C) | (uint16_t)(~P3_IO_MASK_C);  /* Output push pull config */
  GPIOC->PUPDR &= (uint32_t)(~P3_MODER_MASK_C) | (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#endif

  TSL_IO_Clamp();

}


/**
  ******************************************************************************
  * @brief Put All Sensing I/Os in ouput mode at 0.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_IO_Clamp(void)
{
  /* Open the analog switches */
#ifdef COMP1
  RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P1_COMP1_MASK));
#endif
#ifdef COMP2
  RI->ASCR2 &= (uint32_t)(~(SAMP_CAP_COMP2_MASK | P1_COMP2_MASK));
#endif

  /* Discharging sampling capacitor */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(SAMP_CAP_MODER_MASK_A)) | SAMP_CAP_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(SAMP_CAP_MODER_MASK_B)) | SAMP_CAP_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(SAMP_CAP_MODER_MASK_C)) | SAMP_CAP_MODER_MASK_C_OUT);
#endif

  /* Discharging Ctouch */
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P1_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P1_MODER_MASK_A)) | P1_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P1_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P1_MODER_MASK_B)) | P1_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P1_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P1_MODER_MASK_C)) | P1_MODER_MASK_C_OUT);
#endif

  /* Discharging Ctouch */
#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 1)
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P2_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P2_MODER_MASK_A)) | P2_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P2_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P2_MODER_MASK_B)) | P2_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P2_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P2_MODER_MASK_C)) | P2_MODER_MASK_C_OUT);
#endif
#endif

  /* Discharging Ctouch */
#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 2)
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P3_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P3_MODER_MASK_A)) | P3_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P3_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P3_MODER_MASK_B)) | P3_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P3_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P3_MODER_MASK_C)) | P3_MODER_MASK_C_OUT);
#endif
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Wait a while for a good discharging of all capacitors */
  wait(100); // ~25탎 with fHCLK = 32MHz

  /* Open the analog switches */
#ifdef COMP1
  RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P1_COMP1_MASK));
#endif
#ifdef COMP2
  RI->ASCR2 &= (uint32_t)(~(SAMP_CAP_COMP2_MASK | P1_COMP2_MASK));
#endif

}


/**
******************************************************************************
  * @brief Acquisition function for the 1st Channel IO of each group
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_IO_Acquisition_P1(void)
{

  uint16_t MeasurementCounter;

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  uint32_t HSI_fact_Calib;
#endif

  /* Reset the counter values */
  MeasurementCounter = 0;
  Channel_P1.Measure[0] = 0;
  Channel_P1.Measure[1] = 0;
  Channel_P1.Measure[2] = 0;
  Channel_P1.Measure[3] = 0;
  Channel_P1.Measure[4] = 0;
  Channel_P1.Measure[5] = 0;
  Channel_P1.Measure[6] = 0;
  Channel_P1.Measure[7] = 0;
  Channel_P1.Measure[8] = 0;
  Channel_P1.Measure[9] = 0;
  Channel_P1.State.whole = 0;

  /* Open the analog switches */
#ifdef COMP1
  RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P1_COMP1_MASK));
#endif
#ifdef COMP2
  RI->ASCR2 &= (uint32_t)(~(SAMP_CAP_COMP2_MASK | P1_COMP2_MASK));
#endif

  /* All IO to pushpull LOW for discharging all capacitors (Ctouch and Csense) */

  /* Discharging sampling capacitor */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(SAMP_CAP_MODER_MASK_A)) | SAMP_CAP_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(SAMP_CAP_MODER_MASK_B)) | SAMP_CAP_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~ SAMP_CAP_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(SAMP_CAP_MODER_MASK_C)) | SAMP_CAP_MODER_MASK_C_OUT);
#endif

  /* Discharging Ctouch */
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P1_IO_MASK_A);
  GPIOA->MODER = (GPIOA->MODER & (uint32_t)(~(P1_MODER_MASK_A))) | (P1_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P1_IO_MASK_B);
  GPIOB->MODER = (GPIOB->MODER & (uint32_t)(~(P1_MODER_MASK_B))) | (uint32_t)P1_MODER_MASK_B_OUT;
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P1_IO_MASK_C);
  GPIOC->MODER = (GPIOC->MODER & (uint32_t)(~(P1_MODER_MASK_C))) | (uint32_t)P1_MODER_MASK_C_OUT;
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Wait a while for a good discharging of all capacitors */
  wait(50); // ~14탎 with fHCLK = 32MHz

  /* All IO in input floating */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->MODER &= (uint32_t)(~P1_MODER_MASK_A);
#endif
#ifdef PORT_B
  GPIOB->MODER &= (uint32_t)(~P1_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->MODER &= (uint32_t)(~P1_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Start HW spread spectrum */
#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Save the user application calibration value */
  HSI_fact_Calib = RCC->ICSCR;
  /* Reset trimmer */
  RCC->ICSCR &= (uint32_t)(~(uint32_t)(0x1F << 8));
  /* Reset Previous_calibration_value */
  Previous_calibration_value = HW_SPREAD_FREQ_MIN;
  /* Init Trimmer */
  RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
#endif

  /* Loop while all the 1st channel of each group have not reach the VIH level */
  do
  {

    /* Close the sampling capacitor analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(SAMP_CAP_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint32_t)(SAMP_CAP_COMP2_MASK);
#endif

    /* Charging Ctouch with connecting the IO to Vdd (io in push-pull HIGH) */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->ODR |= (uint16_t)(P1_IO_MASK_A);  /* HIGH level */
    GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P1_MODER_MASK_A)) | P1_MODER_MASK_A_OUT);/* Output push pull config */
#endif
#ifdef PORT_B
    GPIOB->ODR |= (uint16_t)(P1_IO_MASK_B); /* HIGH level */
    GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P1_MODER_MASK_B)) | P1_MODER_MASK_B_OUT);/* Output push pull config */
#endif
#ifdef PORT_C
    GPIOC->ODR |= (uint16_t)(P1_IO_MASK_C); /* HIGH level */
    GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P1_MODER_MASK_C)) | P1_MODER_MASK_C_OUT);/* Output push pull config */
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Wait a while for a good charging (programmable delay) */
    __TSL_wait_CLWHTA();

#ifdef PORT_A
    if ((Channel_P1.State.b.Grp1 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_1) == SAMP_CAP_IO_MASK_1))
    {
      Channel_P1.Measure[0] = MeasurementCounter;
      Channel_P1.State.b.Grp1 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P1.State.b.Grp7 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_7) == SAMP_CAP_IO_MASK_7))
    {
      Channel_P1.Measure[6] = MeasurementCounter;
      Channel_P1.State.b.Grp7 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P1.State.b.Grp8 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_8) == SAMP_CAP_IO_MASK_8))
    {
      Channel_P1.Measure[7] = MeasurementCounter;
      Channel_P1.State.b.Grp8 = 1;
    }

    if ((Channel_P1.State.b.Grp10 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_10) == SAMP_CAP_IO_MASK_10))
    {
      Channel_P1.Measure[9] = MeasurementCounter;
      Channel_P1.State.b.Grp10 = 1;
    }
#endif

#if (SAMP_CAP_CH != CH4) && (SAMP_CAP_CH != CH3)

#ifdef PORT_A
    if ((Channel_P1.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P1.Measure[3] = MeasurementCounter;
      Channel_P1.State.b.Grp4 = 1;
    }

    if ((Channel_P1.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P1.Measure[4] = MeasurementCounter;
      Channel_P1.State.b.Grp5 = 1;
    }
#endif

#ifdef PORT_A
    if ((Channel_P1.State.b.Grp2 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_2) == SAMP_CAP_IO_MASK_2))
    {
      Channel_P1.Measure[1] = MeasurementCounter;
      Channel_P1.State.b.Grp2 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P1.State.b.Grp3 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_3) == SAMP_CAP_IO_MASK_3))
    {
      Channel_P1.Measure[2] = MeasurementCounter;
      Channel_P1.State.b.Grp3 = 1;
    }
    if ((Channel_P1.State.b.Grp6 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_6) == SAMP_CAP_IO_MASK_6))
    {
      Channel_P1.Measure[5] = MeasurementCounter;
      Channel_P1.State.b.Grp6 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P1.State.b.Grp9 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_9) == SAMP_CAP_IO_MASK_9))
    {
      Channel_P1.Measure[8] = MeasurementCounter;
      Channel_P1.State.b.Grp9 = 1;
    }
#endif

#elif (SAMP_CAP_CH != CH4)

#ifdef PORT_A
    if ((Channel_P1.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P1.Measure[3] = MeasurementCounter;
      Channel_P1.State.b.Grp4 = 1;
    }

    if ((Channel_P1.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P1.Measure[4] = MeasurementCounter;
      Channel_P1.State.b.Grp5 = 1;
    }
#endif

#endif

    MeasurementCounter++;

    /* Configure All IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~P1_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~P1_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~P1_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Charging the Csense cap with connecting it to Ctouch by closing the analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(P1_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint16_t)(P1_COMP2_MASK);
#endif

    /* Wait a while for a good charge transfering (programmable delay) */
    __TSL_wait_CLWLTA();

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
    if (Previous_calibration_value >= (uint8_t)(HW_SPREAD_FREQ_MAX))
    {
      Previous_calibration_value = HW_SPREAD_FREQ_MIN;
      RCC->ICSCR &= (uint32_t)(~0x00001F00);
      RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
    }
    else
    {
      RCC->ICSCR += 0x100;
      Previous_calibration_value++;
    }
#endif

    /* Sampling capacitor IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Open the analog switches to allow checking of the inputs */
#ifdef COMP1
    RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P1_COMP1_MASK));
#endif
#ifdef COMP2
    RI->ASCR2 &= (uint16_t)(~(SAMP_CAP_COMP2_MASK | P1_COMP2_MASK));
#endif

    /*it's better to implement this like that because it's much more faster than to put this test in the "while test" below */
    if (MeasurementCounter > SCKEY_MAX_ACQUISITION)
    {
      __NOP();
      break;
    }

#if SPREAD_SPECTRUM && SW_SPREAD_SPECTRUM
    TSL_SW_Spread_Spectrum();
#endif

  }
  while (((Channel_P1.State.whole & Channel_P1.EnabledChannels) != Channel_P1.EnabledChannels));

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Restore the user application calibration value */
  RCC->ICSCR  = HSI_fact_Calib;
#endif

}


#if NUMBER_OF_ACQUISITION_PORTS > 1

/**
  ******************************************************************************
  * @brief Acquisition function for the 2nd Channel IO of each group
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_IO_Acquisition_P2(void)
{
  uint32_t MeasurementCounter;

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  uint32_t HSI_fact_Calib;
#endif

  /* Reset the counter values */
  MeasurementCounter = 0;
  Channel_P2.Measure[0] = 0;
  Channel_P2.Measure[1] = 0;
  Channel_P2.Measure[2] = 0;
  Channel_P2.Measure[3] = 0;
  Channel_P2.Measure[4] = 0;
  Channel_P2.Measure[5] = 0;
  Channel_P2.Measure[6] = 0;
  Channel_P2.Measure[7] = 0;
  Channel_P2.Measure[8] = 0;
  Channel_P2.Measure[9] = 0;
  Channel_P2.State.whole = 0;

  /* Open the analog switches */
#ifdef COMP1
  RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P2_COMP1_MASK));
#endif
#ifdef COMP2
  RI->ASCR2 &= (uint16_t)(~(SAMP_CAP_COMP2_MASK | P2_COMP2_MASK));
#endif

  /* All IO to pushpull LOW for discharging all capacitors (Ctouch and Csense) */

  /* Discharging sampling capacitor */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(SAMP_CAP_MODER_MASK_A)) | SAMP_CAP_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(SAMP_CAP_MODER_MASK_B)) | SAMP_CAP_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~ SAMP_CAP_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(SAMP_CAP_MODER_MASK_C)) | SAMP_CAP_MODER_MASK_C_OUT);
#endif

  /* Discharging Ctouch */
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P2_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P2_MODER_MASK_A)) | P2_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P2_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P2_MODER_MASK_B)) | P2_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P2_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P2_MODER_MASK_C)) | P2_MODER_MASK_C_OUT);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Wait a while for a good discharging of all capacitors */
  wait(50); // ~14탎 with fHCLK = 32MHz

  /* All IO in input floating */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->MODER &= (uint32_t)(~P2_MODER_MASK_A);
#endif
#ifdef PORT_B
  GPIOB->MODER &= (uint32_t)(~P2_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->MODER &= (uint32_t)(~P2_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Start HW spread spectrum */
#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Save the user application calibration value */
  HSI_fact_Calib = RCC->ICSCR;
  /* Reset trimmer */
  RCC->ICSCR &= (uint32_t)(~(uint32_t)(0x1F << 8));
  /* Reset Previous_calibration_value */
  Previous_calibration_value = HW_SPREAD_FREQ_MIN;
  /* Init Trimmer */
  RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
#endif

  /* Sampling capacitor IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_A);
#endif
#ifdef PORT_B
  GPIOB->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Loop while all the 1st channel of each group have not reach the VIH level */
  do
  {

    /* Close the sampling capacitor analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(SAMP_CAP_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint32_t)(SAMP_CAP_COMP2_MASK);
#endif

    /* Charging Ctouch with connecting the IO to Vdd (io in push-pull HIGH) */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->ODR |= (uint16_t)(P2_IO_MASK_A);  /* HIGH level */
    GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P2_MODER_MASK_A)) | P2_MODER_MASK_A_OUT);/* Output push pull config */
#endif
#ifdef PORT_B
    GPIOB->ODR |= (uint16_t)(P2_IO_MASK_B); /* HIGH level */
    GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P2_MODER_MASK_B)) | P2_MODER_MASK_B_OUT);/* Output push pull config */
#endif
#ifdef PORT_C
    GPIOC->ODR |= (uint16_t)(P2_IO_MASK_C); /* HIGH level */
    GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P2_MODER_MASK_C)) | P2_MODER_MASK_C_OUT);/* Output push pull config */
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Wait a while for a good charging (programmable delay) */
    __TSL_wait_CLWHTA();

#ifdef PORT_A
    if ((Channel_P2.State.b.Grp1 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_1) == SAMP_CAP_IO_MASK_1))
    {
      Channel_P2.Measure[0] = MeasurementCounter;
      Channel_P2.State.b.Grp1 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P2.State.b.Grp7 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_7) == SAMP_CAP_IO_MASK_7))
    {
      Channel_P2.Measure[6] = MeasurementCounter;
      Channel_P2.State.b.Grp7 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P2.State.b.Grp8 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_8) == SAMP_CAP_IO_MASK_8))
    {
      Channel_P2.Measure[7] = MeasurementCounter;
      Channel_P2.State.b.Grp8 = 1;
    }

    if ((Channel_P2.State.b.Grp10 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_10) == SAMP_CAP_IO_MASK_10))
    {
      Channel_P2.Measure[9] = MeasurementCounter;
      Channel_P2.State.b.Grp10 = 1;
    }
#endif

#if (SAMP_CAP_CH != CH4) && (SAMP_CAP_CH != CH3)

#ifdef PORT_A
    if ((Channel_P2.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P2.Measure[3] = MeasurementCounter;
      Channel_P2.State.b.Grp4 = 1;
    }

    if ((Channel_P2.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P2.Measure[4] = MeasurementCounter;
      Channel_P2.State.b.Grp5 = 1;
    }
#endif

#ifdef PORT_A
    if ((Channel_P2.State.b.Grp2 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_2) == SAMP_CAP_IO_MASK_2))
    {
      Channel_P2.Measure[1] = MeasurementCounter;
      Channel_P2.State.b.Grp2 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P2.State.b.Grp3 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_3) == SAMP_CAP_IO_MASK_3))
    {
      Channel_P2.Measure[2] = MeasurementCounter;
      Channel_P2.State.b.Grp3 = 1;
    }

    if ((Channel_P2.State.b.Grp6 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_6) == SAMP_CAP_IO_MASK_6))
    {
      Channel_P2.Measure[5] = MeasurementCounter;
      Channel_P2.State.b.Grp6 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P2.State.b.Grp9 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_9) == SAMP_CAP_IO_MASK_9))
    {
      Channel_P2.Measure[8] = MeasurementCounter;
      Channel_P2.State.b.Grp9 = 1;
    }
#endif

#elif (SAMP_CAP_CH != CH4)

#ifdef PORT_A
    if ((Channel_P2.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P2.Measure[3] = MeasurementCounter;
      Channel_P2.State.b.Grp4 = 1;
    }

    if ((Channel_P2.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P2.Measure[4] = MeasurementCounter;
      Channel_P2.State.b.Grp5 = 1;
    }
#endif

#endif

    /* Configure All IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~P2_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~P2_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~P2_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    MeasurementCounter++;

    /* Charging the Csense cap with connecting it to Ctouch by closing the analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(P2_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint16_t)(P2_COMP2_MASK);
#endif

    /* Wait a while for a good charge transfering (programmable delay) */
    __TSL_wait_CLWLTA();

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
    if (Previous_calibration_value >= (uint8_t)(HW_SPREAD_FREQ_MAX))
    {
      Previous_calibration_value = HW_SPREAD_FREQ_MIN;
      RCC->ICSCR &= (uint32_t)(~0x00001F00);
      RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
    }
    else
    {
      RCC->ICSCR += 0x100;
      Previous_calibration_value++;
    }
#endif

    /* Sampling capacitor IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Open the analog switches to allow checking of the inputs */
#ifdef COMP1
    RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P2_COMP1_MASK));
#endif
#ifdef COMP2
    RI->ASCR2 &= (uint16_t)(~(SAMP_CAP_COMP2_MASK | P2_COMP2_MASK));
#endif


    /* it's better to implement this like that because it's much more faster than to put this test in the "while test" below */
    if (MeasurementCounter > SCKEY_MAX_ACQUISITION)
    {
      __NOP();
      break;
    }
#if SPREAD_SPECTRUM && SW_SPREAD_SPECTRUM
    TSL_SW_Spread_Spectrum();
#endif

  }
  while (((Channel_P2.State.whole & Channel_P2.EnabledChannels) != Channel_P2.EnabledChannels));

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Restore the user application calibration value */
  RCC->ICSCR  = HSI_fact_Calib;
#endif
}
#endif
//# NUMBER_OF_ACQUISITION_PORTS > 1

#if NUMBER_OF_ACQUISITION_PORTS > 2

/**
  ******************************************************************************
  * @brief Acquisition function for the 3rd Channel IO of each group
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_IO_Acquisition_P3(void)
{
  uint32_t MeasurementCounter;

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  uint32_t HSI_fact_Calib;
#endif

  /* Reset the counter values */
  MeasurementCounter = 0;
  Channel_P3.Measure[0] = 0;
  Channel_P3.Measure[1] = 0;
  Channel_P3.Measure[2] = 0;
  Channel_P3.Measure[3] = 0;
  Channel_P3.Measure[4] = 0;
  Channel_P3.Measure[5] = 0;
  Channel_P3.Measure[6] = 0;
  Channel_P3.Measure[7] = 0;
  Channel_P3.Measure[8] = 0;
  Channel_P3.Measure[9] = 0;
  Channel_P3.State.whole = 0;

  /* Open the analog switches */
#ifdef COMP1
  RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P3_COMP1_MASK));
#endif
#ifdef COMP2
  RI->ASCR2 &= (uint16_t)(~(SAMP_CAP_COMP2_MASK | P3_COMP2_MASK));
#endif

  /* All IO to pushpull LOW for discharging all capacitors (Ctouch and Csense) */

  /* Discharging sampling capacitor */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(SAMP_CAP_MODER_MASK_A)) | SAMP_CAP_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~SAMP_CAP_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(SAMP_CAP_MODER_MASK_B)) | SAMP_CAP_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~ SAMP_CAP_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(SAMP_CAP_MODER_MASK_C)) | SAMP_CAP_MODER_MASK_C_OUT);
#endif

  /* Discharging Ctouch */
#ifdef PORT_A
  GPIOA->ODR &= (uint16_t)(~P3_IO_MASK_A);
  GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P3_MODER_MASK_A)) | P3_MODER_MASK_A_OUT);
#endif
#ifdef PORT_B
  GPIOB->ODR &= (uint16_t)(~P3_IO_MASK_B);
  GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P3_MODER_MASK_B)) | P3_MODER_MASK_B_OUT);
#endif
#ifdef PORT_C
  GPIOC->ODR &= (uint16_t)(~P3_IO_MASK_C);
  GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P3_MODER_MASK_C)) | P3_MODER_MASK_C_OUT);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Wait a while for a good discharging of all capacitors */
  wait(50); // ~14탎 with fHCLK = 32MHz

  /* All IO in input floating */

#if (PROTECT_IO_ACCESS > 0)
  disableInterrupts();
#endif

#ifdef PORT_A
  GPIOA->MODER &= (uint32_t)(~P3_MODER_MASK_A);
#endif
#ifdef PORT_B
  GPIOB->MODER &= (uint32_t)(~P3_MODER_MASK_B);
#endif
#ifdef PORT_C
  GPIOC->MODER &= (uint32_t)(~P3_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
  enableInterrupts();
#endif

  /* Start HW spread spectrum */
#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Save the user application calibration value */
  HSI_fact_Calib = RCC->ICSCR;
  /* Reset trimmer */
  RCC->ICSCR &= (uint32_t)(~(uint32_t)(0x1F << 8));
  /* Reset Previous_calibration_value */
  Previous_calibration_value = HW_SPREAD_FREQ_MIN;
  /* Init Trimmer */
  RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
#endif

  /* Loop while all the 1st channel of each group have not reach the VIH level */
  do
  {

    /* Close the sampling capacitor analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(SAMP_CAP_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint32_t)(SAMP_CAP_COMP2_MASK);
#endif

    /* Charging Ctouch with connecting the IO to Vdd (io in push-pull HIGH) */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->ODR |= (uint16_t)(P3_IO_MASK_A);  /* HIGH level */
    GPIOA->MODER = (uint32_t)((GPIOA->MODER & ~(P3_MODER_MASK_A)) | P3_MODER_MASK_A_OUT);/* Output push pull config */
#endif
#ifdef PORT_B
    GPIOB->ODR |= (uint16_t)(P3_IO_MASK_B); /* HIGH level */
    GPIOB->MODER = (uint32_t)((GPIOB->MODER & ~(P3_MODER_MASK_B)) | P3_MODER_MASK_B_OUT);/* Output push pull config */
#endif
#ifdef PORT_C
    GPIOC->ODR |= (uint16_t)(P3_IO_MASK_C); /* HIGH level */
    GPIOC->MODER = (uint32_t)((GPIOC->MODER & ~(P3_MODER_MASK_C)) | P3_MODER_MASK_C_OUT);/* Output push pull config */
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Wait a while for a good charging (programmable delay) */
    __TSL_wait_CLWHTA();

#ifdef PORT_A
    if ((Channel_P3.State.b.Grp1 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_1) == SAMP_CAP_IO_MASK_1))
    {
      Channel_P3.Measure[0] = MeasurementCounter;
      Channel_P3.State.b.Grp1 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P3.State.b.Grp7 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_7) == SAMP_CAP_IO_MASK_7))
    {
      Channel_P3.Measure[6] = MeasurementCounter;
      Channel_P3.State.b.Grp7 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P3.State.b.Grp8 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_8) == SAMP_CAP_IO_MASK_8))
    {
      Channel_P3.Measure[7] = MeasurementCounter;
      Channel_P3.State.b.Grp8 = 1;
    }

    if ((Channel_P3.State.b.Grp10 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_10) == SAMP_CAP_IO_MASK_10))
    {
      Channel_P3.Measure[9] = MeasurementCounter;
      Channel_P3.State.b.Grp10 = 1;
    }
#endif

#if (SAMP_CAP_CH != CH4) && (SAMP_CAP_CH != CH3)

#ifdef PORT_A
    if ((Channel_P3.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P3.Measure[3] = MeasurementCounter;
      Channel_P3.State.b.Grp4 = 1;
    }

    if ((Channel_P3.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P3.Measure[4] = MeasurementCounter;
      Channel_P3.State.b.Grp5 = 1;
    }
#endif

#ifdef PORT_A
    if ((Channel_P3.State.b.Grp2 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_2) == SAMP_CAP_IO_MASK_2))
    {
      Channel_P3.Measure[1] = MeasurementCounter;
      Channel_P3.State.b.Grp2 = 1;
    }
#endif

#ifdef PORT_B
    if ((Channel_P3.State.b.Grp3 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_3) == SAMP_CAP_IO_MASK_3))
    {
      Channel_P3.Measure[2] = MeasurementCounter;
      Channel_P3.State.b.Grp3 = 1;
    }

    if ((Channel_P3.State.b.Grp6 == 0) && ((GPIOB->IDR & SAMP_CAP_IO_MASK_6) == SAMP_CAP_IO_MASK_6))
    {
      Channel_P3.Measure[5] = MeasurementCounter;
      Channel_P3.State.b.Grp6 = 1;
    }
#endif

#ifdef PORT_C
    if ((Channel_P3.State.b.Grp9 == 0) && ((GPIOC->IDR & SAMP_CAP_IO_MASK_9) == SAMP_CAP_IO_MASK_9))
    {
      Channel_P3.Measure[8] = MeasurementCounter;
      Channel_P3.State.b.Grp9 = 1;
    }
#endif

#elif (SAMP_CAP_CH != CH4)

#ifdef PORT_A
    if ((Channel_P3.State.b.Grp4 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_4) == SAMP_CAP_IO_MASK_4))
    {
      Channel_P3.Measure[3] = MeasurementCounter;
      Channel_P3.State.b.Grp4 = 1;
    }

    if ((Channel_P3.State.b.Grp5 == 0) && ((GPIOA->IDR & SAMP_CAP_IO_MASK_5) == SAMP_CAP_IO_MASK_5))
    {
      Channel_P3.Measure[4] = MeasurementCounter;
      Channel_P3.State.b.Grp5 = 1;
    }
#endif
#endif

    /* Configure All IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~P3_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~P3_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~P3_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    MeasurementCounter++;

    /* Charging the Csense cap with connecting it to Ctouch by closing the analog switch */
#ifdef COMP1
    RI->ASCR1 |= (uint32_t)(P3_COMP1_MASK);
#endif
#ifdef COMP2
    RI->ASCR2 |= (uint16_t)(P3_COMP2_MASK);
#endif

    /* Wait a while for a good charge transfering (programmable delay) */
    __TSL_wait_CLWLTA();

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
    if (Previous_calibration_value >= (uint8_t)(HW_SPREAD_FREQ_MAX))
    {
      Previous_calibration_value = HW_SPREAD_FREQ_MIN;
      RCC->ICSCR &= (uint32_t)(~0x00001F00);
      RCC->ICSCR |= (uint32_t)(Previous_calibration_value << 8);
    }
    else
    {
      RCC->ICSCR += 0x100;
      Previous_calibration_value++;
    }
#endif

    /* Sampling capacitor IOs in input floating */

#if (PROTECT_IO_ACCESS > 0)
    disableInterrupts();
#endif

#ifdef PORT_A
    GPIOA->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_A);
#endif
#ifdef PORT_B
    GPIOB->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_B);
#endif
#ifdef PORT_C
    GPIOC->MODER &= (uint32_t)(~SAMP_CAP_MODER_MASK_C);
#endif

#if (PROTECT_IO_ACCESS > 0)
    enableInterrupts();
#endif

    /* Open the analog switches to allow checking of the inputs */
#ifdef COMP1
    RI->ASCR1 &= (uint32_t)(~(SAMP_CAP_COMP1_MASK | P3_COMP1_MASK));
#endif
#ifdef COMP2
    RI->ASCR2 &= (uint16_t)(~(SAMP_CAP_COMP2_MASK | P3_COMP2_MASK));
#endif

    /* it's better to implement this like that because it's much more faster than to put this test in the "while test" below */
    if (MeasurementCounter > SCKEY_MAX_ACQUISITION)
    {
      __NOP();
      break;
    }
#if SPREAD_SPECTRUM && SW_SPREAD_SPECTRUM
    TSL_SW_Spread_Spectrum();
#endif

  }
  while (((Channel_P3.State.whole & Channel_P3.EnabledChannels) != Channel_P3.EnabledChannels));

#if SPREAD_SPECTRUM && !SW_SPREAD_SPECTRUM
  /* Restore the user application calibration value */
  RCC->ICSCR  = HSI_fact_Calib;
#endif
}

#endif
//# NUMBER_OF_ACQUISITION_PORTS > 2

/**
  ******************************************************************************
  * @brief Basic delay routine
  * @param[in] wait_delay Delay to be applied.
  * With fHCLK = 32MHz: 1 = ~1탎, 50 = ~14탎, 100 = ~25탎, 200 = ~50탎
  * @retval None
  ******************************************************************************
  */
#ifdef __IAR_SYSTEMS_ICC__
#pragma optimize=medium
#elif defined (__CC_ARM)
#pragma O1
#pragma Ospace
#endif
void wait(uint16_t wait_delay)
{
  uint16_t i = wait_delay;
  for (i = wait_delay; i > 0; i--)
  {}
}

#endif
//# defined(STM32L15XX8B)

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
