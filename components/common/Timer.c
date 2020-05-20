/*****************************************************************************
 *
 * MODULE:             Timer
 *
 * COMPONENT:          Timer.c
 *
 * DESCRIPTION:        Timer Module
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5179].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/*          Include files                                                   */
/****************************************************************************/

#include "chip_selection.h"
#include <string.h>
//#include "dbg.h"
#include "Timer.h"

/****************************************************************************/
/*          Macro Definitions                                               */
/****************************************************************************/

#ifdef DEBUG_TIMER
#define TRACE_TIMER    TRUE
#else
#define TRACE_TIMER    FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                                */
/****************************************************************************/

typedef struct
{
    volatile uint32   u8Ticks;
    uint8            u8NumTimers;
    TIMER_tsTimer    *psTimers;
} TIMER_tsCommon;

/****************************************************************************/
/*          Local Function Prototypes                                       */
/****************************************************************************/

/****************************************************************************/
/*          Exported Variables                                              */
/****************************************************************************/

/****************************************************************************/
/*          Local Variables                                                 */
/****************************************************************************/

static TIMER_tsCommon TIMER_sCommon;

/****************************************************************************/
/*          Exported Functions                                              */
/****************************************************************************/

/****************************************************************************
 *
 * NAME: TIMER_eInit
 *
 * DESCRIPTION:
 * Initialises the Timer module
 *
 * RETURNS:
 * TIMER_teStatus
 *
 ****************************************************************************/
TIMER_teStatus TIMER_eInit(TIMER_tsTimer *psTimers, uint8 u8NumTimers)
{

//    DBG_vPrintf(TRACE_TIMER, "ZT: Initialising: ");

    if(psTimers == NULL || u8NumTimers == 0)
    {
//        DBG_vPrintf(TRACE_TIMER, "Failed\n");
        return E_TIMER_FAIL;
    }

    TIMER_sCommon.u8Ticks = 0;
    TIMER_sCommon.u8NumTimers = u8NumTimers;
    TIMER_sCommon.psTimers = psTimers;
    memset(psTimers, 0, sizeof(TIMER_tsTimer) * u8NumTimers);

//    DBG_vPrintf(TRACE_TIMER, "Success\n");

    return E_TIMER_OK;

}


/****************************************************************************
 *
 * NAME: TIMER_cbCallback
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void ISR_vTickTimer(void)
{
    /* TODO: clear interrupt time base */

    if(TIMER_sCommon.u8Ticks < 0xff)
    {
        TIMER_sCommon.u8Ticks++;
    }
}

/****************************************************************************
 *
 * NAME: TIMER_vSleep
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void TIMER_vSleep(void)
{
    /* TODO: disable time base */

}


/****************************************************************************
 *
 * NAME: TIMER_vWake
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void TIMER_vWake(void)
{
    /* TODO: enable time base and restart */
    
}


/****************************************************************************
 *
 * NAME: TIMER_vTask
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void TIMER_vTask(void)
{

    int n;
    TIMER_tsTimer *psTimer;

    /* If no ticks to process, exit */
    if(TIMER_sCommon.u8Ticks == 0)
    {
        return;
    }

    /* Decrement the tick counter */
    TIMER_sCommon.u8Ticks--;

//    DBG_vPrintf(TRACE_TIMER, "ZT: Tick\n");

    /* Process all of the timers */
    for(n = 0; n < TIMER_sCommon.u8NumTimers; n++)
    {

        psTimer = &TIMER_sCommon.psTimers[n];

        /* If this timer is not opened and running, skip it */
        if(psTimer->eState != E_TIMER_STATE_RUNNING)
        {
            continue;
        }

//        DBG_vPrintf(TRACE_TIMER, "ZT: Processing timer %d - time %d\n", n, psTimer->u32Time);

        /* Decrement the time */
        psTimer->u32Time--;

        /* If the timer has not expired, move on to the next one */
        if(psTimer->u32Time > 0)
        {
            continue;
        }

        /* Mark the timer as expired. We must do this _before_ calling the callback
         * in case the user restarts the timer in the callback */
        psTimer->eState = E_TIMER_STATE_EXPIRED;

//        DBG_vPrintf(TRACE_TIMER, "ZT: Timer %d expired\n", n);

        /* If this timer should prevent sleeping while running, decrement the activity count */
        if(psTimer->u8Flags & TIMER_FLAG_PREVENT_SLEEP)
        {
            /*TODO: decrement the activity count*/
            
        }

        /* If the timer has  a valid callback, call it */
        if(psTimer->pfCallback != NULL)
        {
            psTimer->pfCallback(psTimer->pvParameters);
        }

    }

}


/****************************************************************************
 *
 * NAME: TIMER_eOpen
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * TIMER_teStatus
 *
 ****************************************************************************/
