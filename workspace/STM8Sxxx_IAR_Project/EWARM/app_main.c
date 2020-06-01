/*****************************************************************************
 *
 * MODULE:             JN-AN-1218
 *
 * COMPONENT:          app_main.c
 *
 * DESCRIPTION:        Light bulb application main file
 *
 *****************************************************************************
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
 * Copyright NXP B.V. 2017. All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "chip_selection.h"
#include "prj_options.h"
#include "app_main.h"
#include "Queue.h"
#include "Timer.h"
#include "dbg.h"

#ifdef BUTTON_TOTAL_NUMBER
#include "Button.h"
#endif

#ifdef LED_TOTAL_NUMBER
#include "led.h"
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef BUTTON_TOTAL_NUMBER
#define APP_TIMER_BUTTON        1
#else
#define APP_TIMER_BUTTON        0
#endif

#ifdef LED_TOTAL_NUMBER
#define APP_TIMER_LED        1
#else
#define APP_TIMER_LED        0
#endif

#define APP_TOTAL_TIMER   (APP_TIMER_BUTTON + APP_TIMER_LED)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
#if (APP_TOTAL_TIMER != 0)
TIMER_tsTimer asTimers[APP_TOTAL_TIMER];
#endif

#ifdef BUTTON_TOTAL_NUMBER
BUTTON_tsButton asButtons[BUTTON_TOTAL_NUMBER];
#endif

#ifdef LED_TOTAL_NUMBER
LED_tsLed asLeds[LED_TOTAL_NUMBER];
#endif

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: APP_vMainLoop
 *
 * DESCRIPTION:
 * Main application loop
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_vMainLoop(void)
{

    while (1)
    {
        /* call timer task handle soft timer */
        TIMER_vTask();
        
        /*TODO: add watchdog restart */
        
        /*TODO: add main task */
        #ifdef BUTTON_TOTAL_NUMBER
        BUTTON_tsEvent sButtonEvent;
        if (QUEUE_bReceive(&APP_msgButtonEvents, &sButtonEvent))
        {
            switch (sButtonEvent.eState)
            {
            case E_BUTTON_STATE_RELEASE:
                DBG_vPrintf(TRUE, "Button %d Release\n", sButtonEvent.u8NumberIndex);
                break;
            
            case E_BUTTON_STATE_PRESS:
                DBG_vPrintf(TRUE, "Button %d Click %d Times\n", sButtonEvent.u8NumberIndex, sButtonEvent.u8Click);
                break;

            case E_BUTTON_STATE_HOLD_ON:
                DBG_vPrintf(TRUE, "Button %d Hold on\n", sButtonEvent.u8NumberIndex);
                break;
            }
        }
        #endif
        
        /*TODO: add task management power */
    }
}


/****************************************************************************
 *
 * NAME: APP_vSetUpHardware
 *
 * DESCRIPTION:
 * Set up interrupts
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_vSetUpHardware(void)
{
    
}


/****************************************************************************
 *
 * NAME: APP_vInitResources
 *
 * DESCRIPTION:
 * Initialise resources (timers, queue's etc)
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void APP_vInitResources(void)
{
    #if (APP_TOTAL_TIMER != 0)
    TIMER_eInit(asTimers, sizeof(asTimers) / sizeof(TIMER_tsTimer));
    #endif

    #ifdef BUTTON_TOTAL_NUMBER
    BUTTON_eInit(asButtons, sizeof(asButtons) / sizeof(BUTTON_tsButton));
    #endif

    #ifdef LED_TOTAL_NUMBER
    LED_eInit(asLeds, sizeof(asLeds) / sizeof(LED_tsLed));
    #endif
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
