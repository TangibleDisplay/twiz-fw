/**
  ******************************************************************************
  * @file    stm32_tsl_multichannelkey.c
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file provides the functions to
  *          manage the multi-channel keys (wheel and slider).
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
#include "stm32_tsl_multichannelkey.h"
#include "stm32_tsl_api.h"
#include "stm32_tsl_services.h"

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

const Info_Channel * Channel_address_tab[] =
  {
    &Channel_P1
#if NUMBER_OF_ACQUISITION_PORTS > 1
    , &Channel_P2
#endif
#if NUMBER_OF_ACQUISITION_PORTS > 2
    , &Channel_P3
#endif
  };

//==============================================================================
// MCKEY1
//==============================================================================

//------------------------------------------------------------------------------
// MCKEY1 LOOK-UP TABLE FOR CHANNELS AND GROUPS CORRESPONDANCE
//------------------------------------------------------------------------------
const uint8_t MCKEY1_LOOK_TABLE[CHANNEL_PER_MCKEY][2] =
  {
    /********MCKEY1_A************/

#if MCKEY1_A_CH == SCKEY_P1_CH
    0x00
#elif MCKEY1_A_CH == SCKEY_P2_CH
    0x01
#else //MCKEY1_A_CH == SCKEY_P3_CH
    0x02
#endif

#if   (MCKEY1_A == GROUP1)
    , 0x00
#elif (MCKEY1_A == GROUP2)
    , 0x01
#elif (MCKEY1_A == GROUP3)
    , 0x02
#elif (MCKEY1_A == GROUP4)
    , 0x03
#elif (MCKEY1_A == GROUP5)
    , 0x04
#elif (MCKEY1_A == GROUP6)
    , 0x05
#elif (MCKEY1_A == GROUP7)
    , 0x06
#elif (MCKEY1_A == GROUP8)
    , 0x07
#elif (MCKEY1_A == GROUP9)
    , 0x08
#elif (MCKEY1_A == GROUP10)
    , 0x09
#endif

    /********MCKEY1_B************/

#if MCKEY1_B_CH == SCKEY_P1_CH
    , 0x00
#elif MCKEY1_B_CH == SCKEY_P2_CH
    , 0x01
#else //MCKEY1_A_CH == SCKEY_P3_CH
    , 0x02
#endif

#if   (MCKEY1_B == GROUP1)
    , 0x00
#elif (MCKEY1_B == GROUP2)
    , 0x01
#elif (MCKEY1_B == GROUP3)
    , 0x02
#elif (MCKEY1_B == GROUP4)
    , 0x03
#elif (MCKEY1_B == GROUP5)
    , 0x04
#elif (MCKEY1_B == GROUP6)
    , 0x05
#elif (MCKEY1_B == GROUP7)
    , 0x06
#elif (MCKEY1_B == GROUP8)
    , 0x07
#elif (MCKEY1_B == GROUP9)
    , 0x08
#elif (MCKEY1_B == GROUP10)
    , 0x09
#endif

    /********MCKEY1_C************/

#if MCKEY1_C_CH == SCKEY_P1_CH
    , 0x00
#elif MCKEY1_C_CH == SCKEY_P2_CH
    , 0x01
#else //MCKEY1_A_CH == SCKEY_P3_CH
    , 0x02
#endif

#if   (MCKEY1_C == GROUP1)
    , 0x00
#elif (MCKEY1_C == GROUP2)
    , 0x01
#elif (MCKEY1_C == GROUP3)
    , 0x02
#elif (MCKEY1_C == GROUP4)
    , 0x03
#elif (MCKEY1_C == GROUP5)
    , 0x04
#elif (MCKEY1_C == GROUP6)
    , 0x05
#elif (MCKEY1_C == GROUP7)
    , 0x06
#elif (MCKEY1_C == GROUP8)
    , 0x07
#elif (MCKEY1_C == GROUP9)
    , 0x08
#elif (MCKEY1_C == GROUP10)
    , 0x09
#endif

  };

//------------------------------------------------------------------------------
// MCKEY1 CONSTANT TABLES FOR POSITION CALCULATION
//------------------------------------------------------------------------------

