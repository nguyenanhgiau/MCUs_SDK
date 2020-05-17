/**
  ******************************************************************************
  * Copyright of anhgiau (nguyenanhgiau1008@gmail.com)
  * This coding style used at AT&T and Sun in writing C programs.
  * visit: http://www.csd.uoc.gr/~zakkak/hy255/reading/c_coding_style.pdf
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  * - Redistributions of source code must retain the above copyright
  * notice, this list of conditions and the following disclaimer.
  * - Redistributions in binary form must reproduce the above copyright
  * notice, this list of conditions and the following disclaimer in the
  * documentation and/or other materials provided with the distribution.
  * - Development or contribute must follow this coding style.
  *
  * @file:      g_spi.c
  * @author:    anhgiau (nguyenanhgiau1008@gmail.com)
  * @version:   1.0.1
  * @date:      6/4/2018
  * @brief:     Implementation of SPI Driver
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "g_spi.h"
#include "g_tim.h"
 
/* Private Typedef -----------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
#define SPI_CS_LOW()     GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_CS_HIGH()    GPIO_SetBits(GPIOA, GPIO_Pin_4)
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Private Variables Declarations --------------------------------------------*/
/* Private Function Definitions ----------------------------------------------*/
static uint8_t g_spi_wr(uint8_t data)
{
  uint8_t tmp = 0x00;
  /* Wait for SPIy Tx buffer empty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* Send SPIz data */
  SPI_I2S_SendData(SPI1, data);
  
  /* Wait to receive a byte */ 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */    
  tmp = SPI_I2S_ReceiveData(SPI1);
  return tmp;
}
/* Function Definitions ------------------------------------------------------*/

int
g_spi_init(void)
{
  /* Enable GPIO clock for SPI */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  /* Enable SPIy Periph clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  /* 1st phase: SPIy Master and SPIz Slave */
  /* GPIO configuration ------------------------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Configure SPIy pins: SCK, MISO and MOSI ---------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure NSS pin as Output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  SPI_CS_HIGH();
  
  /* SPIy Config -------------------------------------------------------------*/
  SPI_InitTypeDef   SPI_InitStructure;
  /*Reset SPI Interface*/
  SPI_I2S_DeInit(SPI1);
  /*SPI configuration*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 10;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* Enable SPI */
  SPI_Cmd(SPI1, ENABLE);
  
  return SPI_ERROR_NONE;
}

int g_spi_write_read(uint8_t *data, uint32_t len)
{
  uint32_t index = 0;
  SPI_CS_LOW();
  
  while(index < len){
    g_spi_wr(data[index++]);
  }
  
  SPI_CS_HIGH();
  return 0;
}
