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
* @file:      port_fatfs.h
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of PORT FATFS
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PORT_FATFS_H_
#define PORT_FATFS_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "diskio.h"
#include "ff.h"
#include "spi.h"
/* Exported Define -----------------------------------------------------------*/
/* Exported Typedefs ---------------------------------------------------------*/
typedef enum
{
    E_FATFS_OK,
    E_FATFS_FAIL
}FATfs_teStatus;
/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
FATfs_teStatus FATfs_eInit(SPI_tsSpi *psSPI);
/* External Variable Declarations --------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /*PORT_FATFS_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
