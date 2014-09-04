/**
  ******************************************************************************
  * @file    stm32_tsl_singlechannelkey.c
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief  STM32 Touch Sensing Library - This file provides the functions to
  *          manage the single channel Key.
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
#include "stm32_tsl_singlechannelkey.h"
#include "stm32_tsl_api.h"
#include "stm32_tsl_services.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if NUMBER_OF_ACQUISITION_PORTS > 0

/**
  ******************************************************************************
  * @brief Select Port 1 I/Os to burst and call IO driver for burst sequence.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKEY_P1_Acquisition(void)
{

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  uint16_t mask;
#endif

  /* The acquisition in CHARGE TRANSFERT is performed for the 10 groups in one time */

  /* Build the mask for the single key acquisition */
  Channel_P1.EnabledChannels = 0;

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
  for (KeyIndex = 0; KeyIndex < SCKEY_P1_KEY_COUNT; KeyIndex++)
  {
    if ((sSCKeyInfo[KeyIndex].State.whole & (DISABLED_STATE | ERROR_STATE)) == 0)
    {
      Channel_P1.EnabledChannels |= Table_SCKEY_BITS[KeyIndex];
    }
  }
#endif

  /* Ored the mask for the multi channel key */
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  if ((mask = TSL_MCKey_InitAcq(SCKEY_P1_CH)) != 0)
  {
    Channel_P1.EnabledChannels |= mask;
  }
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0

  // The acquisition is only performed if at least one channel is enabled
  if (Channel_P1.EnabledChannels != 0)
  {
    /* Launch the aquisition for the "port1" */
    TSL_IO_Acquisition_P1();
    /* Fill the single key structures */
#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
    for (KeyIndex = 0; KeyIndex < SCKEY_P1_KEY_COUNT; KeyIndex++)
    {
      sSCKeyInfo[KeyIndex].Channel.LastMeas = Channel_P1.Measure[Table_SCKEY_P1[KeyIndex]];
    }
#endif
  }

}

