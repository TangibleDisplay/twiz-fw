/**
  ******************************************************************************
  * @file    stm32_tsl_internal.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file contains all functions
  *          prototype and macros internal to the touch sensing library.
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
#ifndef __TSL_INTERNAL_H
#define __TSL_INTERNAL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l15x_tsl_ct_acquisition.h"
#include "stm32_tsl_api.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern uint8_t KeyIndex;
extern uint8_t ChannelIndex;
extern int16_t Delta;
extern int16_t Delta1;
extern int16_t Delta2;
extern int16_t Delta3;
extern KeyFlag_T TSL_TempGlobalSetting;
extern KeyState_T TSL_TempGlobalState;
extern uint32_t Local_TickECS10ms;
extern TimerFlag_T Local_TickFlag;
extern uint8_t ECSTimeStepCounter;
extern uint8_t ECSTempoCounter;
extern uint8_t ECSTempoPrescaler;

/* Exported functions --------------------------------------------------------*/

#endif /* __TSL_INTERNAL_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