#if MCKEY1_TYPE == 0 // Wheel
#if MCKEY1_LAYOUT_TYPE == 0 // Interlaced electrodes
const int16_t MCKEY1_TABLE_POSITION_OFFSET[CHANNEL_PER_MCKEY][CHANNEL_PER_MCKEY] =
  {
    // sec = 1     2     3
    //   j = 0     1     2
    0,  -64,  107, // maj = 1; i = 0
    21,    0, -107, // maj = 2; i = 1
    -149,   64,    0  // maj = 3; i = 2
  };
#define MCKEY1_SECTOR_COMPUTATION  (85)
#define MCKEY1_POSITION_CORRECTION (43)
#else // Normal electrodes
#error "MCKey with normal electrodes layout type is not supported"
#endif
#endif // Wheel
//# MCKEY1_TYPE == 0

#if MCKEY1_TYPE == 1 // Slider
#if MCKEY1_LAYOUT_TYPE == 0 // Interlaced electrodes
const int16_t MCKEY1_TABLE_POSITION_OFFSET[CHANNEL_PER_MCKEY][CHANNEL_PER_MCKEY] =
  {
    // sec = 1     2     3
    //   j = 0     1     2
    0,  -96,  160, // maj = 1; i = 0
    32,    0, -160, // maj = 2; i = 1
    -224,   96,    0  // maj = 3; i = 2
  };
#define MCKEY1_SECTOR_COMPUTATION  (128)
#define MCKEY1_POSITION_CORRECTION (128)
#else // Normal electrodes
#error "MCKey with normal electrodes layout type is not supported"
#endif
#endif
//# MCKEY1_TYPE == 1

//==============================================================================
// MCKEY2
//==============================================================================

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1

//------------------------------------------------------------------------------
// MCKEY2 LOOK-UP TABLE FOR CHANNELS AND GROUPS CORRESPONDANCE
//------------------------------------------------------------------------------
const uint8_t MCKEY2_LOOK_TABLE[CHANNEL_PER_MCKEY][2] =
  {

    /********MCKEY2_A************/
#if MCKEY2_A_CH == SCKEY_P1_CH
    0x00
#else //port 2 is used
    0x01
#endif

#if   (MCKEY2_A == GROUP1)
    , 0x00
#elif (MCKEY2_A == GROUP2)
    , 0x01
#elif (MCKEY2_A == GROUP3)
    , 0x02
#elif (MCKEY2_A == GROUP4)
    , 0x03
#elif (MCKEY2_A == GROUP5)
    , 0x04
#elif (MCKEY2_A == GROUP6)
    , 0x05
#elif (MCKEY2_A == GROUP7)
    , 0x06
#elif (MCKEY2_A == GROUP8)
    , 0x07
#elif (MCKEY2_A == GROUP9)
    , 0x08
#elif (MCKEY2_A == GROUP10)
    , 0x09
#endif

    /********MCKEY2_B************/
#if MCKEY2_B_CH == SCKEY_P1_CH
    , 0x00
#else //port 2 is used
    , 0x01
#endif

#if   (MCKEY2_B == GROUP1)
    , 0x00
#elif (MCKEY2_B == GROUP2)
    , 0x01
#elif (MCKEY2_B == GROUP3)
    , 0x02
#elif (MCKEY2_B == GROUP4)
    , 0x03
#elif (MCKEY2_B == GROUP5)
    , 0x04
#elif (MCKEY2_B == GROUP6)
    , 0x05
#elif (MCKEY2_B == GROUP7)
    , 0x06
#elif (MCKEY2_B == GROUP8)
    , 0x07
#elif (MCKEY2_B == GROUP9)
    , 0x08
#elif (MCKEY2_B == GROUP10)
    , 0x09
#endif

    /********MCKEY2_C************/
#if MCKEY2_C_CH == SCKEY_P1_CH
    , 0x00
#else //port 2 is used
    , 0x01
#endif

#if   (MCKEY2_C == GROUP1)
    , 0x00
#elif (MCKEY2_C == GROUP2)
    , 0x01
#elif (MCKEY2_C == GROUP3)
    , 0x02
#elif (MCKEY2_C == GROUP4)
    , 0x03
#elif (MCKEY2_C == GROUP5)
    , 0x04
#elif (MCKEY2_C == GROUP6)
    , 0x05
#elif (MCKEY2_C == GROUP7)
    , 0x06
#elif (MCKEY2_C == GROUP8)
    , 0x07
#elif (MCKEY2_C == GROUP9)
    , 0x08
#elif (MCKEY2_C == GROUP10)
    , 0x09
#endif

  };