TIMER_teStatus TIMER_eOpen(uint8 *pu8TimerIndex, TIMER_tpfCallback pfCallback, void *pvParams, uint8 u8Flags)
{

    int n;
    TIMER_tsTimer *psTimer;

//    DBG_vPrintf(TRACE_TIMER, "ZT: Open: ");

    /* Find a unused timer */
    for(n = 0; n < TIMER_sCommon.u8NumTimers; n++)
    {

        psTimer = &TIMER_sCommon.psTimers[n];

        if(psTimer->eState == E_TIMER_STATE_CLOSED)
        {
            psTimer->u8Flags             = u8Flags;
            psTimer->pvParameters        = pvParams;
            psTimer->pfCallback          = pfCallback;
            psTimer->u32Time             = 0;
            psTimer->eState              = E_TIMER_STATE_STOPPED;

            /* Return the index of the timer */
            *pu8TimerIndex = n;

//            DBG_vPrintf(TRACE_TIMER, "Success (%d)\n", n);

            return E_TIMER_OK;

        }
    }

//    DBG_vPrintf(TRACE_TIMER, "Failed\n");

    return E_TIMER_FAIL;

}


/****************************************************************************
 *
 * NAME: TIMER_eClose
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * TIMER_teStatus
 *
 ****************************************************************************/
TIMER_teStatus TIMER_eClose(uint8 u8TimerIndex)
{

//    DBG_vPrintf(TRACE_TIMER, "ZT: Close (%d): ", u8TimerIndex);

    if(u8TimerIndex > TIMER_sCommon.u8NumTimers)
    {
//        DBG_vPrintf(TRACE_TIMER, "Failed\n");
        return E_TIMER_FAIL;
    }

    /* If the timer is currently running, decrease power manager activity count */
    if(TIMER_sCommon.psTimers[u8TimerIndex].eState == E_TIMER_STATE_RUNNING)
    {
        /*TODO: decrease power manager activity count*/
        
    }

    TIMER_sCommon.psTimers[u8TimerIndex].eState = E_TIMER_STATE_CLOSED;

//    DBG_vPrintf(TRACE_TIMER, "Success\n");

    return E_TIMER_OK;

}


/****************************************************************************
 *
 * NAME: TIMER_eStart
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * TIMER_teStatus
 *
 ****************************************************************************/
TIMER_teStatus TIMER_eStart(uint8 u8TimerIndex, uint32 u32Time)
{

//    DBG_vPrintf(TRACE_TIMER, "ZT: Start (%d): ", u8TimerIndex);

    /* Check timer index is valid and the timer has previously been opened */
    if(u8TimerIndex > TIMER_sCommon.u8NumTimers || TIMER_sCommon.psTimers[u8TimerIndex].eState == E_TIMER_STATE_CLOSED || u32Time == 0)
    {
//        DBG_vPrintf(TRACE_TIMER, "Failed\n");
        return E_TIMER_FAIL;
    }

    /* If this timer should prevent sleeping while running and the timer is not currently running, increase power manager activity count */
    if((TIMER_sCommon.psTimers[u8TimerIndex].u8Flags & TIMER_FLAG_PREVENT_SLEEP) && (TIMER_sCommon.psTimers[u8TimerIndex].eState != E_TIMER_STATE_RUNNING))
    {
        /*TODO: increase power manager activity count */
        
    }

    /* Load the timer and start it */
    TIMER_sCommon.psTimers[u8TimerIndex].u32Time = u32Time;
    TIMER_sCommon.psTimers[u8TimerIndex].eState = E_TIMER_STATE_RUNNING;

//    DBG_vPrintf(TRACE_TIMER, "Success\n");

    return E_TIMER_OK;

}


/****************************************************************************
 *
 * NAME: TIMER_eStop
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * TIMER_teStatus
 *
 ****************************************************************************/
TIMER_teStatus TIMER_eStop(uint8 u8TimerIndex)
{

//    DBG_vPrintf(TRACE_TIMER, "ZT: Stop (%d): ", u8TimerIndex);

    /* Check timer index is valid and the timer has previously been opened */
    if(u8TimerIndex > TIMER_sCommon.u8NumTimers || TIMER_sCommon.psTimers[u8TimerIndex].eState == E_TIMER_STATE_CLOSED)
    {
//        DBG_vPrintf(TRACE_TIMER, "Failed\n");
        return E_TIMER_FAIL;
    }

    /* If this timer should prevent sleeping while running and the timer is currently running, decrease power manager activity count */
    if((TIMER_sCommon.psTimers[u8TimerIndex].u8Flags & TIMER_FLAG_PREVENT_SLEEP) && (TIMER_sCommon.psTimers[u8TimerIndex].eState == E_TIMER_STATE_RUNNING))
    {
        /*TODO: decrease power manager activity count*/
        
    }

    /* Stop the timer */
    TIMER_sCommon.psTimers[u8TimerIndex].eState = E_TIMER_STATE_STOPPED;

//    DBG_vPrintf(TRACE_TIMER, "Success\n");

    return E_TIMER_OK;

}


/****************************************************************************
 *
 * NAME: TIMER_eGetState
 *
 * DESCRIPTION:
 *
 *
 * RETURNS:
 * TIMER_teState
 *
 ****************************************************************************/
TIMER_teState TIMER_eGetState(uint8 u8TimerIndex)
{
     return TIMER_sCommon.psTimers[u8TimerIndex].eState;
}


/****************************************************************************/
/***        Local Functions                                                 */
/****************************************************************************/

/****************************************************************************/
/*          END OF FILE                                                     */
/****************************************************************************/
