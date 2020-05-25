/****************************************************************************
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
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/* SDK includes */
#include "led.h"
#include <string.h>
#include "Timer.h"

#ifdef LED_TOTAL_NUMBER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
    uint8       u8NumLeds;
    LED_tsLed   *psLeds;
}LED_tsCommon;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void LED_vTask(void *pvParam);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Global Variables                                              ***/
/****************************************************************************/
uint8 u8TimerTaskLED;
/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static LED_tsCommon LED_sCommon;

static uint32 u32Tick10ms = 9;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
LED_teStatus LED_eInit(LED_tsLed *psLeds, uint8 u8NumLeds)
{
    if (psLeds == NULL || u8NumLeds == 0)
    {
        return E_LED_FAIL;
    }

    LED_sCommon.u8NumLeds = u8NumLeds;
    LED_sCommon.psLeds = psLeds;
    memset(psLeds, 0, sizeof(LED_tsCommon) * u8NumLeds);

    /* create timer update state of led */
    TIMER_eOpen(&u8TimerTaskLED, LED_vTask, NULL, TIMER_FLAG_PREVENT_SLEEP);
    TIMER_eStart(u8TimerTaskLED, TIMER_TIME_MSEC(10));

    return E_LED_OK;
}

LED_teStatus LED_eOpen(uint8          *pu8LedIndex,
                        LED_tsLed     *psLed)
{
    if (psLed->pfOpen != NULL || psLed->pfSetOnOff != NULL)
    {
        int i;
        LED_tsLed *psLeds;

        for ( i = 0; i < LED_sCommon.u8NumLeds; i++)
        {
            psLeds = &LED_sCommon.psLeds[i];

            if (psLeds->pfOpen == NULL)
            {
                /* copy value */
                memcpy(psLeds, psLed, sizeof(LED_tsLed));

                /* call function initialize hardware led */
                psLeds->pfOpen();
                /* return the index of the led */
                *pu8LedIndex = i;

                return E_LED_OK;
            }
        }
    }

    return E_LED_FAIL;
}

LED_teStatus LED_eClose(uint8 u8LedIndex)
{
    LED_tsLed *psLeds;
    psLeds = &LED_sCommon.psLeds[u8LedIndex];

    if (u8LedIndex > LED_sCommon.u8NumLeds || psLeds->pfClose == NULL)
    {
        return E_LED_FAIL;
    }

    /* release hardware led */
    psLeds->pfClose();
    /* reset all method of led */
    memset(psLeds, 0, sizeof(LED_tsLed));

    return E_LED_OK;
}

LED_teStatus LED_eSetOnOff(uint8 u8LedIndex, bool bState)
{
    LED_tsLed *psLeds;
    psLeds = &LED_sCommon.psLeds[u8LedIndex];

    if (u8LedIndex > LED_sCommon.u8NumLeds || psLeds->pfSetOnOff == NULL)
    {
        return E_LED_FAIL;
    }

    /* call function set hardware led */
    psLeds->bState = bState;
    if (psLeds->bActiveHight)
    {
        psLeds->pfSetOnOff(bState);
    }
    else
    {
        psLeds->pfSetOnOff(!bState);
    }

    return E_LED_OK;
}

#ifdef LED_SUPPORT_LEVEL
LED_teStatus LED_eSetLevel(uint8 u8LedIndex, uint8 u8Level)
{
    LED_tsLed *psLeds;
    psLeds = &LED_sCommon.psLeds[u8LedIndex];

    if (u8LedIndex > LED_sCommon.u8NumLeds || psLeds->pfSetLevel == NULL)
    {
        return E_LED_FAIL;
    }

    /* call function set level led */
    psLeds->u8Level = u8Level;
    psLeds->pfSetLevel(u8Level);

    return E_LED_OK;
}
#endif

#ifdef LED_SUPPORT_COLOR
LED_teStatus pfSetRGBColor(uint8 u8LedIndex, uint8 u8Red, uint8 u8Green, uint8 u8Blue)
{
    LED_tsLed *psLeds;
    psLeds = &LED_sCommon.psLeds[u8LedIndex];

    if (u8LedIndex > LED_sCommon.u8NumLeds || psLeds->pfSetColor == NULL)
    {
        return E_LED_FAIL;
    }

    /* call function set color led */
    psLeds->u8Red = u8Red;
    psLeds->u8Green = u8Green;
    psLeds->u8Blue = u8Blue;
    psLeds->pfSetColor(u8Red, u8Green, u8Blue);

    return E_LED_OK;
}
#endif

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/
static void LED_vTask(void *pvParam)
{
    /* restart timer update LED */
	TIMER_eStart(u8TimerTaskLED, TIMER_TIME_MSEC(10));

    u32Tick10ms++;

    /* wrap the tick10ms counter and provide 100ms ticks to cluster */
    if (u32Tick10ms > 9)
    {
        /* TODO: ZCL update 100mS */
        u32Tick10ms = 0;
    }
    #if (defined LED_SUPPORT_LEVEL)
    else
    {
        /* TODO: vLI_CreatePoints */
    }
    #endif
}
#endif /*LED_TOTAL_NUMBER*/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
