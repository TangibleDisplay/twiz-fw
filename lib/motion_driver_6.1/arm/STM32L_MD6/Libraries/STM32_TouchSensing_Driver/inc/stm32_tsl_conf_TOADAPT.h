/**
  ******************************************************************************
  * @file    stm32_tsl_conf_TOADAPT.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file is a template for the
  *          Charge-Transfer acquisition and must be adapted to your requirements.
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
#ifndef __TSL_CONF_H
#define __TSL_CONF_H

//==============================================================================
// The below values are only used for the groups and channels settings.
// Do NOT change the values.
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
#define CH1     (1)
#define CH2     (2)
#define CH3     (3)
#define CH4     (4)

//==============================================================================
// Groups and Channels description for STM32L15x devices.
// For more details please refer to the Touch sensing controller section in the
// reference manual.
//
// GROUP1    CH1   PA0  __/____
//           CH2   PA1  __/__|
//           CH3   PA2  __/__|
//           CH4   PA3  __/__|
//
// GROUP2    CH1   PA6  __/____
//           CH2   PA7  __/__|
//
// GROUP3    CH1   PB0  __/____
//           CH2   PB1  __/__|

// GROUP4    CH1   PA8  __/____
//           CH2   PA9  __/__|
//           CH3   PA10 __/__|
//
// GROUP5    CH1   PA13  __/____
//           CH2   PA14  __/__|
//           CH3   PA15  __/__|
//
// GROUP6    CH1   PB4  __/____ DO NOT USE DUE TO POOR SENSITIVITY
//           CH2   PB5  __/__|  DO NOT USE DUE TO POOR SENSITIVITY
//
// GROUP7    CH1   PB12  __/____
//           CH2   PB13  __/__|
//           CH3   PB14  __/__|
//           CH4   PB15  __/__|
//
// GROUP8    CH1   PC0  __/____
//           CH2   PC1  __/__|
//           CH3   PC2  __/__|
//           CH4   PC3  __/__|
//
// GROUP9    CH1   PC4  __/____
//           CH2   PC5  __/__|
//
// GROUP10   CH1   PC6 __/____
//           CH2   PC7 __/__|
//           CH3   PC8 __/__|
//           CH4   PC9 __/__|
//==============================================================================


/** @addtogroup IOs_Setting
  * @{ */

//==============================================================================
//
// 1) MCU FAMILY SELECTION
//
// In this version of the Library only the STM32L15XX8 (64kb) or STM32L15XXB (128kb)
// devices are supported.
// Select only one line below to identify the device used.
//
//==============================================================================

#define STM32L15XX8_64K  (1)  /**< Select this line if the STM32L15XX8 (64Kb Flash) devices are used */
//#define STM32L15XXB_128K (1)  /**< Select this line if the STM32L15XXB (128Kb Flash) devices are used */

//==============================================================================
//
// 2) SAMPLING CAPACITOR CHANNEL SELECTION
//
// Set the sampling capacitors channel.
//
// The hardware for CHARGE_TRANSFER is done in such way that all the sampling
// capacitors have to be connected to the same channel of each group.
// For example, the sampling capacitors are connected to CH2 of each group.
//
//==============================================================================

#define SAMP_CAP_CH   (CH4)  /**< Possible values are CH1, CH2, CH3 and CH4 */


/** @addtogroup SCKeys_IOs
  * @{ */

//==============================================================================
//
// 3) SINGLE-CHANNEL KEYS DEFINITION FOR THE 1st BANK
//
// Set the number of keys
// Set the channel (must be different from the sampling capacitor channel)
// Set the group for each key, knowing that:
//  if CH1 or CH2 selected: all groups can be selected
//  if CH3 selected       : only GROUP 1, 4, 5, 7, 8 and 10 can be selected
//  if CH4 selected       : only GROUP 1,       7, 8 and 10 can be selected
//
//==============================================================================

#define SCKEY_P1_KEY_COUNT  (1)

#define SCKEY_P1_CH   (CH1) /**< Possible values are CH1, CH2, CH3 and CH4 */

#define SCKEY_P1_A  (GROUP10)
#define SCKEY_P1_B  (0)
#define SCKEY_P1_C  (0)
#define SCKEY_P1_D  (0)
#define SCKEY_P1_E  (0)
#define SCKEY_P1_F  (0)
#define SCKEY_P1_G  (0)
#define SCKEY_P1_H  (0)
#define SCKEY_P1_I  (0)
#define SCKEY_P1_J  (0)


