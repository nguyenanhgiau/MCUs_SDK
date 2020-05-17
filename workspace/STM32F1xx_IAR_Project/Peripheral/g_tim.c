/**
  ******************************************************************************
  * @file:      tim.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      5/31/2018
  * @brief:     Implementation of driver timer
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WE SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
  * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
  * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
  * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
 #include "g_tim.h"
 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static volatile uint32_t g_timebase_tick = 0;
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/**
*/
int g_timebase_init(void)
{
  /* reset timebase */
  g_timebase_tick = 0;
  /* Init interrupt timer or system_tick*/
  if(!SysTick_Config(SystemCoreClock / 1000)){  
    return 0;
  }
  else{
    return -1;
  }
}

/**
*/
uint32_t g_timebase_get(void)
{
  return g_timebase_tick;
}

/**
*/
void g_timebase_delay(uint32_t ms)
{
  uint32_t delay = g_timebase_get() + ms;
  while(delay > g_timebase_get())
  {}
}

/**
*/
void g_timebase_handle(void)
{
  /* Clear flag interrupt (if any)*/
  
  /* increase timebase_tick */
  g_timebase_tick++;
}

/**
*/
int g_timer_create(timer_t *t, uint32_t period)
{
  if(t == '\0'){
    return TIM_ERROR_NULL_PTR;
  }
  else{
    t->period = period;
    t->start = g_timebase_get();
    return TIM_ERROR_NONE;
  }
}

/**
*/
int g_timer_restart(timer_t *t)
{
  if(t == '\0'){
    return TIM_ERROR_NULL_PTR;
  }
  else{
    t->start = g_timebase_get();
    return TIM_ERROR_NONE;
  }
}

/**
*/
uint32_t g_timer_expired(timer_t *t)
{
  uint32_t rslt = g_timebase_get() - t->start;
  if(rslt >= t->period){
    return 0;
  }
  else{
    return (rslt - t->period);
  }
}

 