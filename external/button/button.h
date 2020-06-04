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
#include "Queue.h"
#include "prj_options.h"
/* Exported Define -----------------------------------------------------------*/

typedef void (*BUTTON_tpfOpen)(void);
typedef void (*BUTTON_tpfClose)(void);
typedef bool (*BUTTON_tpfRead)(void);
   
#ifndef BUTTON_TIME_SCAN
#define BUTTON_TIME_SCAN                (TIMER_TIME_MSEC(10))
#endif

#ifndef BUTTON_TIME_NOISE_PRESS
#define BUTTON_TIME_NOISE_PRESS         (1)
#endif

#ifndef BUTTON_TIME_NOISE_RELEASE
#define BUTTON_TIME_NOISE_RELEASE       (1)
#endif

#ifndef BUTTON_TIME_HOLD_OFF
#define BUTTON_TIME_HOLD_OFF            (100)
#endif

#ifndef BUTTON_TIME_HOLD_ON
#define BUTTON_TIME_HOLD_ON             (100)
#endif

#ifndef BUTTON_TIME_SAMPLE
#define BUTTON_TIME_SAMPLE              (50)
#endif

#ifndef BUTTON_TIME_CLICK
#define BUTTON_TIME_CLICK               (20)
#endif

#ifndef BUTTON_QUEUE_SIZE
#define BUTTON_QUEUE_SIZE               (8)
#endif
   
#define BUTTON_DISABLE_SAMPLE           (0)
#define BUTTON_ENABLE_SAMPLE            (1)

/* Exported Typedefs ---------------------------------------------------------*/
/* state of button */
typedef enum {
  E_BUTTON_STATE_RELEASE        = 0,
  E_BUTTON_STATE_DEBOUNCE,
  E_BUTTON_STATE_PRESS,
  E_BUTTON_STATE_HOLD_ON,
}BUTTON_teState;

/* event of button */
typedef struct
{
  BUTTON_teState  eState;
  uint8           u8NumberIndex;
  uint8           u8Click;
}BUTTON_tsEvent;

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
  BUTTON_tpfOpen        pfOpen;         /* pointer to function init hardware button */
  BUTTON_tpfClose       pfClose;        /* pointer to function destroy hardware button */
  BUTTON_tpfRead        pfRead;         /* pointer to function read button */
}BUTTON_tsButton;

typedef enum
{
    E_BUTTON_OK,
    E_BUTTON_FAIL
} BUTTON_teStatus;
/* Exported Structure Declarations -------------------------------------------*/
/* Exported Functions Declarations -------------------------------------------*/
BUTTON_teStatus BUTTON_eInit(BUTTON_tsButton *psButtons, const uint8 u8NumButtons);
BUTTON_teStatus BUTTON_eOpen(uint8 *pu8ButtonIndex, 
                             BUTTON_tpfOpen     pfOpen,
                             BUTTON_tpfClose    pfClose,
                             BUTTON_tpfRead     pfRead);
BUTTON_teStatus BUTTON_eClose(uint8 u8ButtonIndex);

/* External Variable Declarations --------------------------------------------*/
#ifdef BUTTON_TOTAL_NUMBER
extern tsQueue           APP_msgButtonEvents;
extern uint8 u8TimerScanButtons;
#endif
#ifdef __cplusplus
}
#endif
#endif /*BUTTON_H_*/
/************************ (C) COPYRIGHT anhgiau ****************END OF FILE****/