//==============================================================================
//
// 4) SINGLE-CHANNEL KEYS DEFINITION FOR THE 2nd BANK
//
// Set the number of keys
// Set the channel (must be different from the sampling capacitor channel)
// Set the group for each key, knowing that:
//  if CH1 or CH2 selected: all groups can be selected
//  if CH3 selected       : only GROUP 1, 4, 5, 7, 8 and 10 can be selected
//  if CH4 selected       : only GROUP 1,       7, 8 and 10 can be selected
//
//==============================================================================

#define SCKEY_P2_KEY_COUNT  (1)

#define SCKEY_P2_CH   (CH2) /**< Possible values are CH1, CH2, CH3 and CH4 */

#define SCKEY_P2_A  (GROUP10)
#define SCKEY_P2_B  (0)
#define SCKEY_P2_C  (0)
#define SCKEY_P2_D  (0)
#define SCKEY_P2_E  (0)
#define SCKEY_P2_F  (0)
#define SCKEY_P2_G  (0)
#define SCKEY_P2_H  (0)
#define SCKEY_P2_I  (0)
#define SCKEY_P2_J  (0)


//==============================================================================
//
// 5) SINGLE-CHANNEL KEYS DEFINITION FOR THE 3rd BANK
//
// Set the number of keys
// Set the channel (must be different from the sampling capacitor channel)
// Set the group for each key, knowing that:
//  if CH1 or CH2 selected: all groups can be selected
//  if CH3 selected       : only GROUP 1, 4, 5, 7, 8 and 10 can be selected
//  if CH4 selected       : only GROUP 1,       7, 8 and 10 can be selected
//
//==============================================================================

#define SCKEY_P3_KEY_COUNT  (0)

#define SCKEY_P3_CH  (0) /**< Possible values are CH1, CH2, CH3 and CH4 */

#define SCKEY_P3_A  (0)
#define SCKEY_P3_B  (0)
#define SCKEY_P3_C  (0)
#define SCKEY_P3_D  (0)
#define SCKEY_P3_E  (0)
#define SCKEY_P3_F  (0)
#define SCKEY_P3_G  (0)
#define SCKEY_P3_H  (0)
#define SCKEY_P3_I  (0)
#define SCKEY_P3_J  (0)

/** @} SCKeys_IOs */

/** @addtogroup MCKeys_IOs
  * @{ */

//==============================================================================
//
// 6) NUMBER OF MULTI-CHANNEL KEYS AND NUMBER OF CHANNELS USED PER MCKEY
//
// Set the total number of multi-channel keys used (0, 1 or 2)
//
//==============================================================================

#define NUMBER_OF_MULTI_CHANNEL_KEYS  (2)  /**< Number of multi channel keys (value from 0 to 2) */


//==============================================================================
//
// 7) 1st MULTI-CHANNEL KEY DEFINITION (OPTIONAL)
//
// For all the electrodes composing the multi-channel key:
//   Set the channel (must be different from the sampling capacitor channel)
//   set the group
// Define the type of the MCKey (wheel or slider)
//
//==============================================================================

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 0

#define MCKEY1_A_CH  (CH3)      /**< Multi channel key 1: 1st channel port */
#define MCKEY1_A     (GROUP3)   /**< Multi channel key 1: 1st channel mask */
#define MCKEY1_B_CH  (CH3)      /**< Multi channel key 1: 2nd channel port */
#define MCKEY1_B     (GROUP2)   /**< Multi channel key 1: 2nd channel mask */
#define MCKEY1_C_CH  (CH3)      /**< Multi channel key 1: 3rd channel port */
#define MCKEY1_C     (GROUP4)   /**< Multi channel key 1: 3rd channel mask */

#define MCKEY1_TYPE (0)         /**< Multi channel key 1 type: 0=wheel (zero between two electrodes), 1=slider (zero in the middle of one electrode) */

#endif


//==============================================================================
//
// 8) 2nd MULTI-CHANNEL KEY DEFINITION (OPTIONAL)
//
// For all the electrodes composing the multi-channel key:
//   Set the channel (must be different from the sampling capacitor channel)
//   set the group
// Define the type of the MCKey (wheel or slider)
//
//==============================================================================