//------------------------------------------------------------------------------
// MCKEY2 CONSTANT TABLES FOR POSITION CALCULATION
//------------------------------------------------------------------------------

#if MCKEY2_TYPE == 0 // Wheel
#if MCKEY2_LAYOUT_TYPE == 0 // Interlaced electrodes
const int16_t MCKEY2_TABLE_POSITION_OFFSET[CHANNEL_PER_MCKEY][CHANNEL_PER_MCKEY] =
  {
    // sec = 1     2     3
    //   j = 0     1     2
    0,  -64,  107, // maj = 1; i = 0
    21,    0, -107, // maj = 2; i = 1
    -149,   64,    0  // maj = 3; i = 2
  };
#define MCKEY2_SECTOR_COMPUTATION  (85)
#define MCKEY2_POSITION_CORRECTION (43)
#else // Normal electrodes
#error "MCKey with normal electrodes layout type is not supported"
#endif
#endif // Wheel
//# MCKEY2_TYPE == 0

#if MCKEY2_TYPE == 1 // Slider
#if MCKEY2_LAYOUT_TYPE == 0 // Interlaced electrodes
const int16_t MCKEY2_TABLE_POSITION_OFFSET[CHANNEL_PER_MCKEY][CHANNEL_PER_MCKEY] =
  {
    // sec = 1     2     3
    //   j = 0     1     2
    0,  -96,  160, // maj = 1; i = 0
    32,    0, -160, // maj = 2; i = 1
    -224,   96,    0  // maj = 3; i = 2
  };
#define MCKEY2_SECTOR_COMPUTATION  (128)
#define MCKEY2_POSITION_CORRECTION (128)
#else // Normal electrodes
#error "MCKey with normal electrodes layout type is not supported"
#endif
#endif
//# MCKEY2_TYPE == 1

#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 1
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 1


/**
  ******************************************************************************
  * @brief Initialize all MCKey relative parameters and variables.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_Init(void)
{

  for (KeyIndex = 0; KeyIndex < NUMBER_OF_MULTI_CHANNEL_KEYS; KeyIndex++)
  {
    TSL_MCKey_SetStructPointer();
    pMCKeyStruct->State.whole = DISABLED_STATE;
    pMCKeyStruct->DetectThreshold = MCKEY_DETECTTHRESHOLD_DEFAULT;
    pMCKeyStruct->EndDetectThreshold = MCKEY_ENDDETECTTHRESHOLD_DEFAULT;
    pMCKeyStruct->RecalibrationThreshold = MCKEY_RECALIBRATIONTHRESHOLD_DEFAULT;
    pMCKeyStruct->Resolution = MCKEY_RESOLUTION_DEFAULT;
    pMCKeyStruct->DirectionChangeIntegrator = MCKEY_DIRECTION_CHANGE_INTEGRATOR_DEFAULT;
    pMCKeyStruct->DirectionChangeThreshold = MCKEY_DIRECTION_CHANGE_THRESHOLD_DEFAULT;
  }

  /* Set MC key type: Wheel or Slider */
  sMCKeyInfo[0].Setting.b.MCKEY_TYPE = MCKEY1_TYPE;
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
  sMCKeyInfo[1].Setting.b.MCKEY_TYPE = MCKEY2_TYPE;
#endif

}

