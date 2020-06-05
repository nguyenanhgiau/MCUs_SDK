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
* @file:      led.h
* @author:    HaiDang, anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of LED Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_H_
#define LED_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include "prj_options.h"
#include <stdbool.h>
/* Exported Define -----------------------------------------------------------*/

#ifndef LED_TIME_TICK
#define LED_TIME_TICK   (TIMER_TIME_MSEC(10))
#endif

#ifndef LED_BREATHE_STEP
#define LED_BREATHE_STEP    (50)
#endif
/* Exported Typedefs ---------------------------------------------------------*/
typedef void (*LED_tpfOpen)(void);
typedef void (*LED_tpfClose)(void);
typedef void (*LED_tpfSetState)(void*);

#ifdef LED_SUPPORT_COLOR
typedef void (*LED_tpfSetColor)(uint8, uint8, uint8);
#endif

/* Exported Structure Declarations -------------------------------------------*/
/* error code */
typedef enum
{
  E_LED_OK,
  E_LED_FAIL
}LED_teStatus;

#ifdef LED_SUPPORT_EFFECT
/* list effect supported */
typedef enum
{
  E_LED_EFFECT_STOP = 0x00,
  E_LED_EFFECT_BLINK,
  E_LED_EFFECT_FLASH,
  E_LED_EFFECT_BREATHE,
  E_LED_EFFECT_COLOR_LOOP  
}LED_teEffect;

typedef struct
{
  uint8           u8Red;
  uint8           u8Green;
  uint8           u8Blue;
  uint8           u8Level;
}LED_tsColor;

/* struce effect LED */
typedef struct
{
  LED_teEffect   eEffect;
  uint16  u16Tick;      /* counter tick led */
  uint16  u16TimeOn;    /* time LED is on, unit 10ms */
  uint16  u16TimeOff;   /* time LED is off, unit 10ms */
  uint16  u16Period;    /* time in a period, uint 10ms */

  bool    bOn;          /* current state of flash effect */
  uint8   u8Flash;      /* total flash in a period */
  uint8   u8Toggle;     /* counter flash in a period */

  uint8   u8Count;      /* counter periods */
  uint8   u8Loop;       /* total of periods */

  bool    bState;       /* current state on/off */
  #ifdef LED_SUPPORT_COLOR
  bool    bDirection;   /* direction of effect: increase or decrease */
  LED_tsColor     sColor;
  #endif
}LED_tsEffect;
#endif

/* main struct of LED */
typedef struct
{
  bool            bState;
  LED_tpfOpen     pfOpen;
  LED_tpfClose    pfClose;
  
  #ifdef LED_SUPPORT_COLOR
  LED_tsColor     sColor;
  #endif
  LED_tpfSetState pfSetState;
}LED_tsLed;
/* Exported Functions Declarations -------------------------------------------*/
LED_teStatus LED_eInit(LED_tsLed *psLeds, const uint8 u8NumLeds);
LED_teStatus LED_eOpen(uint8          *pu8LedIndex,
                        LED_tsLed     *psLed);
LED_teStatus LED_eClose(uint8 u8LedIndex);
LED_teStatus LED_eSetOnOff(uint8 u8LedIndex, bool bState);

#ifdef LED_SUPPORT_COLOR
LED_teStatus LED_eSetLevel(uint8 u8LedIndex, uint8 u8Level);
LED_teStatus LED_eSetColor(uint8 u8LedIndex, uint8 u8Red, uint8 u8Green, uint8 u8Blue);
#endif

#ifdef LED_SUPPORT_EFFECT
LED_teStatus LED_eStartEffect(uint8 u8LedIndex, LED_tsEffect *psEffect);
#endif
/* External Variable Declarations --------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /*LED_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