#if NUMBER_OF_MULTI_CHANNEL_KEYS > 1

#define MCKEY2_A_CH   (CH1)        /**< Multi channel key 2: 1st channel port */
#define MCKEY2_A      (GROUP2)     /**< Multi channel key 2: 1st channel mask */
#define MCKEY2_B_CH   (CH1)        /**< Multi channel key 2: 2nd channel port */
#define MCKEY2_B      (GROUP4)     /**< Multi channel key 2: 2nd channel mask */
#define MCKEY2_C_CH   (CH1)        /**< Multi channel key 2: 3rd channel port */
#define MCKEY2_C      (GROUP3)     /**< Multi channel key 2: 3rd channel mask */

#define MCKEY2_TYPE (1)            /**< Multi channel key 2 type: 0=wheel (zero between two electrodes), 1=slider (zero in the middle of one electrode) */

#endif

/** @} MCKeys_IOs */

/** @} IOs_Setting */


//============================================================================
//
// 9) TSL PARAMETERS CONFIGURATION
//
//============================================================================

/** @addtogroup TSL_Parameters
  * @{ */

/** @addtogroup SCKeys_Parameters
  * @{ */

// Single-channel keys thresholds
#define SCKEY_DETECTTHRESHOLD_DEFAULT                (8)  /**< Single channel key detection threshold (value from 1 to 127) */
#define SCKEY_ENDDETECTTHRESHOLD_DEFAULT             (5)  /**< Single channel key end detection threshold (value from 1 to 127) */
#define SCKEY_RECALIBRATIONTHRESHOLD_DEFAULT       (-10)  /**< Single channel key calibration threshold (value from -1 to -128) */

// Single-channel keys Acquisition values limits
#define SCKEY_MIN_ACQUISITION                       (50)  /**< Single channel key minimum acquisition value */
#define SCKEY_MAX_ACQUISITION                     (3000)  /**< Single channel key maximum acquisition value */

/** @} SCKeys_Parameters */

/** @addtogroup MCKeys_Parameters
  * @{ */

// Multi-channel keys thresholds
#define MCKEY_DETECTTHRESHOLD_DEFAULT               (30)  /**< Multi channel key detection threshold (value from 1 to 127) */
#define MCKEY_ENDDETECTTHRESHOLD_DEFAULT            (20)  /**< Multi channel key end detection threshold (value from 1 to 127) */
#define MCKEY_RECALIBRATIONTHRESHOLD_DEFAULT       (-30)  /**< Multi channel key calibration threshold (value from -1 to -128) */

// Multi-channel keys resolution
#define MCKEY_RESOLUTION_DEFAULT                     (4)  /**< Multi channel key resolution (value from 1 to 8) */

// Multi-channel keys Direction Change process
#define MCKEY_DIRECTION_CHANGE_ENABLED               (1)  /**< Multi channel key direction change enable (1) or disable (0) switch */
#define MCKEY_DIRECTION_CHANGE_MAX_DISPLACEMENT    (255)  /**< Multi channel key direction change maximum displacement (value from 0 to 255) */
#define MCKEY_DIRECTION_CHANGE_INTEGRATOR_DEFAULT    (1)  /**< Multi channel key direction change integrator (value from 1 to 255) */
#define MCKEY_DIRECTION_CHANGE_THRESHOLD_DEFAULT    (10)  /**< Multi channel key direction change threshold (value from 1 to 255) */

// Multi-channel keys Acquisition values limits
#define MCKEY_MIN_ACQUISITION                      (150)  /**< Multi channel key minimum acquisition value */
#define MCKEY_MAX_ACQUISITION                     (5000)  /**< Multi channel key maximum acquisition value */

// Optional parameters for Delta Normalization Process
// The MSB is the integer part, the LSB is the real part:
// For example to apply a factor 1.10:
// 0x01 to the MSB
// 0x1A to the LSB (0.1 x 256 = 25.6 -> 26 = 0x1A)
// Final value to define is: 0x011A

