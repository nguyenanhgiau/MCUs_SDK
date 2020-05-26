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
* @file:      Serial.h
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of Serial Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SERIAL_H_
#define SERIAL_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "prj_options.h"
/* Exported Define -----------------------------------------------------------*/

/* Exported Typedefs ---------------------------------------------------------*/
typedef void (*SERIAL_ptfOpen)(void);
typedef void (*SERIAL_ptfClose)(void);
typedef void (*SERIAL_ptfSend)(void);
typedef bool (*SERIAL_ptfReceive)(uint8*);
typedef void (*SERIAL_ptfStartSend)(void);
typedef void (*SERIAL_ptfStopSend)(void);
typedef void (*SERIAL_ptfStartReceive)(void);
typedef void (*SERIAL_ptfStopReceive)(void);

typedef struct
{
    uint32              u32Baud;
    SERIAL_ptfOpen      pfOpen;
    SERIAL_ptfClose     pfClose;
    SERIAL_ptfSend      pfSend;
    SERIAL_ptfReceive   pfReceive;
}SERIAL_tsSerial;

/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
/* External Variable Declarations --------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /*SERIAL_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