#if NUMBER_OF_ACQUISITION_PORTS > 1
/**
  ******************************************************************************
  * @brief Select Port 2 I/Os to burst and call IO driver for burst sequence.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKEY_P2_Acquisition(void)
{

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  uint16_t mask;
#endif

  /* The acquisition in CHARGE TRANSFERT is performed for the 8 groups in one time */

  /* Build the mask for the single key acquisition */
  Channel_P2.EnabledChannels = 0;

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 1
  for (KeyIndex = SCKEY_P1_KEY_COUNT; KeyIndex < (SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex++)
  {
    if ((sSCKeyInfo[KeyIndex].State.whole & (DISABLED_STATE | ERROR_STATE)) == 0)
    {
      Channel_P2.EnabledChannels |= Table_SCKEY_BITS[KeyIndex];
    }
  }
#endif

  /* Ored the mask for the multi channel key */
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  if ((mask = TSL_MCKey_InitAcq(SCKEY_P2_CH)) != 0)
  {
    Channel_P2.EnabledChannels |= mask;
  }
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0

  // The acquisition is only performed if at least one channel is enabled
  if (Channel_P2.EnabledChannels != 0)
  {
    /* Launch the aquisition */
    TSL_IO_Acquisition_P2();
    /* Fill the single key structures */
#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 1
    for (KeyIndex = SCKEY_P1_KEY_COUNT; KeyIndex < (SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex++)
    {
      sSCKeyInfo[KeyIndex].Channel.LastMeas = Channel_P2.Measure[Table_SCKEY_P2[KeyIndex - SCKEY_P1_KEY_COUNT]];
    }
#endif
  }

}
#endif
//  NUMBER_OF_ACQUISITION_PORTS > 1

#if NUMBER_OF_ACQUISITION_PORTS > 2
/**
  ******************************************************************************
  * @brief Select Port 3 I/Os to burst and call IO driver for burst sequence.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKEY_P3_Acquisition(void)
{

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  uint16_t mask;
#endif

  /* The acquisition in CHARGE TRANSFERT is performed for the 8 groups in one time */

  /* Build the mask for the single key acquisition */
  Channel_P3.EnabledChannels = 0;

  for (KeyIndex = (SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex < (SCKEY_P3_KEY_COUNT + SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex++)
  {
    if ((sSCKeyInfo[KeyIndex].State.whole & (DISABLED_STATE | ERROR_STATE)) == 0)
    {
      Channel_P3.EnabledChannels |= Table_SCKEY_BITS[KeyIndex];
    }
  }

  /* Ored the mask for the multi channel key */
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  if ((mask = TSL_MCKey_InitAcq(SCKEY_P3_CH)) != 0)
  {
    Channel_P3.EnabledChannels |= mask;
  }
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0

  // The acquisition is only performed if at least one channel is enabled
  if (Channel_P3.EnabledChannels != 0)
  {
    /* Launch the aquisition */
    TSL_IO_Acquisition_P3();
    /* Fill the single key structures */
    for (KeyIndex = (SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex < (SCKEY_P3_KEY_COUNT + SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex++)
    {
      sSCKeyInfo[KeyIndex].Channel.LastMeas = Channel_P3.Measure[Table_SCKEY_P3[KeyIndex - (SCKEY_P1_KEY_COUNT + SCKEY_P2_KEY_COUNT)]];
    }
  }

}
#endif
//  NUMBER_OF_ACQUISITION_PORTS > 2

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0

/**
  ******************************************************************************
  * @brief Initialize all SCKey relative parameters and variables.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_Init(void)
{

  for (KeyIndex = 0; KeyIndex < NUMBER_OF_SINGLE_CHANNEL_KEYS; KeyIndex++)
  {
    TSL_SetStructPointer();
    pKeyStruct->State.whole = DISABLED_STATE;
    pKeyStruct->DetectThreshold = SCKEY_DETECTTHRESHOLD_DEFAULT;
    pKeyStruct->EndDetectThreshold = SCKEY_ENDDETECTTHRESHOLD_DEFAULT;
    pKeyStruct->RecalibrationThreshold = SCKEY_RECALIBRATIONTHRESHOLD_DEFAULT;
  }
}


/**
  ******************************************************************************
  * @brief After Touch Sensing acquisition, this function launch the data interpretation.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_Process(void)
{

  TSL_SetStructPointer();

  TSL_DeltaCalculation();

  switch (pKeyStruct->State.whole)
  {

    case IDLE_STATE:
      if (TSL_SCKey_CheckErrorCondition())
      {
        TSL_SCKey_SetErrorState();
        break;
      }
      TSL_SCKey_IdleTreatment();
      TSL_SCKey_CheckDisabled();
      break;

    case PRE_DETECTED_STATE:
      TSL_SCKey_PreDetectTreatment();
      break;

    case DETECTED_STATE:
      if (TSL_SCKey_CheckErrorCondition())
      {
        TSL_SCKey_SetErrorState();
        break;
      }
      TSL_SCKey_DetectedTreatment();
      TSL_SCKey_CheckDisabled();
      break;

    case POST_DETECTED_STATE:
      TSL_SCKey_PostDetectTreatment();
      break;

    case PRE_CALIBRATION_STATE:
      TSL_SCKey_PreRecalibrationTreatment();
      break;

    case CALIBRATION_STATE:
      if (TSL_SCKey_CheckErrorCondition())
      {
        TSL_SCKey_SetErrorState();
        break;
      }
      TSL_SCKey_CalibrationTreatment();
      TSL_SCKey_CheckDisabled();
      break;

    case ERROR_STATE:
      TSL_SCKey_CheckDisabled();
      break;

    case DISABLED_STATE:
      TSL_SCKey_CheckEnabled();
      break;

    default:
      for (;;)
      {
        // Infinite loop.
      }

  }

  TSL_TempGlobalSetting.whole |= pKeyStruct->Setting.whole;
  TSL_TempGlobalState.whole |= pKeyStruct->State.whole;
  pKeyStruct->Setting.b.CHANGED = 0;

}


/**
  ******************************************************************************
  * @brief Check SCKey info during Idle state: Verify detection and recalibration.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_IdleTreatment(void)
{
#if NEGDETECT_AUTOCAL == 1
  if (Delta <= pKeyStruct->RecalibrationThreshold)
  {
    TSL_SCKey_SetPreRecalibrationState();
    return;
  }
#endif

#if NEGDETECT_AUTOCAL == 1
  if (Delta >= pKeyStruct->DetectThreshold)
#else
  if ((Delta >= pKeyStruct->DetectThreshold) || (Delta <= pKeyStruct->RecalibrationThreshold))
#endif
  {
    TSL_SCKey_SetPreDetectState();
    if (!DetectionIntegrator)
    {
      pKeyStruct->Channel.IntegratorCounter++;
      TSL_SCKey_PreDetectTreatment();
    }
  }
}


/**
  ******************************************************************************
  * @brief Check SCKey info during PRE DETECT state: Verify detection integrator and detection exclusion.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_PreDetectTreatment(void)
{
#if NEGDETECT_AUTOCAL == 1
  if (Delta >= pKeyStruct->DetectThreshold)
#else
  if ((Delta >= pKeyStruct->DetectThreshold) || (Delta <= pKeyStruct->RecalibrationThreshold))
#endif
  {
    TSL_SCKey_DxS();
    pKeyStruct->Channel.IntegratorCounter--;
    if (!pKeyStruct->Channel.IntegratorCounter)
    {
      TSL_SCKey_SetDetectedState();
    }
  }
  else
  {
    TSL_SCKey_BackToIdleState();
    return;
  }
}


/**
  ******************************************************************************
  * @brief Check SCKey info during DETECTED state: Verify detection timeout, end of detection and detection exclusion.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_DetectedTreatment(void)
{
#if NEGDETECT_AUTOCAL == 1
  if (Delta <= pKeyStruct->EndDetectThreshold)
#else
  if (((Delta <= pKeyStruct->EndDetectThreshold) && (Delta > 0)) ||
      ((Delta >= pKeyStruct->RecalibrationThreshold) && (Delta < 0)))
#endif
  {
    TSL_SCKey_SetPostDetectState();
    if (!EndDetectionIntegrator)
    {
      pKeyStruct->Channel.IntegratorCounter++;
      TSL_SCKey_PostDetectTreatment();
    }
    return;
  }

  TSL_SCKey_DetectionTimeout();

}


/**
  ******************************************************************************
  * @brief Check SCKey info during POST DETECT state: Verify end of detection.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_PostDetectTreatment(void)
{
#if NEGDETECT_AUTOCAL == 1
  if (Delta <= pKeyStruct->EndDetectThreshold)
#else
  if (((Delta <= pKeyStruct->EndDetectThreshold) && (Delta > 0)) ||
      ((Delta >= pKeyStruct->RecalibrationThreshold) && (Delta < 0)))
#endif
  {
    pKeyStruct->Channel.IntegratorCounter--;
    if (!pKeyStruct->Channel.IntegratorCounter)
    {
      TSL_SCKey_SetIdleState();
    }
  }
  else
  {
    // No reset of DTO counter.
    TSL_SCKey_BackToDetectedState();
  }
}


/**
  ******************************************************************************
  * @brief Check SCKey info during PRE RECALIBRATION state: Verify condition for recalibration.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_PreRecalibrationTreatment(void)
{
  if (Delta <= pKeyStruct->RecalibrationThreshold)
  {
    pKeyStruct->Channel.IntegratorCounter--;
    if (!pKeyStruct->Channel.IntegratorCounter)
    {
      TSL_SCKey_SetCalibrationState();
    }
  }
  else
  {
    TSL_SCKey_BackToIdleState();
  }
}


/**
  ******************************************************************************
  * @brief During calibration, calculates the new reference.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_CalibrationTreatment(void)
{
  pKeyStruct->Channel.Reference += pKeyStruct->Channel.LastMeas;
  pKeyStruct->Counter--;
  if (!pKeyStruct->Counter)
  {
    // Warning: Must be divided by SCKEY_CALIBRATION_COUNT_DEFAULT !!!
    pKeyStruct->Channel.Reference = (pKeyStruct->Channel.Reference >> 3);
    TSL_SCKey_SetIdleState();
  }
}


/**
  ******************************************************************************
  * @brief Takes into account the customer code settings in memory to disable the key.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_CheckDisabled(void)
{
  if (!pKeyStruct->Setting.b.ENABLED)
  {
    TSL_SCKey_SetDisabledState();
  }
}


/**
  ******************************************************************************
  * @brief Takes into account the customer code settings in memory to enable the key.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_SCKey_CheckEnabled(void)
{
  if (pKeyStruct->Setting.b.ENABLED && pKeyStruct->Setting.b.IMPLEMENTED)
  {
    TSL_SCKey_SetCalibrationState();
  }
}


/**
  ******************************************************************************
  * @brief Verification of the last acquisition result to be within the authorized range.
  * @param None
  * @retval uint8_t Error status
  * @retval 0x00 Last acquisition is OK
  * @retval 0xFF Burst count out of range
  ******************************************************************************
  */
uint8_t TSL_SCKey_CheckErrorCondition(void)
{
  if ((pKeyStruct->Channel.LastMeas < SCKEY_MIN_ACQUISITION)
      || (pKeyStruct->Channel.LastMeas > SCKEY_MAX_ACQUISITION))
  {
    return 0xFF;  // Error case !
  }

  return 0; // OK

}
#endif
//  NUMBER_OF_SINGLE_CHANNEL_KEYS > 0

#endif
//  NUMBER_OF_ACQUISITION_PORTS > 0

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