// 1st Multi-channel keys Delta Normalization coefficients
#define MCKEY1_DELTA_COEFF_A  (0x0100)  /**< MCKey1 Channel A parameter */
#define MCKEY1_DELTA_COEFF_B  (0x0100)  /**< MCKey1 Channel B parameter */
#define MCKEY1_DELTA_COEFF_C  (0x0100)  /**< MCKey1 Channel C parameter */

// 2nd Multi-channel keys Delta Normalization coefficients
#define MCKEY2_DELTA_COEFF_A  (0x0100)  /**< MCKey2 Channel A parameter */
#define MCKEY2_DELTA_COEFF_B  (0x0100)  /**< MCKey2 Channel B parameter */
#define MCKEY2_DELTA_COEFF_C  (0x0100)  /**< MCKey2 Channel C parameter */

/** @} MCKeys_Parameters */

/** @addtogroup Common_Parameters
  * @{ */

// Integrators
#define DETECTION_INTEGRATOR_DEFAULT       (2)  /**< Detection Integrator = Debounce Filter (value from 0 to 255) */
#define END_DETECTION_INTEGRATOR_DEFAULT   (2)  /**< End detection Integrator = Debounce Filter (from 0 to 255) */
#define RECALIBRATION_INTEGRATOR_DEFAULT  (10)  /**< Calibration integrator (value from 1 to 255) */

// IIR Filter
#define ECS_TIME_STEP_DEFAULT             (20)  /**< IIR Filter Sampling frequency, multiple of 10ms */
#define ECS_TEMPO_DEFAULT                 (20)  /**< IIR Filter Delay after detection, multiple of 100ms */
#define ECS_IIR_KFAST_DEFAULT             (20)  /**< IIR Filter K factor for fast filtering */
#define ECS_IIR_KSLOW_DEFAULT             (10)  /**< IIR Filter K factor for slow filtering */

// Detection Timeout
#define DTO_DEFAULT                        (2)  /**< 1s unit (value from 0 (= infinite!) to 255) */

// IOs protection from Interrupt modification
#define PROTECT_IO_ACCESS                  (1)  /**< IOs protection from Interrupt modification: When enabled (=1), the interruptions are disabled when the IOs are written during the acquisition sequence */

// Capacitance Loading Waiting HIGH Time Adjustment
#define CLWHTA                             (1)  /**< Waiting time during capacitor charging: value from 0 to 50 (= 1.56µs @ 32MHz) */
// Capacitance Loading Waiting LOW Time Adjustment
#define CLWLTA                             (1)  /**< Waiting time during transfer phase: value from 0 to 50 (= 1.56µs @ 32MHz) */

// Spread spectrum
#define SPREAD_SPECTRUM                    (0)  /**< Spread spectrum enabled (=1) */
#define SW_SPREAD_SPECTRUM                 (0)  /**< Software Spread spectrum enabled (=1) for STM32L15x only when the UART is also used */
#define SPREAD_COUNTER_MIN                 (1)  /**< Spread min value */
#define SPREAD_COUNTER_MAX               (254)  /**< Spread max value */

// Active Shield
#define ACTIVE_SHIELD_GROUP                (0)  /**< Active Shield is disabled (=0), else this symbolic constant contains the group dedicated to this function, this group will be driven but never acquired */
#define ACTIVE_SHIELD_CHANNEL              (0)  /**< Active Shield channel: CH1 to CH4 to select the channel or '0' to select all the pins of the group */

// Timer Callback to allow the user to add its own function called from the timer interrupt sub-routine
#define TIMER_CALLBACK                     (0)  /**< if (1) Allows the use of a callback function in the timer interrupt. This function will be called every 0.5ms. The callback function must be defined inside the application and have the following prototype FAR void USER_TickTimerCallback(void); */

// Inline functions
#define USE_INLINED_FUNCTIONS              (1)  /**< Inline functions are enabled (=1) */

/** @} Common_Parameters */

/** @} TSL_Parameters */


//==============================================================================
//
// DEFINITIONS CHECK. DO NOT TOUCH ANYTHING BELOW !!!
//
//==============================================================================

// The Charge-Transfer acquisition is the only acquisition principle supported
// in this version of the Library and must be always defined.
#define CHARGE_TRANSFER (1)

// Automatic Calibration always enabled
#define NEGDETECT_AUTOCAL (1)

#include "stm32_tsl_checkconfig.h"

#endif /* __TSL_CONF_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
