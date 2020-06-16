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
* @file:      watchdog.h
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of Serial Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WATCHDOG_H_
#define WATCHDOG_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "prj_options.h"

/* Exported Define -----------------------------------------------------------*/

#ifndef WWDG_TIME_RESET
#define WWDG_TIME_RESET     (5)
#endif

/* Exported Typedefs ---------------------------------------------------------*/
typedef void (*WDG_ptfStart)(void);
typedef void (*WDG_ptfStop)(void);
typedef void (*WDG_ptfRestart)(void);
typedef bool (*WDG_ptfResetEvent)(void);

typedef struct
{
    uint16          u16Time;
    WDG_ptfStart    pfStart;
    WDG_ptfStop     pfStop;
    WDG_ptfRestart  pfRestart;
    WDG_ptfResetEvent pfResetEvent;
}WATCHDOG_tsWDG;

typedef enum
{
    E_WATCHDOG_OK,
    E_WATCHDOG_FAIL
}WATCHDOG_teStatus;
/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
WATCHDOG_teStatus WATCHDOG_vInit(WATCHDOG_tsWDG *psWDG);
void WATCHDOG_vStart(void);
void WATCHDOG_vStop(void);
void WATCHDOG_vRestart(void);
bool WATCHDOG_bResetEvent(void);
/* External Variable Declarations --------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /*WATCHDOG_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
