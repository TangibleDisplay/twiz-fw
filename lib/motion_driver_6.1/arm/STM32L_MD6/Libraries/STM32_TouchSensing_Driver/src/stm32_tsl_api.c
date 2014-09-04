/**
  ******************************************************************************
  * @file    stm32_tsl_api.c
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file defines the memory buffers,
  *          variables and functions used to communicate between Touch Sensing
  *          Library and the customer code.
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
#include "stm32_tsl_api.h"
#include "stm32_tsl_singlechannelkey.h"
#include "stm32_tsl_multichannelkey.h"
#include "stm32_tsl_services.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----             MEMORY BUFFER API                                    -----
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TSLState_T TSLState;
uint8_t KeyIndex;

KeyFlag_T TSL_GlobalSetting;
KeyState_T TSL_GlobalState;

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
Single_Channel_Complete_Info_T *  pKeyStruct;
Single_Channel_Complete_Info_T sSCKeyInfo[NUMBER_OF_SINGLE_CHANNEL_KEYS];
#endif
//# NUMBER_OF_SINGLE_CHANNEL_KEYS > 0

uint8_t DetectionTimeout;
uint8_t DetectionIntegrator;
uint8_t EndDetectionIntegrator;
uint8_t RecalibrationIntegrator;
uint8_t ECSTimeStep;
uint8_t ECSTemporization;
uint8_t ECS_K_Fast;
uint8_t ECS_K_Slow;

#ifdef IT_SYNC
__IO IT_Sync_Flags_T IT_Sync_Flags;
#endif
//# IT_SYNC

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0

#if defined(USE_PRAGMA_SECTION)
#pragma section [TSLMCK_RAM]
#pragma section [TSLMCK_RAM0]
#endif
//# defined(USE_PRAGMA_SECTION)

uint8_t ChannelIndex;

#if (CHANNEL_PER_MCKEY == 3)
Tria_Channel_Complete_Info_T * pMCKeyStruct;
Tria_Channel_Complete_Info_T sMCKeyInfo[NUMBER_OF_MULTI_CHANNEL_KEYS];
#else
#error "CHANNEL_PER_MCKEY must be defined to 3 only."
#endif
//# (CHANNEL_PER_MCKEY == 3)

#endif // NUMBER_OF_MULTI_CHANNEL_KEYS > 0
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 0

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//-----             LIBRARY PRIVATE GLOBALS                              -----
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int16_t  Delta;
uint32_t Local_TickECS10ms;
TimerFlag_T Local_TickFlag;
uint8_t ECSTimeStepCounter;
uint8_t ECSTempoCounter;
uint8_t ECSTempoPrescaler;
KeyFlag_T TSL_TempGlobalSetting;
KeyState_T TSL_TempGlobalState;

//============================================================================
//-----         CONDITIONAL COMPILING FOR MCKEYS !!!               -----------
//============================================================================

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0

int16_t Delta1;
int16_t Delta2;
int16_t Delta3;

#endif
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 0

#if defined(USE_PRAGMA_SECTION)
#pragma section [TSL_RAM]
#pragma section [TSL_RAM0]
#endif
//# defined(USE_PRAGMA_SECTION)

/**
  ******************************************************************************
  * @brief Initialize memory API and structures.
  * @param None
  * @retval None
  * @note Must be run once in the main function during initialization phase.
  ******************************************************************************
  */
void TSL_Init(void)
{

  disableInterrupts();

  DetectionIntegrator = DETECTION_INTEGRATOR_DEFAULT;
  EndDetectionIntegrator = END_DETECTION_INTEGRATOR_DEFAULT;
  ECSTimeStep = ECS_TIME_STEP_DEFAULT;
  ECSTemporization = ECS_TEMPO_DEFAULT;
  RecalibrationIntegrator = RECALIBRATION_INTEGRATOR_DEFAULT;
  DetectionTimeout = DTO_DEFAULT;

  ECS_K_Fast = ECS_IIR_KFAST_DEFAULT;
  ECS_K_Slow = ECS_IIR_KSLOW_DEFAULT;
  ECSTimeStepCounter = ECSTimeStep;
  ECSTempoCounter = 0;
  ECSTempoPrescaler = 0;

  TSL_IO_Init();

  TSL_Timer_Init();

#if NUMBER_OF_SINGLE_CHANNEL_KEYS > 0
  TSL_SCKey_Init();
#endif

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
  TSL_MCKey_Init();
#endif

  enableInterrupts();

  TSLState = TSL_IDLE_STATE;

}


/**
  ******************************************************************************
  * @brief Main function of Touch Sensing Library.
  * @param None
  * @retval None
  * @note Must be called from main loop to run the library state machine.
  ******************************************************************************
  */
