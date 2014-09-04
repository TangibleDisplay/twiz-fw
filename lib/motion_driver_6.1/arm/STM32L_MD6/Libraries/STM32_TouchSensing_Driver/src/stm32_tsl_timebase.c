/**
  ******************************************************************************
  * @file    stm32_tsl_timebase.c
  * @author  MCD Application Team
  * @version v0.1.3
  * @date    23-September-2011
  * @brief   STM32 Touch Sensing Library - This file provides all the functions
  *          to handle the time base.
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
#include "stm32_tsl_timebase.h"
#include "stm32_tsl_api.h"
#include "stm32_tsl_internal.h"
#include "stm32l1xx_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint32_t TSL_Tick_Base = 0;
uint32_t TSL_Tick_10ms = 0;
uint32_t TSL_Tick_100ms = 0;
uint32_t TSL_TickCount_ECS_10ms = 0;
uint32_t TSL_Tick_User1 = 0;
uint32_t TSL_Tick_User2 = 0;
TimerFlag_T  TSL_Tick_Flags;
RCC_ClocksTypeDef RCC_Clocks;
static __IO uint32_t TSL_TimingDelay = 0;

/* Private function prototypes -----------------------------------------------*/
void TSL_Timer_Init(void);
void TSL_Timer_Check_1sec_Tick(void);
void TSL_Timer_Check_100ms_Tick(void);
void TSL_Timer_Check_10ms_Tick(void);
void TSL_Timer_Adjust(uint32_t adjust_delay);

/* Private functions ---------------------------------------------------------*/

/**
  ******************************************************************************
  * @brief Timer initialization used for ECS, DTO and user application.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_Init(void)
{

  TSL_Tick_100ms = 0;
  TSL_Tick_10ms = 0;
  TSL_Tick_Base = 0;
  TSL_TickCount_ECS_10ms = 0;
  TSL_Tick_Flags.whole = 0;

  /* Configure SysTick timer to generate interrupts every 500탎 */
  RCC_GetClocksFreq(&RCC_Clocks);

  if (SysTick_Config(RCC_Clocks.HCLK_Frequency / 2000))
  {
    for (;;) /* Capture error */
    {
    }
  }

}

/**
  ******************************************************************************
  * @brief Short local routine to check 1 sec tick. A tick flag is set for DTO.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_Check_1sec_Tick(void)
{
  if (TSL_Tick_100ms >= 10)
  {
    TSL_Tick_100ms -= 10;
    TSL_Tick_Flags.b.DTO_1sec = 1;  // Tick Flag for Max On Duration set every second.
  }
}


/**
  ******************************************************************************
  * @brief Short local routine to check 100 ms tick.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_Check_100ms_Tick(void)
{
  if (TSL_Tick_10ms >= 10)
  {
    TSL_Tick_10ms -= 10;
    TSL_Tick_100ms++;
    TSL_Timer_Check_1sec_Tick();
  }
}


/**
  ******************************************************************************
  * @brief Short local routine to check 10 ms tick.
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_Check_10ms_Tick(void)
{
  if (TSL_Tick_Base >= TICK_FACTOR_10MS)
  {
    TSL_Tick_Base -= TICK_FACTOR_10MS;
    TSL_Tick_10ms++;
    TSL_TickCount_ECS_10ms++;   // Tick Flag for Drift increment every 10 ms.
    TSL_Timer_Check_100ms_Tick();
  }
}


/**
  ******************************************************************************
  * @brief Interrupt Service Routine for HW Timer used as timebase.
  * It manages the timer ticks for the whole application (IT every 500탎).
  * @param None
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_ISR(void)
{

  TSL_Tick_Base++;

  // Generic timing delay
  if (TSL_TimingDelay > 0)
  {
    TSL_TimingDelay--;
  }

  TSL_Timer_Check_10ms_Tick();

  if (TSL_Tick_Flags.b.User1_Start_100ms) /* Application request */
  {
    TSL_Tick_Flags.b.User1_Start_100ms = 0;
    TSL_Tick_Flags.b.User1_Flag_100ms = 0;
    TSL_Tick_User1 = (TICK_FACTOR_10MS * 10);
  }

  if (TSL_Tick_Flags.b.User2_Start_100ms) /* Application request */
  {
    TSL_Tick_Flags.b.User2_Start_100ms = 0;
    TSL_Tick_Flags.b.User2_Flag_100ms = 0;
    TSL_Tick_User2 = (TICK_FACTOR_10MS * 10);
  }

  if (TSL_Tick_User1 > 0)
  {
    TSL_Tick_User1--;
    if (TSL_Tick_User1 == 0)
    {
      TSL_Tick_Flags.b.User1_Flag_100ms = 1; /* Give information to Application */
    }
  }

  if (TSL_Tick_User2 > 0)
  {
    TSL_Tick_User2--;
    if (TSL_Tick_User2 == 0)
    {
      TSL_Tick_Flags.b.User2_Flag_100ms = 1; /* Give information to Application */
    }
  }

#if TIMER_CALLBACK
  USER_TickTimerCallback();
#endif

}


/**
  ******************************************************************************
  * @brief Modify the tick values for specific cases when the H/W timer doesn't
  * work (halt, ...).
  * @param[in] adjust_delay Time to add to the ticks (unit is 500탎).
  * @retval None
  ******************************************************************************
  */
void TSL_Timer_Adjust(uint32_t adjust_delay)
{

  disableInterrupts();

  do
  {
    if (adjust_delay > TICK_FACTOR_10MS)
    {
      TSL_Tick_Base += TICK_FACTOR_10MS;
      adjust_delay -= TICK_FACTOR_10MS;
      TSL_Timer_Check_10ms_Tick();
    }
    else
    {
      TSL_Tick_Base++;
      adjust_delay--;
      TSL_Timer_Check_10ms_Tick();
    }
  }
  while (adjust_delay);

  enableInterrupts();

}

/**
  * @brief Generic wait routine using the Systick as clock base
  * @param[in] delay Wait delay (unit is 500탎)
  * @retval None
  */
void TSL_Wait(__IO uint32_t delay)
{
  TSL_TimingDelay = delay;
  while (TSL_TimingDelay != 0); // Variable decrement by Systick interrupt
}

/* Public functions ----------------------------------------------------------*/

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
