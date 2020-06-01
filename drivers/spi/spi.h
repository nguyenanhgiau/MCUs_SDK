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
* @file:      SPI.h
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of Serial Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_H_
#define SPI_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "prj_options.h"

/* Exported Define -----------------------------------------------------------*/
/* Exported Typedefs ---------------------------------------------------------*/
typedef void (*SPI_ptfOpen)(void);
typedef void (*SPI_ptfClose)(void);
typedef void (*SPI_ptfSend)(void);

typedef struct
{
    SPI_ptfOpen         pfOpen;
    SPI_ptfClose        pfClose;
    SPI_ptfSend         pfSend;
}SPI_tsSpi;

typedef enum
{
    E_SPI_OK,
    E_SPI_FAIL,
}SPI_teStatus;
/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
SPI_teStatus SPI_eInit(SPI_tsSpi *psSpis, const uint8 u8NumSpis);
SPI_teStatus SPI_eOpen(uint8 *pu8SpiIndex, SPI_tsSpi *psSpi);
SPI_teStatus SPI_eClose(uint8 u8SpiIndex);
void SPI_vSelectChip(uint8 u8SpiIndex, bool bSelect);
SPI_teStatus SPI_eBeginTransaction(uint8 u8SpiIndex);
uint8 SPI_u8Transfer(uint8 u8SpiIndex, uint8 u8Byte);
SPI_teStatus SPI_eEndTransaction(uint8 u8SpiIndex);
/* External Variable Declarations --------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /*SPI_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