/**
  ******************************************************************************
  * @brief Select I/Os to burst and call IO driver for burst sequence.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey1_Acquisition(void)
{

  KeyIndex = 0;
  TSL_MCKey_SetStructPointer();

  if (!((pMCKeyStruct->State.whole == ERROR_STATE) || (pMCKeyStruct->State.whole == DISABLED_STATE)))
  {
    for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
    {
      sTouchIO.PORT_ADDR = (GPIO_TypeDef *)(Table_MCKEY_PORTS[ChannelIndex]);
      sTouchIO.AcqMask = Table_MCKEY_BITS[ChannelIndex];
      sTouchIO.Measurement = &sMCKeyInfo[0].Channel[ChannelIndex].LastMeas;
      sTouchIO.Type = MCKEY_TYPE;
      sMCKeyInfo[0].Channel[ChannelIndex].LastMeas = Channel_address_tab[MCKEY1_LOOK_TABLE[ChannelIndex][0]]->Measure[MCKEY1_LOOK_TABLE[ChannelIndex][1]];
    }
  }

}


#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
/**
  ******************************************************************************
  * @brief Select I/Os to burst and call IO driver for burst sequence.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey2_Acquisition(void)
{

  KeyIndex = 1;
  TSL_MCKey_SetStructPointer();

  if (!((pMCKeyStruct->State.whole == ERROR_STATE) || (pMCKeyStruct->State.whole == DISABLED_STATE)))
  {
    for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
    {
      sTouchIO.PORT_ADDR = (GPIO_TypeDef *)(Table_MCKEY_PORTS[CHANNEL_PER_MCKEY + ChannelIndex]);
      sTouchIO.AcqMask = Table_MCKEY_BITS[CHANNEL_PER_MCKEY + ChannelIndex];
      sTouchIO.Measurement = &sMCKeyInfo[1].Channel[ChannelIndex].LastMeas;
      sTouchIO.Type = MCKEY_TYPE;
      sMCKeyInfo[1].Channel[ChannelIndex].LastMeas = Channel_address_tab[MCKEY2_LOOK_TABLE[ChannelIndex][0]]->Measure[MCKEY2_LOOK_TABLE[ChannelIndex][1]];
    }
  }

}
#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 1


/**
  ******************************************************************************
  * @brief After Touch Sensing acquisition, data processing.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_Process(void)
{

  TSL_MCKey_SetStructPointer();

  switch (pMCKeyStruct->State.whole)
  {
    case IDLE_STATE:
      if (TSL_MCKey_CheckErrorCondition())
      {
        TSL_MCKey_SetErrorState();
        break;
      }
      TSL_MCKey_IdleTreatment();
      TSL_MCKey_CheckDisabled();
      break;

    case PRE_DETECTED_STATE:
      TSL_MCKey_PreDetectTreatment();
      break;

    case DETECTED_STATE:
      if (TSL_MCKey_CheckErrorCondition())
      {
        TSL_MCKey_SetErrorState();
        break;
      }
      TSL_MCKey_DetectedTreatment();
      TSL_MCKey_CheckDisabled();
      break;

    case POST_DETECTED_STATE:
      TSL_MCKey_PostDetectTreatment();
      break;

    case PRE_CALIBRATION_STATE:
      TSL_MCKey_PreRecalibrationTreatment();
      break;

    case CALIBRATION_STATE:
      if (TSL_MCKey_CheckErrorCondition())
      {
        TSL_MCKey_SetErrorState();
        break;
      }
      TSL_MCKey_CalibrationTreatment();
      TSL_MCKey_CheckDisabled();
      break;

    case ERROR_STATE:
      TSL_MCKey_CheckDisabled();
      break;

    case DISABLED_STATE:
      TSL_MCKey_CheckEnabled();
      break;

    default:
      for (;;)
      {
        // Infinite loop.
      }
  }

  TSL_TempGlobalSetting.whole |= pMCKeyStruct->Setting.whole;
  TSL_TempGlobalState.whole |= pMCKeyStruct->State.whole;
  pMCKeyStruct->Setting.b.CHANGED = 0;
  pMCKeyStruct->Setting.b.POSCHANGED = 0;

}


/**
  ******************************************************************************
  * @brief Check for MCKey info during Idle state: Verify detection and recalibration.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_IdleTreatment(void)
{

  /* Check all channels to enter in Calibration state if needed */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {

    TSL_MCKey_DeltaCalculation(ChannelIndex);

#if NEGDETECT_AUTOCAL == 1
    if (Delta <= pMCKeyStruct->RecalibrationThreshold)
    {
      TSL_MCKey_SetPreRecalibrationState();
      return;
    }
#endif

  }

  /* Check all channels to enter in Pre-detect state if needed */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {

    TSL_MCKey_DeltaCalculation(ChannelIndex);

#if NEGDETECT_AUTOCAL == 1
    if (Delta >= pMCKeyStruct->DetectThreshold)
#else
    if ((Delta >= pMCKeyStruct->DetectThreshold) || (Delta <= pMCKeyStruct->RecalibrationThreshold))
#endif
    {
      TSL_MCKey_SetPreDetectState();
      if (!DetectionIntegrator)
      {
        pMCKeyStruct->Channel[0].IntegratorCounter++;
        TSL_MCKey_PreDetectTreatment();
      }
      return;
    }

  }

}


