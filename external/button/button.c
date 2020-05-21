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
#include "Button.h"
#include <string.h>
#include "Timer.h"

/* Private Typedef -----------------------------------------------------------*/
typedef struct
{
	uint8		        u8NumButtons;
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
	TIMER_eOpen(&u8TimerScanButtons, BUTTON_vScanTask, NULL, TIMER_FLAG_PREVENT_SLEEP);
	TIMER_eStart(u8TimerScanButtons, TIMER_TIME_MSEC(10));
	
	return E_BUTTON_OK;
}

BUTTON_teStatus BUTTON_eOpen(uint8 *pu8ButtonIndex, 
                             BUTTON_tpfOpen     pfOpen,
                             BUTTON_tpfClose    pfClose,
                             BUTTON_tpfGetState pfGetState, bool bPullUp)
{
	int i;
	BUTTON_tsButton *psButtons;
	
        if (pfOpen != NULL && pfGetState != NULL) 
        {
                for (i = 0; i < BUTTON_sCommon.u8NumButtons; i++)
                {
                        psButtons = &BUTTON_sCommon.psButtons[i];
                        
                        if (psButtons->pfOpen == NULL)
                        {
                                /* set default value */
                                psButtons->bPullUp = bPullUp;
                                psButtons->oldState = BUTTON_STATE_RELEASE;
                                psButtons->newState = BUTTON_STATE_RELEASE;
                                psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;
                                psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;
                                psButtons->pfOpen = pfOpen;
                                psButtons->pfClose = pfClose;
                                psButtons->pfGetState = pfGetState;
                                
                                /* call function init hardware button */
                                psButtons->pfOpen();
                                /* Return the index of the timer */
                                *pu8ButtonIndex = i;
                                
                                return E_BUTTON_OK;
                        }
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
	
        /* release hardware button */
        BUTTON_sCommon.psButtons[u8ButtonIndex].pfClose();
        /* reset all method of button */
        BUTTON_sCommon.psButtons[u8ButtonIndex].pfOpen = NULL;
        BUTTON_sCommon.psButtons[u8ButtonIndex].pfClose = NULL;
	BUTTON_sCommon.psButtons[u8ButtonIndex].pfGetState = NULL;
	
	return E_BUTTON_OK;
}

void BUTTON_vScanTask(void *pvParam)
{
	/* restart timer scan button */
	TIMER_eStart(u8TimerScanButtons, TIMER_TIME_MSEC(10));
	
	/*TODO: scan button */
        int i;
        BUTTON_tsButton *psButtons;
        
        for (i = 0; i < BUTTON_sCommon.u8NumButtons; i++)
        {
                psButtons = &BUTTON_sCommon.psButtons[i];

                if (psButtons->pfGetState != NULL)
                {
                        switch (psButtons->newState)
                        {
                        case BUTTON_STATE_RELEASE:                                              /* button state release */
                                if (psButtons->pfGetState() != psButtons->bPullUp)              /* if button down */
                                {
                                        psButtons->newState = BUTTON_STATE_DEBOUNDCE;           /* change mode */
                                        psButtons->timerNoisePress = 0;                         /* reset timer sample press */
                                        psButtons->timerNoiseRelease = 0;                       /* reset timer sample release */
                                        psButtons->timePress = 0;                               /* reset time press */
                                }
                                else
                                {
                                        if (psButtons->flagSampleRelease == BUTTON_ENABLE_SAMPLE)               /* if sample release is enable */
                                        {
                                                if (psButtons->timerSampleRL++ >= BUTTON_TIME_SAMPLE)           /* check timer sample release */
                                                {
                                                        psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;   /* not sample button release more */
                                                        psButtons->timePress = 0;                               /* reset time press */
                                                }
                                                
                                        }
                                        
                                }
                                
                                psButtons->oldState = BUTTON_STATE_RELEASE;                                     /* save old state */
                                break;
                        
                        case BUTTON_STATE_DEBOUNDCE:                                                            /* process deboundce */
                                if (psButtons->pfGetState() != psButtons->bPullUp)
                                {
                                        psButtons->timerNoisePress++;                                           /* increment sample press */
                                        if (psButtons->timerNoisePress >= BUTTON_TIME_NOISE_PRESS)              /* check noise press */
                                        {
                                                psButtons->timePress++;                                         /* increment time press */
                                                psButtons->newState = BUTTON_STATE_PRESS;                       /* change mode */
                                        }
                                        
                                }
                                else
                                {
                                        psButtons->timerNoiseRelease++;                                         /* increment sample release */
                                        if (psButtons->timerNoiseRelease >= BUTTON_TIME_NOISE_RELEASE)          /* check noise release */
                                        {
                                                psButtons->newState = BUTTON_STATE_RELEASE;                     /* change mode */
                                        }
                                }
                                
                                psButtons->oldState = BUTTON_STATE_DEBOUNDCE;                                   /* update old state */
                                break;

                        case BUTTON_STATE_PRESS:
                                if (psButtons->pfGetState() != psButtons->bPullUp)
                                {
                                        psButtons->timePress++;                                                 /* increment time press */
                                        if (psButtons->oldState == BUTTON_STATE_DEBOUNDCE)
                                        {
                                                psButtons->flagSampleResult = BUTTON_ENABLE_SAMPLE;             /* Enable sample result */
                                                psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;           /* Disable sample release */
                                                psButtons->countClick++;                                        /* Increment count click */
                                        }
                                }
                                else
                                {
                                        psButtons->newState = BUTTON_STATE_DEBOUNDCE;                           /* button is up */
                                        psButtons->timerNoisePress = 0;                                         /* reset timer sample press */
                                        psButtons->timerNoiseRelease = 0;                                       /* reset timer sample release */
                                        psButtons->timePress = 0;                                               /* reset time press */
                                }
                                
                                if (psButtons->timePress >= BUTTON_TIME_HOLD_ON)                                /* check time press */
                                {
                                        psButtons->newState = BUTTON_STATE_HOLD_ON;                             /* change mode */
                                        /* TODO: send queue button with state hold on */
                                        psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;                    /* Disable sample result */
                                }
                                
                                psButtons->oldState = BUTTON_STATE_PRESS;                                       /* update old sate */
                                break;

                        case BUTTON_STATE_HOLD_ON:
                                if (psButtons->pfGetState() != psButtons->bPullUp)
                                {
                                        psButtons->timePress++;
                                        psButtons->timePress += psButtons->timePress;                           /* update time press */
                                }
                                else                                                                            /* button is up */
                                {
                                        psButtons->flagSampleRelease = BUTTON_ENABLE_SAMPLE;
                                        psButtons->newState = BUTTON_STATE_DEBOUNDCE;
                                        psButtons->timerNoisePress = 0;                                         /* reset timer sample press */
                                        psButtons->timerNoiseRelease = 0;                                       /* reset timer sample release */
                                        psButtons->timePress = 0;
                                }
                                
                                psButtons->oldState = BUTTON_STATE_HOLD_ON;                                     /* update old sate */
                                break;
                        }

                        if (psButtons->flagSampleResult == BUTTON_ENABLE_SAMPLE)                                /* if sample result is enable */
                        {
                                if (psButtons->timerSampleRS++ >= BUTTON_TIME_SAMPLE)                           /* timer Sample result expired */
                                {
                                        if (psButtons->timePress <= BUTTON_TIME_CLICK)                          /* just update when not hold_on */
                                        {
                                                /* TODO send to queue button with value psButtons->countClick */
                                        }
                                        
                                        psButtons->countClick = 0;                                              /* reset count click */
                                        psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;                    /* change mode sample */
                                        psButtons->timerSampleRS = 0;                                           /* reset timer result */
                                        psButtons->flagSampleRelease = BUTTON_ENABLE_SAMPLE;                    /* enable sample release */
                                        psButtons->timerSampleRL = 0;                                           /* reset timer release */
                                }
                        }
                }
                
        }
}

