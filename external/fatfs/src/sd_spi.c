
/*
********************************************************************************
* Copyright of anhgiau (nguyenanhgiau1008@gmail.com)
* Follow this coding style used at:
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
* @file:      sd_spi.c
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.2
* @date:      8/15/2018
* @brief:     Implementation of Platfrom Interface SD Card
********************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "spi_drv.h"

/*
 * Code is split into 3 parts:
 * - generic SPI code: adapt for your MCU
 * - sd card code, with crc7 and crc16 calculations
 *   there's lots of it, but it's simple
 * - fatfs interface. If you use anything else, look here for
 *   interface to SD card code
 */
 
/* Private Typedef -----------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
#define SPI_SD		SPI1
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Private Variables Declarations --------------------------------------------*/

static volatile
DSTATUS Stat = STA_NOINIT;	/* Physical drive status */

static volatile
UINT Timer1, Timer2;	/* 1kHz decrement timer stopped at zero (disk_timerproc()) */

static
BYTE CardType;			/* Card type flags */

/* Private Function Definitions ----------------------------------------------*/

static void spi_init(void)
{
#if MCU_USED == STM32F10x
	GPIO_InitTypeDef gpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);

	gpio.GPIO_Pin = GPIO_Pin_4;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio);
	
	SPI_InitTypeDef spi;
	/* ^ with /2 APB1 this will be 15mhz/234k at 60mhz
	 * 18/281 at 72. which is ok, 100<x<400khz, and <25mhz */
	SPI_Cmd(SPI_SD, DISABLE);

	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;

	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(SPI_SD, &spi);

	SPI_Cmd(SPI_SD, ENABLE);
#endif /**/
}

static 
BYTE xchg_spi(BYTE data)
{
	/* RXNE always happens after TXE, so if this function is used
	 * we don't need to check for TXE */
#if MCU_USED == STM32F10x
	SPI_SD->DR = data;
	while ((SPI_SD->SR & SPI_I2S_FLAG_RXNE) == 0);
	return SPI_SD->DR;
#endif 
}

/* crc helpers */
static uint8_t crc7_one(uint8_t t, uint8_t data)
{
	int i;
	const uint8_t g = 0x89;

	t ^= data;
	for (i=0; i<8; i++) {
		if (t & 0x80)
			t ^= g;
		t <<= 1;
	}
	return t;
}

uint8_t crc7(const uint8_t *p, int len)
{
	int j;
	uint8_t crc = 0;
	for (j=0; j<len; j++)
		crc = crc7_one(crc, p[j]);

	return crc>>1;
}

/* http://www.eagleairaust.com.au/code/crc16.htm */
static uint16_t crc16_ccitt(uint16_t crc, uint8_t ser_data)
{
	crc  = (uint8_t)(crc >> 8) | (crc << 8);
	crc ^= ser_data;
	crc ^= (uint8_t)(crc & 0xff) >> 4;
	crc ^= (crc << 8) << 4;
	crc ^= ((crc & 0xff) << 4) << 1;

	return crc;
}

uint16_t crc16(const uint8_t *p, int len)
{
	int i;
	uint16_t crc = 0;

	for (i=0; i<len; i++)
		crc = crc16_ccitt(crc, p[i]);

	return crc;
}

/*** sd functions - on top of spi code ***/
static void sd_cmd(BYTE cmd, DWORD arg)
{
	BYTE n, res;
	
	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}
	
	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect();
		if (!select()) return 0xFF;
	}
	
	/* Send command packet */
	xchg_spi(0x40 | cmd);				/* Start + command index */
	xchg_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xchg_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xchg_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xchg_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xchg_spi(n);
	
	/* Receive command resp */
	if (cmd == CMD12) xchg_spi(0xFF);	/* Diacard following one byte when CMD12 */
	n = 10;								/* Wait for response (10 bytes max) */
	do {
		res = xchg_spi(0xFF); 
	} while ((res & 0x80) && --n);
	
	return res;							/* Return received response */
}

/* Function Definitions ------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE drv)
{
	BYTE n, cmd, ty, ocr[4];
	
	if (drv) return STA_NOINIT;			/* Supports only drive 0 */
	spi_init();
	
	if (Stat & STA_NODISK) return Stat;	/* Is card existing in the soket? */
	
	/*Config SPI Slow*/
	
	for (n = 10; n; n--) xchg_spi(0xFF);	/* Send 80 dummy clocks */
	
	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {
		Timer1 = 1000;
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			for (n = 0; n < 4 ; n++) ocr[n] = xchg_spi(0xFF);	/* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {
				while (Timer1 && send_cmd(ACMD41, 1UL << 30));
			}
		}
	}
}
