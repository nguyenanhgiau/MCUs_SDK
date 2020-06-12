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

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/
static void LED_vOutput(uint8 u8LedIndex);

#ifdef LED_SUPPORT_EFFECT
static void LED_vIdEffectTick(void *pvParam);
static LED_teStatus LED_bCheckValidEffect(LED_tsEffect *psEffect);
static void LED_vEffectOutput(uint8 u8LedIndex);
#endif
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Global Variables                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static LED_tsLed asLeds[LED_TOTAL_NUMBER];

#ifdef LED_SUPPORT_EFFECT
static LED_tsEffect LED_asEffect[LED_TOTAL_NUMBER];
uint8 u8TimerTickLED;
#endif
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
LED_teStatus LED_eInit(void)
{
    memset(asLeds, 0, sizeof(LED_tsLed) * LED_TOTAL_NUMBER);

    #ifdef LED_SUPPORT_EFFECT
    TIMER_eOpen(&u8TimerTickLED, LED_vIdEffectTick, NULL, TIMER_FLAG_PREVENT_SLEEP);
    TIMER_eStart(u8TimerTickLED, LED_TIME_TICK);
    #endif

    return E_LED_OK;
}

LED_teStatus LED_eOpen(uint8          *pu8LedIndex,
                        LED_tsLed     *psLed)
{
    if (psLed->pfOpen != NULL 
        || psLed->pfSetState == NULL
    )
    {
        int i;
        LED_tsLed *psLeds;

        for ( i = 0; i < LED_TOTAL_NUMBER; i++)
        {
            psLeds = &asLeds[i];

            if (psLeds->pfOpen == NULL)
            {
                /* copy value */
                memcpy(psLeds, psLed, sizeof(LED_tsLed));

                #ifdef LED_SUPPORT_COLOR
                if ((psLeds->sColor.u8Level == 0) && (psLeds->sColor.u8Red == 0)
                    && (psLeds->sColor.u8Green == 0) && (psLeds->sColor.u8Blue == 0))
                {
                    psLeds->sColor.u8Level = 255;
                    psLeds->sColor.u8Red = 255;
                    psLeds->sColor.u8Green = 255;
                    psLeds->sColor.u8Blue = 255;
                }
                #endif

                /* call function initialize hardware led */
                psLeds->pfOpen();

                /* turn off led */
                LED_vOutput(i);

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
    psLeds = &asLeds[u8LedIndex];

    if (u8LedIndex > LED_TOTAL_NUMBER || psLeds->pfClose == NULL)
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
    psLeds = &asLeds[u8LedIndex];

    if (u8LedIndex > LED_TOTAL_NUMBER
        || psLeds->pfSetState == NULL
    )
    {
        return E_LED_FAIL;
    }

    /* Different value? */
    if (psLeds->bState != bState)
    {
        /* Note the new state */
        psLeds->bState = bState;
        /* Set outputs */
        LED_vOutput(u8LedIndex);
    }

    return E_LED_OK;
}

#ifdef LED_SUPPORT_COLOR
LED_teStatus LED_eSetLevel(uint8 u8LedIndex, uint8 u8Level)
{
    LED_tsLed *psLeds;
    psLeds = &asLeds[u8LedIndex];

    if (u8LedIndex > LED_TOTAL_NUMBER)
    {
        return E_LED_FAIL;
    }

    /* Different value? */
    if (psLeds->sColor.u8Level != u8Level)
    {
        /* Note the new level */
        psLeds->sColor.u8Level = (u8Level == 0)? 1 : u8Level;
        /* Is the LED on */
        if (psLeds->bState)
        {
            /* Set outputs */
            LED_vOutput(u8LedIndex);
        }
    }

    return E_LED_OK;
}

LED_teStatus LED_eSetColor(uint8 u8LedIndex, uint8 u8Red, uint8 u8Green, uint8 u8Blue)
{
    LED_tsLed *psLeds;
    psLeds = &asLeds[u8LedIndex];

    if (u8LedIndex > LED_TOTAL_NUMBER || psLeds->pfSetState == NULL)
    {
        return E_LED_FAIL;
    }

    /* Different value? */
    if (psLeds->sColor.u8Red != u8Red || psLeds->sColor.u8Green != u8Green || psLeds->sColor.u8Blue != u8Blue)
    {
        /* Note the new values */
        psLeds->sColor.u8Red = u8Red;
        psLeds->sColor.u8Green = u8Green;
        psLeds->sColor.u8Blue = u8Blue;
        /* Is the LED on? */
        if (psLeds->bState)
        {
            /* Set outputs */
            LED_vOutput(u8LedIndex);
        }
    }

    return E_LED_OK;
}
#endif

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/
static void LED_vOutput(uint8 u8LedIndex)
{
    LED_tsLed *psLeds;
    psLeds = &asLeds[u8LedIndex];

    #ifdef LED_SUPPORT_COLOR
    LED_tsColor sColor;

    /* Is LED on? */
    if (psLeds->bState)
    {
        /* Scale color for brightness level */
        sColor.u8Red = (uint8)(((uint32)psLeds->sColor.u8Red * (uint32)psLeds->sColor.u8Level) / (uint32)255);
        sColor.u8Green = (uint8)(((uint32)psLeds->sColor.u8Green * (uint32)psLeds->sColor.u8Level) / (uint32)255);
        sColor.u8Blue = (uint8)(((uint32)psLeds->sColor.u8Blue * (uint32)psLeds->sColor.u8Level) / (uint32)255);

        /* Don't allow fully off */
        if (sColor.u8Red == 0) sColor.u8Red = 1;
        if (sColor.u8Green == 0) sColor.u8Green = 1;
        if (sColor.u8Blue == 0) sColor.u8Blue = 1;
    }
    else
    {
        sColor.u8Red = 0;
        sColor.u8Green = 0;
        sColor.u8Blue = 0;
    }
    /* Set output RGB channels */
    psLeds->pfSetState(&sColor);
    #else
    psLeds->pfSetState(&psLeds->bState);
    #endif
}

#ifdef LED_SUPPORT_EFFECT
LED_teStatus LED_eStartEffect(uint8 u8LedIndex, LED_tsEffect *psEffect)
{
    /* check valid */
    if (u8LedIndex > LED_TOTAL_NUMBER)
    {
        return E_LED_FAIL;
    }

    /* Check valid data */
    if (LED_bCheckValidEffect(psEffect) != E_LED_OK)
    {
        return E_LED_FAIL;
    }

    LED_tsEffect *psLedEffect;
    psLedEffect = &LED_asEffect[u8LedIndex];
    
    /* copy value to array */
    memcpy(psLedEffect, psEffect, sizeof(LED_tsEffect));

    return E_LED_OK;
}

static void LED_vIdEffectTick(void *pvParam)
{
    /* restart timer */
    TIMER_eStart(u8TimerTickLED, LED_TIME_TICK);

    /* handle tick LED */
    int i;
    LED_tsEffect    *psEffect;

    for ( i = 0; i < LED_TOTAL_NUMBER; i++)
    {
        psEffect = &LED_asEffect[i];

        switch (psEffect->eEffect)
        {
        case E_LED_EFFECT_BLINK:
            /**
             *      |< u8TimeOn>|
             *       ___________         
             *      |           |       |
             *      |           |       |
             *      |           |_______|
             *            
             *      |<-----u16Period---->
             */
            /* check times of effect */
            if (psEffect->u8Loop != 0 && psEffect->u8Count >= psEffect->u8Loop)
            {
                LED_vOutput(i);     /* recover old state if required */
                psEffect->eEffect = E_LED_EFFECT_STOP;
            }
            else
            {
                if (psEffect->u16Tick < psEffect->u16Period)
                {
                    psEffect->u16Tick++;

                    if (psEffect->u16Tick < psEffect->u16TimeOn)
                    {
                        psEffect->bState = TRUE;     /* Turn on LED */
                    }
                    else
                    {
                        psEffect->bState = FALSE;    /* Turn off LED */
                    }
                    LED_vEffectOutput(i);   /* Set out for effect */
                }
                else
                {
                    psEffect->u16Tick = 0;       /* Reset tick */
                    psEffect->u8Count++;        /* Increase counter period */
                }
            }
            break;

        case E_LED_EFFECT_FLASH:
            /**
             *       ___     ___         ___     ___
             *      |   |   |   |       |   |   |   |
             *      |   |   |   |       |   |   |   |
             *      |   |___|   |_______|   |___|   |______
             *            
             *      |<-----u16Period--->|
             */
            /* check times of effect */
            if (psEffect->u8Loop != 0 && psEffect->u8Count >= psEffect->u8Loop)
            {
                LED_vOutput(i); /* recover old state if required */
                psEffect->eEffect = E_LED_EFFECT_STOP;
            }
            else
            {
                if (psEffect->u16Tick < psEffect->u16Period)
                {
                    psEffect->u16Tick++;

                    if (psEffect->u8Toggle < psEffect->u8Flash)
                    {
                        if (!psEffect->bOn)
                        {
                            if (psEffect->u16Tick < (psEffect->u16TimeOn + ((psEffect->u16TimeOn + psEffect->u16TimeOff) * psEffect->u8Toggle)))
                            {
                                psEffect->bState = TRUE;         /* Turn on LED */
                            }
                            else
                            {
                                psEffect->bOn = !psEffect->bOn; /* Toggle state of LED */
                            }
                        }
                        else
                        {
                            if (psEffect->u16Tick < ((psEffect->u16TimeOn + psEffect->u16TimeOff) * (1 + psEffect->u8Toggle)))
                            {
                                psEffect->bState = FALSE;        /* Turn off LED */
                            }
                            else
                            {
                                psEffect->bOn = !psEffect->bOn; /* Toggle state of LED */
                                psEffect->u8Toggle++;           /* Increase counter flash */
                            }
                        }
                        LED_vEffectOutput(i);   /* Set out for effect */
                    }
                }
                else
                {
                    psEffect->u16Tick = 0;      /* Reset tick */
                    psEffect->u8Toggle = 0;     /* Reset toggle counter */
                    psEffect->u8Count++;        /* Increase counter period */
                }
            }
            break;

        #ifdef LED_SUPPORT_COLOR
        case E_LED_EFFECT_BREATHE:
            /**
             *         /\      /\
             *        /  \    /  \
             *       /    \  /    \
             *      /      \/      \
             *            
             *      |u16Per |
             */
            /* check counter of breathe, break if expired */
            if (psEffect->u8Loop != 0 && psEffect->u8Count >= psEffect->u8Loop)
            {
                LED_vOutput(i);     /* recover old state if required */
                psEffect->eEffect = E_LED_EFFECT_STOP;
            }
            else
            {
                if (psEffect->bDirection)   /* phrase increase */
                {
                    if (psEffect->sColor.u8Level >= (LED_BREATHE_STEP * 5))
                    {
                        psEffect->sColor.u8Level -= LED_BREATHE_STEP;
                        psEffect->bDirection = FALSE;
                    }
                    else
                    {
                        psEffect->sColor.u8Level += LED_BREATHE_STEP;
                    }
                }
                else    /* phrase decrease */
                {
                    if (psEffect->sColor.u8Level == 0)
                    {
                        psEffect->bDirection = TRUE;
                        psEffect->u8Count++;        /* Increase counter period */
                    }
                    else
                    {
                        psEffect->sColor.u8Level -= LED_BREATHE_STEP;
                    }
                }
                LED_vEffectOutput(i);   /* Set out for effect */
            }
            break;

        case E_LED_EFFECT_COLOR_LOOP:
            /* TODO: loop color follow HSV or RGB */
            break;
        #endif
        
        default:
            break;
        }
    }
}

static LED_teStatus LED_bCheckValidEffect(LED_tsEffect *psEffect)
{
    switch (psEffect->eEffect)
    {
    case E_LED_EFFECT_BLINK:
        if (psEffect->u16TimeOn >= psEffect->u16Period)
        {
            return E_LED_FAIL;
        }
        break;
    
    case E_LED_EFFECT_FLASH:
        if (((psEffect->u16TimeOn + psEffect->u16TimeOff) * psEffect->u8Flash) > psEffect->u16Period)
        {
            return E_LED_FAIL;
        }
        break;

    default:
        break;
    }
    return E_LED_OK;
}

static void LED_vEffectOutput(uint8 u8LedIndex)
{
    LED_tsLed *psLeds;
    psLeds = &asLeds[u8LedIndex];

    LED_tsEffect *psEffect;
    psEffect = &LED_asEffect[u8LedIndex];

    #ifdef LED_SUPPORT_COLOR
    LED_tsColor sColor;

    /* Is LED on? */
    if (psEffect->bState)
    {
        /* Scale color for brightness level */
        sColor.u8Red = (uint8)(((uint32)psEffect->sColor.u8Red * (uint32)psEffect->sColor.u8Level) / (uint32)255);
        sColor.u8Green = (uint8)(((uint32)psEffect->sColor.u8Green * (uint32)psEffect->sColor.u8Level) / (uint32)255);
        sColor.u8Blue = (uint8)(((uint32)psEffect->sColor.u8Blue * (uint32)psEffect->sColor.u8Level) / (uint32)255);

        /* Don't allow fully off */
        if (sColor.u8Red == 0) sColor.u8Red = 1;
        if (sColor.u8Green == 0) sColor.u8Green = 1;
        if (sColor.u8Blue == 0) sColor.u8Blue = 1;
    }
    else
    {
        sColor.u8Red = 0;
        sColor.u8Green = 0;
        sColor.u8Blue = 0;
    }
    /* Set output RGB channels */
    psLeds->pfSetState(&sColor);
    #else
    psLeds->pfSetState(&psEffect->bState);
    #endif
}
#endif

#endif /*LED_TOTAL_NUMBER*/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
