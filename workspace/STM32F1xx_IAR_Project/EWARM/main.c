/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
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
#include "port_mcu.h"

/** @addtogroup STM32F0xx_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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

static void led_initialize(void);
static void led_set_state(void *pvParam);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */ 
  PORTABLE_vInit();
  
  /* Initialize debugger module */
  DBG_vInit(uart_initialize, uart_drv_send, uart_drv_receive);
  DBG_vPrintf(TRUE, "*%s DEVICE RESET %s*\n", "***********", "***********");
  
  /* common initialize */
  APP_vSetUpHardware();

  APP_vInitResources();

  APP_vInitialise();

  /* Infinite loop */
  APP_vMainLoop();
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
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
  /* GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure pin in inout pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

static bool BUTTON_bRead(void)
{
  return (bool)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
}

static void uart_drv_send(uint8_t u8TxByte)
{
  /* write a character to the USART */
  USART_SendData(USART1, (uint8_t) u8TxByte);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

static uint8_t uart_drv_receive(void)
{
  /* Loop until the end of receive */
  while (USART_GetFlagStatus(USART1, USART_IT_RXNE) == RESET);

  /* get data from USART */
  return (uint8)USART_ReceiveData(USART1);
}

static void uart_initialize(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef USART_InitStructure;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enalbe clock for USART1, AFIO and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | 
                           RCC_APB2Periph_GPIOA, ENABLE);
                            
    /* GPIOA PIN9 alternative function Tx */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* GPIOA PIN9 alternative function Rx */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
 
    /* Baud rate 115200, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    USART_InitStructure.USART_BaudRate = 115200;   
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}

static void led_initialize(void)
{
  /* GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure pin in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
}

static void led_set_state(void *pvParam)
{
  bool bState = (bool*)pvParam;
  GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)!bState);
}

static void APP_vInitialise(void)
{
    BUTTON_eOpen(&u8ButtonTest, BUTTON_vOpen, NULL, BUTTON_bRead);

    LED_tsLed sLed = {
        .bState = FALSE,
        .pfOpen = &led_initialize,
        .pfSetState = &led_set_state
    };
    LED_eOpen(&u8LedTest, &sLed);
}
/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
