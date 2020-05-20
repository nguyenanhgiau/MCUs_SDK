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
* @file:      Button.h
* @author:    HaiDang, anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Header file of Button Driver
********************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BUTTON_H_
#define BUTTON_H_
 
#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "chip_selection.h"
#include <stdbool.h>
/* Exported Define -----------------------------------------------------------*/
   
/* state of button */
typedef enum {
  E_BUTTON_STATE_RELEASE        = 0,
  E_BUTTON_STATE_SINGLE_CLICK,
  E_BUTTON_STATE_DOUBLE_CLICK,
  E_BUTTON_STATE_TRIPLE_CLICK,
  E_BUTTON_STATE_HOLD_ON        = 0xF0,
  E_BUTTON_STATE_DEBOUNDCE      = 0xFE
}Button_teState;

#define BUTTON_STATE_RELEASE            (0)
#define BUTTON_STATE_DEBOUNDCE          (1)
#define BUTTON_STATE_PRESS              (2)
#define BUTTON_STATE_HOLD_ON            (3)
   
#define BUTTON_TIME_NOISE_PRESS         (1)
#define BUTTON_TIME_NOISE_RELEASE       (1)
#define BUTTON_TIME_HOLD_OFF            (200)
#define BUTTON_TIME_HOLD_ON             (200)
#define BUTTON_TIME_SAMPLE              (80)
#define BUTTON_TIME_SINGLE_CLICK        (20)
   
#define BUTTON_DISABLE_SAMPLE           (0)
#define BUTTON_ENABLE_SAMPLE            (1)

#define BUTTON_RELEASE          (0)
#define BUTTON_SINGLE_CLICK     (1)
#define BUTTON_DOUBLE_CLICK     (2)
#define BUTTON_TRIPLE_CLICK     (3)
#define BUTTON_HOLD_ON          (0xFF)
/* Exported Typedefs ---------------------------------------------------------*/
typedef struct{
  uint8_t       oldState;               /* old state of button*/
  uint8_t       newState;               /* new state of button */
  uint8_t       timerNoisePress;        /* timer process noise press */
  uint8_t       timerNoiseRelease;      /* timer process noise release */
  uint8_t       countClick;             /* times click of button */
  uint8_t       timePress;              /* timer button was pressed */
  bool          flagSampleResult;       /* flag sample result */
  uint8_t       timerSampleRS;          /* timer sample result */
  bool          flagSampleRelease;      /* flag sample release */
  uint8_t       timerSampleRL;          /* timer sample release */
  uint8_t       (*read)(void);          /* pointer to function read button */
}BUTTON_tsButton;

typedef struct{
  uint8_t result;                       /* result actual of button */
  uint32_t timePress;                   /* time button was press */
}ButtonResult_t;

typedef enum
{
    E_BUTTON_OK,
    E_BUTTON_FAIL
} BUTTON_teStatus;
/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
BUTTON_teStatus BUTTON_eInit(BUTTON_tsButton *psButtons, uint8 u8NumButtons);
/* External Variable Declarations --------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /*BUTTON_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
