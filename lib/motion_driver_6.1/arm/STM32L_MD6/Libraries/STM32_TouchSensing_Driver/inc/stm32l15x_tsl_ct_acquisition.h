/**
  ******************************************************************************
  * @file    stm32l15x_tsl_ct_acquisition.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file contains all functions
  *          prototype and macros handling the Charge-Transfer acquisition for
  *          STM32L15x devices.
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
#ifndef __STM32L15X_TSL_CT_ACQUISITION_H
#define __STM32L15X_TSL_CT_ACQUISITION_H


/* Includes ------------------------------------------------------------------*/
#include "stm32_tsl_conf.h"

#if defined(STM32L15XX8B)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*                   STRUCTURE DEFINITIONS                                    */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef enum
{
  SCKEY_TYPE = 0,
  MCKEY_TYPE = 1
}
IOTYPE_T;

/* Contains all informations of the touched I/O */
typedef struct
{
  GPIO_TypeDef *PORT_ADDR;  /* Base adress of the GPIO (DR address) */
  uint8_t AcqMask;           /* Mask used to acquire the pin value */
  uint16_t *Measurement;     /* Contains the last measurement value */
  IOTYPE_T Type;             /* Contains the type of the key touched */
}
TOUCH_PORT_Info_T;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*                       CONSTANTS DEFINITIONS                              */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*                       MACRO DEFINITIONS                              */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define enableInterrupts()   __set_PRIMASK(0);
#define disableInterrupts()  __set_PRIMASK(1);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*                           GLOBALS DEFINITIONS                              */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern TOUCH_PORT_Info_T sTouchIO;

/* Warning: must be the same definition as in the stm32_tsl_conf.h file */
#define GROUP1  (0x0001)
#define GROUP2  (0x0002)
#define GROUP3  (0x0004)
#define GROUP4  (0x0008)
#define GROUP5  (0x0010)
#define GROUP6  (0x0020)
#define GROUP7  (0x0040)
#define GROUP8  (0x0080)
#define GROUP9  (0x0100)
#define GROUP10 (0x0200)

typedef union
{
  uint32_t whole;
  struct
  {
  unsigned  Grp1  : 1;
  unsigned  Grp2  : 1;
  unsigned  Grp3  : 1;
  unsigned  Grp4  : 1;
  unsigned  Grp5  : 1;
  unsigned  Grp6  : 1;
  unsigned  Grp7  : 1;
  unsigned  Grp8  : 1;
  unsigned  Grp9  : 1;
  unsigned  Grp10 : 1;
  }
  b;
}
Channel_State;

typedef struct
{
  uint16_t Measure[10];
  Channel_State State;
  uint16_t EnabledChannels;
}
Info_Channel;


extern Info_Channel Channel_P1;
#if (NUMBER_OF_SINGLE_CHANNEL_PORTS > 0) && (SCKEY_P1_KEY_COUNT > 0)
extern const uint8_t Table_SCKEY_P1[SCKEY_P1_KEY_COUNT];
#endif

#if NUMBER_OF_ACQUISITION_PORTS > 1
extern Info_Channel Channel_P2;
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 1
extern const uint8_t Table_SCKEY_P2[SCKEY_P2_KEY_COUNT];
#endif
#endif

#if NUMBER_OF_ACQUISITION_PORTS > 2
extern Info_Channel Channel_P3;
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 2
extern const uint8_t Table_SCKEY_P3[SCKEY_P3_KEY_COUNT];
#endif
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----             __CONSTANT TABLE FOR I/O ADDRESSING                    -----
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern const uint16_t Table_SCKEY_BITS[];
extern const uint16_t Table_MCKEY_BITS[];
extern const uint16_t Table_MCKEY_PORTS[];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----             FUNCTION DEFINITION                                  -----
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void TSL_IO_Init(void);
void TSL_IO_Clamp(void);
void TSL_IO_Acquisition_P1(void);
void TSL_IO_Acquisition_P2(void);
void TSL_IO_Acquisition_P3(void);
void wait(uint16_t wait_delay);

#endif /* if defined(STM32L15XX8B) */

#endif /* __STM32L15X_TSL_CT_ACQUISITION_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
