/**
  ******************************************************************************
  * @file    stm32_tsl_singlechannelkey.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file contains all functions
  *          prototype and macros for the single channel key management.
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
#ifndef __TSL_SINGLEHANNELKEY_H
#define __TSL_SINGLECHANNELKEY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_tsl_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void TSL_SCKey_Init(void);
void TSL_SCKEY_P1_Acquisition(void);
void TSL_SCKEY_P2_Acquisition(void);
void TSL_SCKEY_P3_Acquisition(void);
void TSL_SCKey_Process(void);
void TSL_SCKey_IdleTreatment(void);
void TSL_SCKey_PreDetectTreatment(void);
void TSL_SCKey_DetectedTreatment(void);
void TSL_SCKey_PostDetectTreatment(void);
void TSL_SCKey_PreRecalibrationTreatment(void);
void TSL_SCKey_CalibrationTreatment(void);
void TSL_SCKey_PreErrorTreatment(void);
void TSL_SCKey_CheckDisabled(void);
void TSL_SCKey_CheckEnabled(void);
uint8_t TSL_SCKey_CheckErrorCondition(void);

#endif /* __TSL_SINGLECHANNELKEY_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
