/**
  ******************************************************************************
  * @file    stm32_tsl_timebase.h
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file provides all the functions
  *          to manage the Charge-Transfer acquisition based on STM32L15x devices
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
#include "stm32l1xx.h"

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TSL_TIMEBASE_H
#define __TSL_TIMEBASE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_tsl_conf.h"

/* Exported types ------------------------------------------------------------*/
/** Contains all timer flags */
typedef union
{
  uint8_t whole;
  struct
  {
  unsigned DTO_1sec          : 1; /**< Detection Time Out (flag set every second) */
  unsigned User1_Start_100ms : 1; /**< From Customer Code to TSL */
  unsigned User1_Flag_100ms  : 1; /**< From TSL to Customer Code */
  unsigned User2_Start_100ms : 1; /**< From Customer Code to TSL */
  unsigned User2_Flag_100ms  : 1; /**< From TSL to Customer Code */
  unsigned RESERVED5         : 1;
  unsigned RESERVED6         : 1;
  unsigned RESERVED7         : 1;
  }
  b;
} TimerFlag_T;

/* Exported constants --------------------------------------------------------*/
/* This factor multiplied by the tick base (0.5ms) must give a 10ms delay */
#define TICK_FACTOR_10MS (20)

/* Exported macro ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
extern uint32_t TSL_Tick_Base;
extern uint32_t TSL_Tick_10ms;
extern uint32_t TSL_Tick_100ms;
extern uint32_t TSL_TickCount_ECS_10ms;
extern TimerFlag_T TSL_Tick_Flags;

/* Exported functions ------------------------------------------------------- */
void TSL_Timer_ISR(void);
void TSL_Timer_Init(void);
void TSL_Timer_Adjust(uint32_t adjust_delay);
void TSL_Timer_Check_1sec_Tick(void);
void TSL_Timer_Check_100ms_Tick(void);
void TSL_Timer_Check_10ms_Tick(void);
void TSL_Wait(__IO uint32_t delay);

#endif /* __TSL_TIMEBASE_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