/**
  ******************************************************************************
  * @brief Check MCKey info during PRE DETECT state: Verify detection integrator and detection exclusion.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_PreDetectTreatment(void)
{

  uint8_t ConditionLeaveDetect = 1; /* Per default we go back to idle state */

  /* Go back in idle state if ALL channels are below the detect threshold */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    TSL_MCKey_DeltaCalculation(ChannelIndex);
#if NEGDETECT_AUTOCAL == 1
    if (Delta >= pMCKeyStruct->DetectThreshold)
#else
    if ((Delta >= pMCKeyStruct->DetectThreshold) || (Delta <= pMCKeyStruct->RecalibrationThreshold))
#endif
    {
      ConditionLeaveDetect = 0;
      break;
    }
  }

  if (ConditionLeaveDetect)
  {
    TSL_MCKey_BackToIdleState();
    return;
  }
  else
  {
    TSL_MCKey_DxS();
    pMCKeyStruct->Channel[0].IntegratorCounter--;
    if (!pMCKeyStruct->Channel[0].IntegratorCounter)
    {
      TSL_MCKey_Position();
      TSL_MCKey_SetDetectedState();
    }
  }

}


/**
  ******************************************************************************
  * @brief Check MCKey info during DETECTED state:
  * Verify detection timeout, end of detection and detection exclusion.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_DetectedTreatment(void)
{

  uint8_t ConditionLeaveDetect = 1;

  /* Go in post-detect state if ALL channels are below or equal the end-detect threshold */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    TSL_MCKey_DeltaCalculation(ChannelIndex);
#if NEGDETECT_AUTOCAL == 1
    if (Delta > pMCKeyStruct->EndDetectThreshold)
#else
    if (((Delta > pMCKeyStruct->EndDetectThreshold) && (Delta > 0)) ||
        ((Delta < pMCKeyStruct->RecalibrationThreshold) && (Delta < 0)))
#endif
    {
      ConditionLeaveDetect = 0;
      break;
    }
  }

  if (ConditionLeaveDetect)
  {
    TSL_MCKey_SetPostDetectState();
    if (!EndDetectionIntegrator)
    {
      pMCKeyStruct->Channel[0].IntegratorCounter++;
      TSL_MCKey_PostDetectTreatment();
    }
    return;
  }

  /* Detection TimeOut only if there is no change in the slider/wheel position */
  if (TSL_MCKey_Position() == 0x00)
  {
    TSL_MCKey_DetectionTimeout();
  }

}


/**
  ******************************************************************************
  * @brief Check MCKey info during POST DETECT state: Verify end of detection.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_PostDetectTreatment(void)
{

  uint8_t ConditionLeaveDetect = 1;

  /* Go in idle state if ALL channels are below or equal the end-detect threshold */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    TSL_MCKey_DeltaCalculation(ChannelIndex);
#if NEGDETECT_AUTOCAL == 1
    if (Delta > pMCKeyStruct->EndDetectThreshold)
#else
    if (((Delta > pMCKeyStruct->EndDetectThreshold) && (Delta > 0)) ||
        ((Delta < pMCKeyStruct->RecalibrationThreshold) && (Delta < 0)))
#endif
    {
      ConditionLeaveDetect = 0;
      break;
    }
  }

  if (ConditionLeaveDetect)
  {
    pMCKeyStruct->Channel[0].IntegratorCounter--;
    if (!pMCKeyStruct->Channel[0].IntegratorCounter)
    {
      TSL_MCKey_SetIdleState();
    }
  }
  else
  {
    // No reset of DTO counter.
    TSL_MCKey_BackToDetectedState();
  }
}


/**
  ******************************************************************************
  * @brief Check MCKey info during PRE RECALIBRATION state: Verify condition for recalibration.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_PreRecalibrationTreatment(void)
{

  uint8_t ConditionLeaveCalib = 1;

  /* Go in calibration state if at least ONE channel is below the calibration threshold */
  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    TSL_MCKey_DeltaCalculation(ChannelIndex);
    if (Delta <= pMCKeyStruct->RecalibrationThreshold)
    {
      ConditionLeaveCalib = 0;
      break;
    }
  }

  if (!ConditionLeaveCalib)
  {
    pMCKeyStruct->Channel[0].IntegratorCounter--;
    if (!pMCKeyStruct->Channel[0].IntegratorCounter)
    {
      TSL_MCKey_SetCalibrationState();
    }
  }
  else
  {
    TSL_MCKey_BackToIdleState();
  }

}


