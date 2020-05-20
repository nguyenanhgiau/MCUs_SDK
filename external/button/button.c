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
* @file:      Button.c
* @author:    anhgiau (nguyenanhgiau1008@gmail.com)
* @version:   1.0.1
* @date:      7/31/2018
* @brief:     Implementation of Button Driver
********************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include <string.h>
#include "Timer.h"

/* Private Typedef -----------------------------------------------------------*/
typedef struct
{
	volatile uint8		u8NumButtons;
	BUTTON_tsButton 	*psButtons;
}BUTTON_tsCommon;
/* Private Define ------------------------------------------------------------*/
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
/* Private Variables Declarations --------------------------------------------*/
static BUTTON_tsCommon BUTTON_sCommon;
uint8 u8TimerScanButtons;

static void BUTTON_vScanTask(void *pvParam);


BUTTON_teStatus BUTTON_eInit(BUTTON_tsButton *psButtons, uint8 u8NumButtons)
{
	if (psButtons == NULL || u8NumButtons == 0)
	{
		return E_BUTTON_FAIL;
	}
	
	BUTTON_sCommon.u8NumButtons = u8NumButtons;
	BUTTON_sCommon.psButtons = psButtons;
	memset(psButtons, 0, sizeof(BUTTON_tsButton) * u8NumButtons);
	
	/* Create timer for scan button */
	TIMER_eOpen(&u8TimerScanButtons, BUTTON_vScanTask, TIMER_FLAG_PREVENT_SLEEP);
	TIMER_eStart(u8TimerScanButtons, TIMER_TIME_MSEC(10));
	
	return E_BUTTON_OK;
}

BUTTON_teStatus BUTTON_eOpen(uint8 *pu8ButtonIndex, BUTTON_tpfGetState pfGetState, bool bPullUp)
{
	int n;
	BUTTON_tsButton *psButtons;
	
	for (n = 0; n < BUTTON_sCommon.u8NumButtons; n++)
	{
		psButtons = &BUTTON_sCommon.psButtons[n];
		
		if (psButtons->pfGetState == NULL)
		{
			/* set default value */
			psButtons->oldState = BUTTON_STATE_RELEASE;
			psButtons->newState = BUTTON_STATE_RELEASE;
			psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;
			psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;
			psButtons->pfGetState = pfGetState;
			
			/* Return the index of the timer */
			*pu8ButtonIndex = n;
			
			return E_BUTTON_OK;
		}
	}
	
	return E_BUTTON_FAIL;
}

BUTTON_teStatus BUTTON_eClose(uint8 u8ButtonIndex)
{
	if (u8ButtonIndex > BUTTON_sCommon.u8NumButtons)
	{
		return E_BUTTON_FAIL;
	}
	
	BUTTON_sCommon.psButtons[u8ButtonIndex].pfGetState = NULL;
	
	return E_BUTTON_OK;
}

void BUTTON_vScanTask(void *pvParam)
{
	/* restart timer scan button */
	TIMER_eStart(u8TimerScanButtons, TIMER_TIME_MSEC(10));
	
	/*TODO: scan button */
}

