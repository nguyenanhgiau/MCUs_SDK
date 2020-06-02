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
#include "Button.h"
#include "led.h"
#include "prj_options.h"
#include "app_main.h"
#include "dbg.h"
#include "portable.h"
#include "serial.h"

/* Private defines -----------------------------------------------------------*/
/* Private variable ----------------------------------------------------------*/
uint8 u8ButtonTest;
uint8 u8SerialTest;
uint8 u8LedTest;
/* Private function prototypes -----------------------------------------------*/
static void     BUTTON_vOpen(void);
static bool     BUTTON_bRead(void);

static void APP_vInitialise(void);

static void uart_initialize(void);
static void uart_drv_send(uint8_t u8TxByte);
static uint8_t uart_drv_receive(void);
static void uart_start_send(void);
static void uart_stop_send(void);
static void uart_start_receive(void);
static void uart_stop_receive(void);

static void led_initialize(void);
static void led_set_state(bool bState);

void main(void)
{
    /* configure private for unique mcu
  * such as:
  * - system clock 
  * - uart for debug module 
  * - time base 
  * - watchdog timer
  * */
  PORTABLE_vInit();

  /* Initialize debugger module */
//   DBG_vInit(uart_initialize, uart_drv_send, uart_drv_receive);
//   DBG_vPrintf(TRUE, "*%s DEVICE RESET %s*\n", "***********", "***********");

  /* common initialize */
  APP_vSetUpHardware();

  APP_vInitResources();

  APP_vInitialise();
  
  SERIAL_eWrite(u8SerialTest, "Hello\n");

  LED_tsEffect sEffect = {
      .eEffect = E_LED_EFFECT_FLASH,
      .u16TimeOn = 15,
      .u16TimeOff = 15,
      .u8Flash = 3,
      .u8Loop = 10,
      .u16Period = 200,
  };
  LED_eStartEffect(u8LedTest, &sEffect);

  /* Infinite loop */
  APP_vMainLoop();  
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
static void BUTTON_vOpen(void)
{
    GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT);
}

static bool BUTTON_bRead(void)
{
    return (bool)GPIO_ReadInputPin(GPIOA, GPIO_PIN_1);
}

static void uart_drv_send(uint8_t u8TxByte)
{
    UART1_SendData8(u8TxByte);
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

static uint8_t uart_drv_receive(void)
{
    while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
    return UART1_ReceiveData8();
}

static void uart_initialize(void)
{
    /* Deinit USART */
    UART1_DeInit();
    /* UART1 configuration ------------------------------------------------------*/
    UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    /* Start USART */
    UART1_Cmd(ENABLE);
}

static void uart_start_send(void)
{
    UART1_ITConfig(UART1_IT_TXE, ENABLE);
}

static void uart_stop_send(void)
{
    UART1_ITConfig(UART1_IT_TXE, DISABLE);
}

static void uart_start_receive(void)
{
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
}

static void uart_stop_receive(void)
{
    UART1_ITConfig(UART1_IT_RXNE_OR, DISABLE);
}

static void led_initialize(void)
{
    GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
}

static void led_set_state(bool bState)
{
    if (!bState)
    {
        GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
    }
    else
    {
        GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
    }
}

static void APP_vInitialise(void)
{
    // BUTTON_eOpen(&u8ButtonTest, BUTTON_vOpen, NULL, BUTTON_bRead, true);

    SERIAL_tsSerial sSerial = {
        .pfOpen = &uart_initialize,
        .pfSend = &uart_drv_send,
        .pfReceive = &uart_drv_receive,
        .pfStartSend = &uart_start_send,
        .pfStopSend = &uart_stop_send,
        .pfStartReceive = &uart_start_receive,
        .pfStopReceive = &uart_stop_receive
    };
    SERIAL_eOpen(&u8SerialTest, &sSerial);
    SERIAL_vStartReceive(u8SerialTest);

    LED_tsLed sLed = {
        .bState = FALSE,
        .pfOpen = &led_initialize,
        .pfSetOnOff = &led_set_state
    };
    LED_eOpen(&u8LedTest, &sLed);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
