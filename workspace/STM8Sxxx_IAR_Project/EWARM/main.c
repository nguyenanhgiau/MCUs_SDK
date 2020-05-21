/**
******************************************************************************
* @file    Project/main.c 
* @author  MCD Application Team
* @version V2.2.0
* @date    30-September-2014
* @brief   Main program body
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/ 


/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "Queue.h"
#include "Timer.h"
#include "Button.h"

/* Private defines -----------------------------------------------------------*/
#define TX_QUEUE_SIZE                                               150
#define RX_QUEUE_SIZE                                               150
#define BT_QUEUE_SIZE                                               8
/* Private variable ----------------------------------------------------------*/
TIMER_tsTimer asTimers[3];
BUTTON_tsButton asButtons[3];
uint8           u8Button_1;

tsQueue           APP_msgSerialRx;
tsQueue           APP_msgSerialTx;
tsQueue           APP_msgButtonEvents;

uint8             au8AtRxBuffer [ RX_QUEUE_SIZE ];
uint8             au8AtTxBuffer [ TX_QUEUE_SIZE ];
BUTTON_tsEvent    asButtonMsg [BT_QUEUE_SIZE];
/* Private function prototypes -----------------------------------------------*/
static void timebase_initialize(void);
static void uart_initialize(void);
static void     BUTTON_vOpen(void);
static bool     BUTTON_bRead(void);

void main(void)
{
  /* Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  /* Initialize debugger module */
  uart_initialize();

  /*test send by Queue */
  uint8 *a = "Hello\n";
  uint8 i;
  for (i=0; i<6; i++)
  {
    QUEUE_bSend(&APP_msgSerialTx, &a[i]);
  }
  UART1_ITConfig(UART1_IT_TXE, ENABLE);//start send
  
  /* Initialize timer platform */
  TIMER_eInit(asTimers, sizeof(asTimers) / sizeof(TIMER_tsTimer));
  timebase_initialize();
  /* Create timer */
  
  /*Initialize modules for application */
  BUTTON_eInit(asButtons, sizeof(asButtons) / sizeof(BUTTON_tsButton));
  BUTTON_eOpen(&u8Button_1, BUTTON_vOpen, NULL, BUTTON_bRead, true);
  QUEUE_vCreate( &APP_msgButtonEvents,       BT_QUEUE_SIZE,          sizeof ( BUTTON_tsEvent ),                  (uint8*)asButtonMsg );
  
  /* Infinite loop */
  while (1)
  {
    /* call timer task handle soft timer */
    TIMER_vTask();
    
    /*TODO: add watchdog restart */
    
    /*TODO: add main task */
    BUTTON_tsEvent sButtonEvent;
    if (QUEUE_bReceive(&APP_msgButtonEvents, &sButtonEvent))
    {
        switch (sButtonEvent.eState)
        {
        case E_BUTTON_STATE_RELEASE:
            break;
        
        case E_BUTTON_STATE_PRESS:
            break;

        case E_BUTTON_STATE_HOLD_ON:
            break;
        }
    }
    
    /*TODO: add task managenment power */
  }
  
}

#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/* Private functions ---------------------------------------------------------*/
static void timebase_initialize(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  
  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

static void BUTTON_vOpen(void)
{
    GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
}

static bool BUTTON_bRead(void)
{
    return (bool)GPIO_ReadInputPin(GPIOA, GPIO_PIN_1);
}

static void uart_initialize(void)
{
    /* Deinit USART */
    UART1_DeInit();
    /* UART1 configuration ------------------------------------------------------*/
    UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    /*create buffer to storage data of UART*/
    QUEUE_vCreate( &APP_msgSerialTx,       TX_QUEUE_SIZE,          sizeof ( uint8 ),                  (uint8*)au8AtTxBuffer );
    QUEUE_vCreate( &APP_msgSerialRx,       RX_QUEUE_SIZE,          sizeof ( uint8 ),                  (uint8*)au8AtRxBuffer );
    /* Start USART */
    UART1_Cmd(ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    
    /* Enable general interrupts */
    enableInterrupts();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