/**
  ******************************************************************************
  * @brief During calibration, calculates the new reference.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_CalibrationTreatment(void)
{

  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    pMCKeyStruct->Channel[ChannelIndex].Reference += pMCKeyStruct->Channel[ChannelIndex].LastMeas;
  }

  pMCKeyStruct->Counter--;

  if (!pMCKeyStruct->Counter)
  {
    for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
    {
      // Warning: Must be divided by MCKEY_CALIBRATION_COUNT_DEFAULT !!!
      pMCKeyStruct->Channel[ChannelIndex].Reference = (pMCKeyStruct->Channel[ChannelIndex].Reference >> 3);
    }
    TSL_MCKey_SetIdleState();
  }

}


/**
  ******************************************************************************
  * @brief Calculates the position on the MCKey.
  * @param None
  * @retval uint8_t Position status
  * @retval 0x00 Position not changed
  * @retval 0xFF New position detected
  ******************************************************************************
  */
uint8_t TSL_MCKey_Position(void)
{

  uint8_t Index1, Index2;
  uint16_t Major, Minor, SectorComputation;
  int16_t NewPosition;
  uint8_t uNewPosition;
  uint32_t tmpdelta;
  uint8_t PositionCorrection;
  uint8_t retval = 0x00;

  Delta1 = 0;
  Delta2 = 0;
  Delta3 = 0;

  Index1 = 0;
  Index2 = 0;

  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {

    TSL_MCKey_DeltaCalculation(ChannelIndex);

    // Delta must be positive only otherwise it is noise
    if (Delta < 0)
    {
      Delta = 0;
    }

    /* We normalize the Delta */
    tmpdelta = (uint32_t)(Delta * (uint32_t)(pMCKeyStruct->Channel[0].Reference));
    tmpdelta = tmpdelta / pMCKeyStruct->Channel[ChannelIndex].Reference;
    Delta = (int16_t)tmpdelta;

    /* Apply a fixed coefficient */
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
    if (KeyIndex == 0) // MCKEY1
    {
#endif
      if (MCKEY1_DELTA_COEFF[ChannelIndex] != 0x0100)
      {
        tmpdelta = (uint32_t)(Delta * MCKEY1_DELTA_COEFF[ChannelIndex]);
        Delta = (int16_t)(tmpdelta >> (uint8_t)8);
      }
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
    }
    else // MCKEY2
    {
      if (MCKEY2_DELTA_COEFF[ChannelIndex] != 0x0100)
      {
        tmpdelta = (uint32_t)(Delta * MCKEY2_DELTA_COEFF[ChannelIndex]);
        Delta = (int16_t)(tmpdelta >> (uint8_t)8);
      }
    }
#endif

    /* Sort the biggest, middle and lowest signals measured
       - Delta1 and Index1 = biggest
       - Delta2 and Index2 = middle
       - Delta3 and Index3 = lowest */
    if (Delta > Delta1)
    {
      Delta3 = Delta2;
      Delta2 = Delta1;
      Delta1 = Delta;
      Index2 = Index1;
      Index1 = ChannelIndex;
    }
    else
    {
      if (Delta > Delta2)
      {
        Delta3 = Delta2;
        Delta2 = Delta;
        Index2 = ChannelIndex;
      }
      else
      {
        if (Delta > Delta3)
        {
          Delta3 = Delta;
        }
      }
    }

  } /* for all channels */

  /* Noise filter: we need at least two significant Delta measurements */
  if (Delta2 < ((uint8_t)(pMCKeyStruct->EndDetectThreshold >> 1)) - 1)
  {
    return retval;
  }

  //----------------------------------------------------------------------------
  // Position calculation...
  //----------------------------------------------------------------------------

  /*----------------------------------------------------------------------------
    B = Biggest signal measured (Delta1/Index1)
    M = Middle signal measured (Delta2/Index2)
    S = Smallest signal measured (Delta3/Index3)
    
    - The equation to find the position is:
      Position = Offset +/- [ Sector_Size x ( Major / (Major + Minor) ) ]
   
    - The Offset is the position of the middle of the Middle signal segment.
      All the Offset values are stored in the ROM table Table_POSITION_OFFSET.
   
    - Major = Biggest - Smallest signals
      Minor = Middle - Smallest signals
   
    - The Sector_Size depends of the number of channels used
  ----------------------------------------------------------------------------*/

  /* Calculates the Major and Minor parameters */
  Minor = Delta2 - Delta3; // Middle - Smallest signals
  Major = Delta1 - Delta3; // Biggest - Smallest signals

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
  if (KeyIndex == 0) // MCKEY1
  {
#endif
    NewPosition = MCKEY1_TABLE_POSITION_OFFSET[Index1][Index2];
    SectorComputation = MCKEY1_SECTOR_COMPUTATION;
    PositionCorrection = MCKEY1_POSITION_CORRECTION;
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
  }
  else // MCKEY2
  {
    NewPosition = MCKEY2_TABLE_POSITION_OFFSET[Index1][Index2];
    SectorComputation = MCKEY2_SECTOR_COMPUTATION;
    PositionCorrection = MCKEY2_POSITION_CORRECTION;
  }
#endif

  /* Calculates: [ Sector_Size x ( Major / (Major + Minor) ) ] */
  SectorComputation = Major * SectorComputation;
  SectorComputation = SectorComputation / (Major + Minor);

  // Use the sign bit from table to define the interpretation direction.
  // The NewPosition is multiplied by 2 because the Offset stored in the ROM
  // table is divided by 2...
  if (NewPosition > 0)   // means Offset is > 0 in the ROM table
  {
    NewPosition = (int16_t)(NewPosition << 1); /*lint !e701 suppress info on this line only */
    NewPosition += SectorComputation;
  }
  else // means Offset is <= 0 in the ROM table
  {
    NewPosition = (int16_t)((-NewPosition) << 1); /*lint !e701 suppress info on this line only */
    NewPosition -= SectorComputation;
  }

  if (pMCKeyStruct->Setting.b.MCKEY_TYPE) // It's a Slider...
  {

    /* First adjustment used to shift all the values to obtain the "zero" */
    if (NewPosition > 0)
    {
      NewPosition -= PositionCorrection;
    }
    else
    {
      NewPosition = NewPosition + 256 - PositionCorrection;
    }

    /* Second adjustment used to clamp the values at extremities of slider */
    if ((NewPosition & 0x8000) != 0)
    {
      NewPosition = 0;
    }
    if ((NewPosition & 0x7f00) != 0)
    {
      NewPosition = 255;
    }

  }
  else // It's a Wheel: we keep only the low byte
  {
    NewPosition = (uint8_t)NewPosition;
  }

  //----------------------------------------------------------------------------
  // Direction Change Process
  //----------------------------------------------------------------------------

#if MCKEY_DIRECTION_CHANGE_ENABLED > 0

  if (pMCKeyStruct->Setting.b.DIRECTION) // Anticlockwise direction ...
  {

    // Check Direction changed and Position overflow from 0x00 to 0xFF not realized !
    if (((uint8_t)NewPosition > pMCKeyStruct->UnScaledPosition) && (((uint8_t)NewPosition - pMCKeyStruct->UnScaledPosition) < MCKEY_DIRECTION_CHANGE_MAX_DISPLACEMENT))
    {
      if (NewPosition < (uint16_t)(pMCKeyStruct->UnScaledPosition + pMCKeyStruct->DirectionChangeThreshold))
      {
        pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
        return retval;
      }
      else
      {
        pMCKeyStruct->Channel[1].IntegratorCounter--;
        if (!pMCKeyStruct->Channel[1].IntegratorCounter)
        {
          pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
          pMCKeyStruct->Setting.b.DIRECTION = 0;  // New direction accepted: clockwise.
        }
        else
        {
          return retval;
        }
      }
    }

    // Check position overflow from 0xFF to 0x00 to be filtered !
    if ((NewPosition + MCKEY_DIRECTION_CHANGE_MAX_DISPLACEMENT) < pMCKeyStruct->UnScaledPosition)
    {
      if ((NewPosition + MCKEY_DIRECTION_CHANGE_TOTAL_STEPS) < (uint16_t)(pMCKeyStruct->UnScaledPosition + pMCKeyStruct->DirectionChangeThreshold))
      {
        pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
        return retval;
      }
      else
      {
        pMCKeyStruct->Channel[1].IntegratorCounter--;
        if (!pMCKeyStruct->Channel[1].IntegratorCounter)
        {
          pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
          pMCKeyStruct->Setting.b.DIRECTION = 0;  // New direction accepted: clockwise.
        }
        else
        {
          return retval;
        }
      }
    }

  }
  else // Clockwise direction... DEFAULT SETTING !
  {

    // Check Direction changed and Position overflow from 0xFF to 0x00 not realized !
    if (((uint8_t)NewPosition < pMCKeyStruct->UnScaledPosition) && ((pMCKeyStruct->UnScaledPosition - (uint8_t)NewPosition) < MCKEY_DIRECTION_CHANGE_MAX_DISPLACEMENT))
    {
      if ((NewPosition + pMCKeyStruct->DirectionChangeThreshold) > pMCKeyStruct->UnScaledPosition)
      {
        pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
        return retval;
      }
      else
      {
        pMCKeyStruct->Channel[1].IntegratorCounter--;
        if (!pMCKeyStruct->Channel[1].IntegratorCounter)
        {
          pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
          pMCKeyStruct->Setting.b.DIRECTION = 1;  // New direction accepted: anticlockwise.
        }
        else
        {
          return retval;
        }
      }
    }

    // Check position overflow from 0x00 to 0xFF to be filtered !
    if (NewPosition > (uint16_t)(pMCKeyStruct->UnScaledPosition + MCKEY_DIRECTION_CHANGE_MAX_DISPLACEMENT))
    {
      if ((NewPosition + pMCKeyStruct->DirectionChangeThreshold) > (uint16_t)(pMCKeyStruct->UnScaledPosition + MCKEY_DIRECTION_CHANGE_TOTAL_STEPS))
      {
        pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
        return retval;
      }
      else
      {
        pMCKeyStruct->Channel[1].IntegratorCounter--;
        if (!pMCKeyStruct->Channel[1].IntegratorCounter)
        {
          pMCKeyStruct->Channel[1].IntegratorCounter = pMCKeyStruct->DirectionChangeIntegrator;
          pMCKeyStruct->Setting.b.DIRECTION = 1;  // New direction accepted: anticlockwise.
        }
        else
        {
          return retval;
        }
      }
    }

  }

#endif // MCKEY_DIRECTION_CHANGE_ENABLED
//# MCKEY_DIRECTION_CHANGE_ENABLED > 0

  //----------------------------------------------------------------------------
  // Final result...
  //----------------------------------------------------------------------------

  // The UnScaledPosition parameter is always updated
  // The Position parameter is updated only if different from the previous one

  pMCKeyStruct->UnScaledPosition = (uint8_t)NewPosition;

  uNewPosition = (uint8_t)((uint8_t)NewPosition >> (MCKEY_RESOLUTION_CALCULATION - pMCKeyStruct->Resolution));

  if (pMCKeyStruct->Position != uNewPosition)
  {
    pMCKeyStruct->Position = uNewPosition;
    pMCKeyStruct->Setting.b.POSCHANGED = 1; /* Warning: Application layer must reset this flag */
    retval = 0xFF;
  }

  return retval;

}