void TSL_Action(void)
{

  switch (TSLState)
  {

    case TSL_IDLE_STATE:
      disableInterrupts();
      Local_TickFlag.b.DTO_1sec = TSL_Tick_Flags.b.DTO_1sec;
      TSL_Tick_Flags.b.DTO_1sec = 0;
      enableInterrupts();

#if NUMBER_OF_SINGLE_CHANNEL_PORTS == 0
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
      TSLState = TSL_MCKEY1_ACQ_STATE;
#else
      TSLState = TSL_ECS_STATE;
#endif
#endif
//# NUMBER_OF_SINGLE_CHANNEL_PORTS == 0

#if NUMBER_OF_ACQUISITION_PORTS > 0
      TSLState = TSL_SCKEY_P1_ACQ_STATE;
      break;

    case TSL_SCKEY_P1_ACQ_STATE:
      TSL_SCKEY_P1_Acquisition();
      TSLState = TSL_SCKEY_P1_PROC_STATE;
      break;

    case TSL_SCKEY_P1_PROC_STATE:
#if (SCKEY_P1_KEY_COUNT > 0)
      for (KeyIndex = 0; KeyIndex < SCKEY_P1_KEY_COUNT; KeyIndex++)
      {
        TSL_SCKey_Process();
      }
#endif
#endif
//# NUMBER_OF_ACQUISITION_PORTS > 0

#if NUMBER_OF_ACQUISITION_PORTS > 1
      TSLState = TSL_SCKEY_P2_ACQ_STATE;
#else
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
      TSLState = TSL_MCKEY1_ACQ_STATE;
#else
      TSLState = TSL_ECS_STATE;
#endif
#endif
//# NUMBER_OF_ACQUISITION_PORTS > 1

      break;

#if NUMBER_OF_ACQUISITION_PORTS > 1
    case TSL_SCKEY_P2_ACQ_STATE:
      TSL_SCKEY_P2_Acquisition();
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 1
      TSLState = TSL_SCKEY_P2_PROC_STATE;
      break;

    case TSL_SCKEY_P2_PROC_STATE:
      for (KeyIndex = SCKEY_P1_KEY_COUNT; KeyIndex < (SCKEY_P2_KEY_COUNT + SCKEY_P1_KEY_COUNT); KeyIndex++)
      {
        TSL_SCKey_Process();
      }
#endif
#if NUMBER_OF_ACQUISITION_PORTS > 2
      TSLState = TSL_SCKEY_P3_ACQ_STATE;
#else
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
      TSLState = TSL_MCKEY1_ACQ_STATE;
#else
      TSLState = TSL_ECS_STATE;
#endif
#endif
      break;
#endif
//# NUMBER_OF_ACQUISITION_PORTS > 1

#if NUMBER_OF_ACQUISITION_PORTS > 2
    case TSL_SCKEY_P3_ACQ_STATE:
      TSL_SCKEY_P3_Acquisition();
#if NUMBER_OF_SINGLE_CHANNEL_PORTS > 2
      TSLState = TSL_SCKEY_P3_PROC_STATE;
      break;

    case TSL_SCKEY_P3_PROC_STATE:
      for (KeyIndex = (SCKEY_P1_KEY_COUNT + SCKEY_P2_KEY_COUNT); KeyIndex < (SCKEY_P3_KEY_COUNT + SCKEY_P1_KEY_COUNT + SCKEY_P2_KEY_COUNT); KeyIndex++)
      {
        TSL_SCKey_Process();
      }
#endif
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
      TSLState = TSL_MCKEY1_ACQ_STATE;
#else
      TSLState = TSL_ECS_STATE;
#endif
      break;
#endif
//# NUMBER_OF_ACQUISITION_PORTS > 2

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0
    case TSL_MCKEY1_ACQ_STATE:
      TSL_MCKey1_Acquisition();
#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
      TSLState = TSL_MCKEY2_ACQ_STATE;
#else
      TSLState = TSL_MCKEY_PROC_STATE;
#endif
      break;

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1
    case TSL_MCKEY2_ACQ_STATE:
      TSL_MCKey2_Acquisition();
      TSLState = TSL_MCKEY_PROC_STATE;
      break;
#endif

    case TSL_MCKEY_PROC_STATE:
      for (KeyIndex = 0; KeyIndex < NUMBER_OF_MULTI_CHANNEL_KEYS; KeyIndex++)
      {
        TSL_MCKey_Process();
      }
      TSLState = TSL_ECS_STATE;
      break;
#endif
//# NUMBER_OF_MULTI_CHANNEL_KEYS > 0

    case TSL_ECS_STATE:
      TSL_ECS();
      TSL_GlobalSetting.whole = TSL_TempGlobalSetting.whole;
      TSL_TempGlobalSetting.whole = 0;
      TSL_GlobalState.whole = TSL_TempGlobalState.whole;
      TSL_TempGlobalState.whole = 0;
      TSLState = TSL_IDLE_STATE;
      break;

    default:
      for (;;)
      {
        // Wait for Watchdog reset ...
      }
  }
}

/* Public functions ----------------------------------------------------------*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
