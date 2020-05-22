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

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Global Variables                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static LED_tsCommon LED_sCommon;
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

    /* TODO: create timer update state of led */

    return E_LED_OK;
}

LED_teStatus LED_eOpen(uint8          *pu8LedIndex,
                        LED_tpfOpen   pfOpen,
                        LED_tpfClose  pfClose,
                        LED_tpfSet    pfSet,
                        bool          bActiveHight)
{
    if (pfOpen != NULL || pfSet != NULL)
    {
        int i;
        LED_tsLed *psLeds;

        for ( i = 0; i < LED_sCommon.u8NumLeds; i++)
        {
            psLeds = &LED_sCommon.psLeds[i];

            if (psLeds->pfOpen == NULL)
            {
                /* set default value */
                psLeds->bActiveHight = bActiveHight;
                psLeds->eState = E_LED_STATE_OFF;
                psLeds->pfOpen = pfOpen;
                psLeds->pfClose = pfClose;
                psLeds->pfSet = pfSet;

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
    psLeds->pfOpen = NULL;
    psLeds->pfClose = NULL;
    psLeds->pfSet = NULL;

    return E_LED_OK;
}

LED_teStatus LED_eSet(uint8 u8LedIndex, LED_teState eState)
{
    LED_tsLed *psLeds;
    psLeds = &LED_sCommon.psLeds[u8LedIndex];

    if (u8LedIndex > LED_sCommon.u8NumLeds || psLeds->pfSet == NULL)
    {
        return E_LED_FAIL;
    }

    /* call function set hardware led */
    psLeds->eState = eState;
    psLeds->pfSet(eState);

    return E_LED_OK;
}
#endif /*LED_TOTAL_NUMBER*/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
