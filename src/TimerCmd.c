/**************************************************************************
* File Name         TimerCmd.c
* Description       
*				          
* Date				          Name(s)						          Action
* October 5, 2021		    Jaskaran K. & David C.			First Implementation
***************************************************************************/

/**************************************************************************
---------------------------- LIBRARY DEFINITIONS --------------------------
***************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "common.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_pwr.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "stm32f4xx_hal_pwr_ex.h"
#include "stm32f4xx_hal_cortex.h"

/**************************************************************************
--------------------------- PRECOMPILER DEFINITIONS -----------------------
***************************************************************************/
#define MICROSECONDS 0
#define MILISECONDS 1

/**************************************************************************
------------------------------- VARIABLE TYPES ----------------------------
***************************************************************************/
typedef struct{
  uint32_t timeout;     // virtual counter timeout
  uint32_t current;     // virtual counter current value
  uint8_t flag;         // ready flag
  uint8_t repetitive;   // count 1 single time or undefinetely
}V_TIMER;

// Linked list node structure
typedef struct{
  V_TIMER *vTimerPtr;
  struct V_NODE *nextNode;
}V_NODE;

/**************************************************************************
---------------------------- GLOBAL VARIABLES --------------------------
***************************************************************************/
TIM_HandleTypeDef htim3;  // Timer Handler 
V_NODE *head = NULL;      // head of linked list

/**************************************************************************
------------------------ OWN FUNCTION DEFINITIONS -------------------------
***************************************************************************/

/*--------------------------------------------------------------------------
*	Name:			    TimerInit
*	Description:	
*	Parameters:		void
*
*	Returns:		  ret: CmdReturnOk = 0 if Okay.
---------------------------------------------------------------------------*/
ParserReturnVal_t TimerInit()
{ 
  uint16_t timebase = 0; //input in microseconds
  uint16_t tim3Prescaler = 1; // 0xFFFF 84
  uint16_t tim3Period = 1; //0xFFFF 65535

  if (!fetch_uint16_arg(&timebase))
    printf("Please enter (1) for ms or (0) for us (default).\n");
  else{
    if(timebase == MILISECONDS){
      tim3Prescaler = 1000;
      tim3Period = 84;
    }
    else{
      tim3Prescaler = 1;
      tim3Period = 84;
    }
    // Set up the timer
    __HAL_RCC_TIM3_CLK_ENABLE();
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = tim3Prescaler;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = tim3Period;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
      printf("Error 1 initializing the timer\n");
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
      printf("Error 2 initializing the timer\n");
    }
    HAL_NVIC_SetPriority((IRQn_Type) TIM3_IRQn, (uint32_t) 0, (uint32_t) 1);
    HAL_NVIC_EnableIRQ((IRQn_Type) TIM3_IRQn);
    // Start timer
    HAL_TIM_Base_Start_IT(&htim3);

  }
  return CmdReturnOk;
}

// Function called when Timer3 sets an interrupt
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

// Callback whenever timer updates/overflows

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

}

// MACRO: Add new command to help menu
ADD_CMD("timerinit", TimerInit,"\t\tInitializes timer.")

