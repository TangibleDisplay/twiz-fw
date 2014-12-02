/**
  ******************************************************************************
  * @file    stm32_tsl_multichannelkey.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file contains all functions
  *          prototype and macros for the multi channel key management.
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
#ifndef __TSL_MULTICHANNELKEY_H
#define __TSL_MULTICHANNELKEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_tsl_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void TSL_MCKey_Init(void);
void TSL_MCKey1_Acquisition(void);
void TSL_MCKey2_Acquisition(void);
void TSL_MCKey_Process(void);
void TSL_MCKey_IdleTreatment(void);
void TSL_MCKey_PreDetectTreatment(void);
void TSL_MCKey_DetectedTreatment(void);
void TSL_MCKey_PostDetectTreatment(void);
void TSL_MCKey_PreRecalibrationTreatment(void);
void TSL_MCKey_CalibrationTreatment(void);
void TSL_MCKey_PreErrorTreatment(void);
uint8_t TSL_MCKey_Position(void);
void TSL_MCKey_CheckDisabled(void);
void TSL_MCKey_CheckEnabled(void);
uint8_t TSL_MCKey_CheckErrorCondition(void);

#endif /* __TSL_MULTICHANNELKEY_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
