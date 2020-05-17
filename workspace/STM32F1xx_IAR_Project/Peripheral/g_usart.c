/**
  ******************************************************************************
  * @file:      g_usart.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      5/31/2018
  * @brief:     Implementation of Driver UART
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
#include "g_usart.h"
#include "ringbuf.h"
 
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static ringbuf_t USART1_TXRingBuff;
static ringbuf_t USART1_RXRingBuff;
static uint8_t USART1_TXBuff[UART_TX_MAX_BUFFER];
static uint8_t USART1_RXBuff[UART_RX_MAX_BUFFER];
/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/* Private function ----------------------------------------------------------*/

/**
*/
int g_usart_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  
  /* Enable USART1 & GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Configure USART1 Tx as alternate function push-pull */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Configure USART1 Rx as input floating */
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Init ringbuf buffer */
  ringbuf_init(&USART1_TXRingBuff, USART1_TXBuff, UART_TX_MAX_BUFFER);
  ringbuf_init(&USART1_RXRingBuff, USART1_RXBuff, UART_RX_MAX_BUFFER);
  
  /* Enable IT RXNE, use to get data bytes */
//  USART_ITConfig( USART1, USART_IT_TXE, ENABLE );
  USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
  
  /* Configure USARTx */
  USART_InitStruct.USART_BaudRate = UART_DEF_BAUDRATE;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART1, &USART_InitStruct);
  /* USART enable */
  USART_Cmd(USART1, ENABLE);
  return UART_ERROR_NONE;
}

/**
*/
int
g_usart_write(uint8_t data)
{
  if(ringbuf_putc(&USART1_TXRingBuff, data) != RINGBUF_ERROR_NONE)
  {
    return UART_ERROR_TX;
  }
  //start send
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  return UART_ERROR_NONE;
}

/**
* @brief USART1 write string
*/
int
g_usart_write_string(const uint8_t *str)
{
  while(*str)
  {
    if(ringbuf_putc(&USART1_TXRingBuff, *str++) != RINGBUF_ERROR_NONE)
    {
      return UART_ERROR_TX;
    }
  }
  //start send
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  return UART_ERROR_NONE;
}

/**
* @brief USART1 read 1 byte from buffer
*/
int g_usart_read(uint8_t *rs)
{
  if(ringbuf_getc(&USART1_RXRingBuff, rs) != RINGBUF_ERROR_NONE){
    return UART_ERROR_RX;
  }
  else {
    return UART_ERROR_NONE;
  }
}

/**
* @brief USART1 read len byte
*/
int g_usart_read_len(uint8_t *buf, uint32_t len)
{
  for(uint32_t i=0; i<len; i++)
  {
    if (ringbuf_getc(&USART1_RXRingBuff, buf++) != RINGBUF_ERROR_NONE){
      return UART_ERROR_RX;
    }
  }
  return UART_ERROR_NONE;
}

/**
* @brief Function handle interrupt USART1
*/
void
g_usart_irq_handle(uint8_t type_irq)
{
  unsigned char c = 0xff;
  if(USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET ){
    c = USART_ReceiveData(USART1);
    ringbuf_putc(&USART1_RXRingBuff, c);
  }
  else if(USART_GetITStatus( USART1, USART_IT_TXE ) != RESET ){
    if(ringbuf_getc(&USART1_TXRingBuff, &c) == 0)
      USART_SendData(USART1, c);
    else
      USART_ITConfig( USART1, USART_IT_TXE, DISABLE );
  }
}