/**
  ******************************************************************************
  * @brief Check for customer code setting and adapt key state.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_CheckDisabled(void)
{

  if (!pMCKeyStruct->Setting.b.ENABLED)
  {
    TSL_MCKey_SetDisabledState();
  }

}


/**
  ******************************************************************************
  * @brief Check for customer code setting and adapt key state.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_MCKey_CheckEnabled(void)
{

  if (pMCKeyStruct->Setting.b.ENABLED && pMCKeyStruct->Setting.b.IMPLEMENTED)
  {
    TSL_MCKey_SetCalibrationState();
  }

}


/**
  ******************************************************************************
  * @brief Verify the last burst sequence gave a result within the authorized range.
  * @param None
  * @retval uint8_t Error status
  * @retval 0x00 Success
  * @retval 0xFF Error
  ******************************************************************************
  */
uint8_t TSL_MCKey_CheckErrorCondition(void)
{

  for (ChannelIndex = 0; ChannelIndex < CHANNEL_PER_MCKEY; ChannelIndex++)
  {
    if ((pMCKeyStruct->Channel[ChannelIndex].LastMeas < MCKEY_MIN_ACQUISITION)
        || (pMCKeyStruct->Channel[ChannelIndex].LastMeas > MCKEY_MAX_ACQUISITION))
    {
      return 0xFF;  // Error case !
    }
  }

  return 0;

}

#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0

/* Public functions ----------------------------------------------------------*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
