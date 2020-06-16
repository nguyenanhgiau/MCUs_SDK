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
#include "port_mcu.h"
#include "watchdog.h"

#include "port_fatfs.h"
/* Private defines -----------------------------------------------------------*/
#define WINDOW_VALUE        97
#define COUNTER_INIT       104
/* Private variable ----------------------------------------------------------*/
uint8 u8ButtonTest;
uint8 u8SerialTest;
uint8 u8LedTest;

//static DSTATUS        fr;    /* Result code */
//static FATFS          fatfs;          /* File system object */
//static FIL            fil;            /* File object */
/* Private function prototypes -----------------------------------------------*/
static void     BUTTON_vOpen(void);
static bool     BUTTON_bRead(void);

static void APP_vInitialise(void);

static void uart_initialize(void);
static void uart_drv_send(uint8_t u8TxByte);
static uint8_t uart_drv_receive(void);

static void led_initialize(void);
static void led_set_state(void *pvParam);

static void init_spi (void);
static void spi_select(bool bSelect);
static uint8 spi_xchg(uint8 u8Byte);

static void watchdog_start(void);
static void watchdog_restart(void);
static bool watchdog_get_reset(void);

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
  DBG_vInit(uart_initialize, uart_drv_send, uart_drv_receive);
  DBG_vPrintf(TRUE, "*%s DEVICE RESET %s*\n", "***********", "***********");

  /* common initialize */
  APP_vSetUpHardware();

  APP_vInitResources();

  APP_vInitialise();
  
//  uint8_t buff[100];
//  fr = f_mount(&fatfs, "", 0);
//  fr = f_open(&fil, "test.txt", FA_READ);
//  if ( fr == FR_OK || fr == FR_EXIST) {
//    UINT        u8Read;
//    fr = f_read(&fil, buff, 100, &u8Read);
//      
//    /* Close the file */
//    f_close(&fil);
//  }

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
    GPIO_Init(GPIOC, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
}

static bool BUTTON_bRead(void)
{
    return (bool)GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6);
}

static void uart_drv_send(uint8_t u8TxByte)
{
    /* Write a character to the USART */
    USART_SendData8(USART1, u8TxByte);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

static uint8_t uart_drv_receive(void)
{
    /* Loop until the Read data register flag is SET */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData8(USART1);
}

static void uart_initialize(void)
{
    /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - USART Clock disabled
    */
    /* Enable USART clock */
    CLK_PeripheralClockConfig((CLK_Peripheral_TypeDef)CLK_Peripheral_USART1, ENABLE);
  
    /* Configure USART Tx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);

    /* Configure USART Rx as alternate function push-pull  (software pull up)*/
    GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);

    /* USART configuration */
    USART_Init(USART1, (uint32_t)115200,
               USART_WordLength_8b,
               USART_StopBits_1,
               USART_Parity_No,
               (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));
}

static void led_initialize(void)
{
    GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Fast);
}

static void led_set_state(void *pvParam)
{
    bool *pbOn = (bool*)pvParam;
    GPIO_WriteBit(GPIOB, GPIO_Pin_4, (BitAction)(*pbOn));
}

static
void init_spi (void)
{
  /* sFLASH_SPI Periph clock enable */
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);

  /* Set the MOSI,MISO and SCK at high level */
  GPIO_ExternalPullUpConfig(GPIOB, GPIO_Pin_5 | \
                            GPIO_Pin_7 | GPIO_Pin_6, ENABLE);

  /* Configure FLASH_CS as Output push-pull, used as Flash Chip select */
  GPIO_Init(GPIOD, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Slow);

  /* SPI configuration */
  SPI_Init(SPI1, SPI_FirstBit_MSB, SPI_BaudRatePrescaler_4, SPI_Mode_Master,
           SPI_CPOL_Low, SPI_CPHA_1Edge, SPI_Direction_2Lines_FullDuplex,
           SPI_NSS_Soft, 0x07);

  /* Enable SPI  */
  SPI_Cmd(SPI1, ENABLE);
}

static
void spi_select(bool bSelect)
{
  if (bSelect)
  {
    GPIO_SetBits(GPIOD, GPIO_Pin_4);
  }
  else
  {
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
  }
}

static
uint8 spi_xchg(uint8 u8Byte)
{
  SPI1->DR = u8Byte;
  while ((SPI1->SR & 0x83) != 0x03);
  return (SPI1->DR);
}

static void watchdog_start(void)
{
  /* WWDG configuration: WWDG is clocked by SYSCLK = 2MHz */
  /* WWDG timeout is equal to 251,9 ms */
  /* Watchdog Window = (COUNTER_INIT - 63) * 1 step
                     = 41 * (12288 / 2Mhz)
                     = 251,9 ms
  */
  /* Non Allowed Window = (COUNTER_INIT - WINDOW_VALUE) * 1 step
                        = (104 - 97) * 1 step
                        =  7 * 1 step 
                        =  7 * (12288 / 2Mhz) 
                        =  43.008 ms
   */
  /* So the non allowed window starts from 0.0 ms to 43.008 ms
  and the allowed window starts from 43.008 ms to 251,9 ms
  If refresh is done during non allowed window, a reset will occur.
  If refresh is done during allowed window, no reset will occur.
  If the WWDG down counter reaches 63, a reset will occur. */
  WWDG_Init(104, 97);
}

static void watchdog_restart(void)
{
  if ((WWDG_GetCounter() & 0x7F) < 97)
  {
    /* Refresh WWDG counter during allowed window so no MCU reset will occur */
    WWDG_SetCounter(107);
  }
}

static bool watchdog_get_reset(void)
{
  /* Check if the MCU has resumed from WWDG reset */
  if (RST_GetFlagStatus(RST_FLAG_WWDGF) != RESET)
  {
    /* Clear WWDGF Flag */
    RST_ClearFlag(RST_FLAG_WWDGF);
    return TRUE;
  }
  else
  {
    return FALSE;
  }
  
}

static void APP_vInitialise(void)
{
    WATCHDOG_tsWDG sWDG = {
        .u16Time = 104,
        .pfStart = &watchdog_start,
        .pfRestart = &watchdog_restart,
        .pfResetEvent = &watchdog_get_reset};
    WATCHDOG_vInit(&sWDG);

    BUTTON_eOpen(&u8ButtonTest, BUTTON_vOpen, NULL, BUTTON_bRead);

    LED_tsLed sLed = {
        .bState = FALSE,
        .pfOpen = &led_initialize,
        .pfSetState = &led_set_state
    };
    LED_eOpen(&u8LedTest, &sLed);
    
    SPI_tsSpi sSPI = {
        .pfOpen = init_spi,
        .pfSelect = spi_select,
        .pfXchgByte = spi_xchg
    };
    
    FATfs_eInit(&sSPI);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
