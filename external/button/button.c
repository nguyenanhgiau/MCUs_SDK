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
#include "Queue.h"

#ifdef BUTTON_TOTAL_NUMBER
/* Private Typedef -----------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
/* Private Structure Definition ----------------------------------------------*/
/* Global Variables ----------------------------------------------------------*/
tsQueue           APP_msgButtonEvents;
uint8 u8TimerScanButtons;
/* Private Variables Declarations --------------------------------------------*/
static BUTTON_tsButton  asButtons[BUTTON_TOTAL_NUMBER];
static BUTTON_tsEvent    asButtonMsg [BUTTON_QUEUE_SIZE];

static void BUTTON_vScanTask(void *pvParam);


BUTTON_teStatus BUTTON_eInit(void)
{
	memset(asButtons, 0, sizeof(BUTTON_tsButton) * BUTTON_TOTAL_NUMBER);

        /* Create queue for result of button */
        QUEUE_vCreate( &APP_msgButtonEvents, BUTTON_QUEUE_SIZE, sizeof(BUTTON_tsEvent), (uint8*)asButtonMsg);
	
	/* Create timer for scan button */
	TIMER_eOpen(&u8TimerScanButtons, BUTTON_vScanTask, NULL, TIMER_FLAG_PREVENT_SLEEP);
	TIMER_eStart(u8TimerScanButtons, BUTTON_TIME_SCAN);
	
	return E_BUTTON_OK;
}

BUTTON_teStatus BUTTON_eOpen(uint8 *pu8ButtonIndex, 
                             BUTTON_tpfOpen     pfOpen,
                             BUTTON_tpfClose    pfClose,
                             BUTTON_tpfRead     pfRead)
{
        if (pfOpen != NULL && pfRead != NULL) 
        {
                int i;
	        BUTTON_tsButton *psButtons;

                for (i = 0; i < BUTTON_TOTAL_NUMBER; i++)
                {
                        psButtons = &asButtons[i];
                        
                        if (psButtons->pfOpen == NULL)
                        {
                                /* set default value */
                                psButtons->oldState = E_BUTTON_STATE_RELEASE;
                                psButtons->newState = E_BUTTON_STATE_RELEASE;
                                psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;
                                psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;
                                psButtons->pfOpen = pfOpen;
                                psButtons->pfClose = pfClose;
                                psButtons->pfRead = pfRead;
                                
                                /* call function init hardware button */
                                psButtons->pfOpen();
                                /* Return the index of the button */
                                *pu8ButtonIndex = i;
                                
                                return E_BUTTON_OK;
                        }
                }
        }
	
	return E_BUTTON_FAIL;
}

BUTTON_teStatus BUTTON_eClose(uint8 u8ButtonIndex)
{
        BUTTON_tsButton *psButtons;
        psButtons = &asButtons[u8ButtonIndex];

	if (u8ButtonIndex > BUTTON_TOTAL_NUMBER || psButtons->pfClose == NULL)
	{
		return E_BUTTON_FAIL;
	}
	
        /* release hardware button */
        psButtons->pfClose();
        /* reset all method of button */
        memset(psButtons, 0, sizeof(BUTTON_tsButton));
	
	return E_BUTTON_OK;
}

