/**
  ******************************************************************************
  * @file    stm32_tsl_services.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file contains all internal
  *          functions prototype and macros.
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
#ifndef __TSL_SERVICES_H
#define __TSL_SERVICES_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_tsl_conf.h"
#include "stm32_tsl_api.h"
#include "stm32_tsl_internal.h"

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
extern const uint8_t MCKEY1_LOOK_TABLE[CHANNEL_PER_MCKEY][2];
extern const uint16_t MCKEY1_DELTA_COEFF[];
#endif

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
extern const uint8_t MCKEY2_LOOK_TABLE[CHANNEL_PER_MCKEY][2];
extern const uint16_t MCKEY2_DELTA_COEFF[];
#endif

/* Exported functions --------------------------------------------------------*/
void TSL_SetStructPointer(void);
void TSL_DeltaCalculation(void);
void TSL_SCKey_SetIdleState(void);
void TSL_SCKey_BackToIdleState(void);
void TSL_SCKey_SetPreDetectState(void);
void TSL_SCKey_SetDetectedState(void);
void TSL_SCKey_SetPostDetectState(void);
void TSL_SCKey_BackToDetectedState(void);
void TSL_SCKey_SetPreRecalibrationState(void);
void TSL_SCKey_SetCalibrationState(void);
void TSL_SCKey_SetErrorState(void);
void TSL_SCKey_SetDisabledState(void);
void TSL_ECS(void);
void TSL_SCKey_DxS(void);
void TSL_SCKey_DetectionTimeout(void);
uint16_t TSL_MCKey_InitAcq(uint8_t channel);
void TSL_MCKey_SetStructPointer(void);
void TSL_MCKey_DeltaCalculation(uint8_t ChIdx);
void TSL_MCKey_SetIdleState(void);
void TSL_MCKey_BackToIdleState(void);
void TSL_MCKey_SetPreDetectState(void);
void TSL_MCKey_SetDetectedState(void);
void TSL_MCKey_SetPostDetectState(void);
void TSL_MCKey_BackToDetectedState(void);
void TSL_MCKey_SetPreRecalibrationState(void);
void TSL_MCKey_SetCalibrationState(void);
void TSL_MCKey_SetErrorState(void);
void TSL_MCKey_SetDisabledState(void);
void TSL_MCKey_DxS(void);
void TSL_MCKey_DetectionTimeout(void);

#endif /* __TSL_SERVICES_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