static void BUTTON_vScanTask(void *pvParam)
{
	/* restart timer scan button */
	TIMER_eStart(u8TimerScanButtons, BUTTON_TIME_SCAN);
	
	/* scan button */
        int i;
        BUTTON_tsButton *psButtons;
        
        for (i = 0; i < BUTTON_TOTAL_NUMBER; i++)
        {
                psButtons = &asButtons[i];

                if (psButtons->pfRead != NULL)
                {
                        switch (psButtons->newState)
                        {
                        case E_BUTTON_STATE_RELEASE:                                            /* button state release */
                                if (psButtons->pfRead() != TRUE)                                /* if button down */
                                {
                                        psButtons->newState = E_BUTTON_STATE_DEBOUNCE;         /* change mode */
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
                                                        psButtons->timerSampleRL = 0;
                                                        psButtons->timePress = 0;                               /* reset time press */
                                                        /* send to queue button with state release */
                                                        BUTTON_tsEvent sButtonEvent;
                                                        sButtonEvent.eState = E_BUTTON_STATE_RELEASE;
                                                        sButtonEvent.u8NumberIndex = i;
                                                        QUEUE_bSend(&APP_msgButtonEvents, &sButtonEvent);
                                                }
                                        }
                                        
                                }
                                
                                psButtons->oldState = E_BUTTON_STATE_RELEASE;                                   /* save old state */
                                break;
                        
                        case E_BUTTON_STATE_DEBOUNCE:                                                          /* process debounce */
                                if (psButtons->pfRead() != TRUE)
                                {
                                        psButtons->timerNoisePress++;                                           /* increment sample press */
                                        if (psButtons->timerNoisePress >= BUTTON_TIME_NOISE_PRESS)              /* check noise press */
                                        {
                                                psButtons->timePress++;                                         /* increment time press */
                                                psButtons->newState = E_BUTTON_STATE_PRESS;                     /* change mode */
                                        }
                                }
                                else
                                {
                                        psButtons->timerNoiseRelease++;                                         /* increment sample release */
                                        if (psButtons->timerNoiseRelease >= BUTTON_TIME_NOISE_RELEASE)          /* check noise release */
                                        {
                                                psButtons->newState = E_BUTTON_STATE_RELEASE;                   /* change mode */
                                        }
                                }
                                
                                psButtons->oldState = E_BUTTON_STATE_DEBOUNCE;                                 /* update old state */
                                break;

                        case E_BUTTON_STATE_PRESS:
                                if (psButtons->pfRead() != TRUE)
                                {
                                        psButtons->timePress++;                                                 /* increment time press */
                                        if (psButtons->oldState == E_BUTTON_STATE_DEBOUNCE)
                                        {
                                                psButtons->flagSampleResult = BUTTON_ENABLE_SAMPLE;             /* Enable sample result */
                                                psButtons->flagSampleRelease = BUTTON_DISABLE_SAMPLE;           /* Disable sample release */
                                                psButtons->countClick++;                                        /* Increment count click */
                                        }

                                        if (psButtons->timePress >= BUTTON_TIME_HOLD_ON)                        /* check time press */
                                        {
                                                psButtons->newState = E_BUTTON_STATE_HOLD_ON;                   /* change mode */
                                                psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;            /* Disable sample result */
                                                psButtons->countClick = 0;                                      /* reset count click */
                                                /* send queue button with state hold on */
                                                BUTTON_tsEvent sButtonEvent;
                                                sButtonEvent.eState = E_BUTTON_STATE_HOLD_ON;
                                                sButtonEvent.u8NumberIndex = i;
                                                QUEUE_bSend(&APP_msgButtonEvents, &sButtonEvent);
                                        }
                                }
                                else
                                {
                                        psButtons->newState = E_BUTTON_STATE_DEBOUNCE;                         /* button is up */
                                        psButtons->timerNoisePress = 0;                                         /* reset timer sample press */
                                        psButtons->timerNoiseRelease = 0;                                       /* reset timer sample release */
                                        psButtons->timePress = 0;                                               /* reset time press */
                                }
                                
                                psButtons->oldState = E_BUTTON_STATE_PRESS;                                     /* update old sate */
                                break;

                        case E_BUTTON_STATE_HOLD_ON:
                                if (psButtons->pfRead() != TRUE)
                                {
                                        psButtons->timePress++;
                                        psButtons->timePress += psButtons->timePress;                           /* update time press */
                                }
                                else                                                                            /* button is up */
                                {
                                        psButtons->flagSampleRelease = BUTTON_ENABLE_SAMPLE;
                                        psButtons->newState = E_BUTTON_STATE_DEBOUNCE;
                                        psButtons->timerNoisePress = 0;                                         /* reset timer sample press */
                                        psButtons->timerNoiseRelease = 0;                                       /* reset timer sample release */
                                        psButtons->timePress = 0;
                                }
                                
                                psButtons->oldState = E_BUTTON_STATE_HOLD_ON;                                   /* update old sate */
                                break;
                        }

                        if (psButtons->flagSampleResult == BUTTON_ENABLE_SAMPLE)                                /* if sample result is enable */
                        {
                                if (psButtons->timerSampleRS++ >= BUTTON_TIME_SAMPLE)                           /* timer Sample result expired */
                                {
                                        if (psButtons->timePress <= BUTTON_TIME_CLICK)                          /* just update when not hold_on */
                                        {
                                                /* send to queue button with value psButtons->countClick */
                                                BUTTON_tsEvent sButtonEvent;
                                                sButtonEvent.eState = E_BUTTON_STATE_PRESS;
                                                sButtonEvent.u8NumberIndex = i;
                                                sButtonEvent.u8Click = psButtons->countClick;
                                                QUEUE_bSend(&APP_msgButtonEvents, &sButtonEvent);
                                                psButtons->countClick = 0;                                      /* reset count click */
                                        }
                                        
                                        psButtons->flagSampleResult = BUTTON_DISABLE_SAMPLE;                    /* change mode sample */
                                        psButtons->timerSampleRS = 0;                                           /* reset timer result */
                                        psButtons->flagSampleRelease = BUTTON_ENABLE_SAMPLE;                    /* enable sample release */
                                        psButtons->timerSampleRL = 0;                                           /* reset timer release */
                                }
                        }
                }
                
        }
}

BUTTON_teStatus BUTTON_eStop(void)
{
        /* check queue result */
        int i;
        BUTTON_tsButton *psButtons;

        for ( i = 0; i < BUTTON_TOTAL_NUMBER; i++)
        {
                psButtons = &asButtons[i];
                if (psButtons->newState != E_BUTTON_STATE_RELEASE)
                {
                        return E_BUTTON_FAIL;
                }
        }
        
        /* stop timer scan button */
        TIMER_eStop(u8TimerScanButtons);
        return E_BUTTON_OK;
}

BUTTON_teStatus BUTTON_eRestart(void)
{
        TIMER_eStart(u8TimerScanButtons, BUTTON_TIME_SCAN);
        return E_BUTTON_OK;
}

#endif /*BUTTON_TOTAL_NUMBER*/

