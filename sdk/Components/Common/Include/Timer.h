/*****************************************************************************
 *
 * MODULE:             Timer
 *
 * COMPONENT:          Timer.h
 *
 * DESCRIPTION:        Zigbee Timer Module
 * MODIFY:             giauna
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

#ifndef TIMER_H_
#define TIMER_H_

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define TIMER_TIME_SEC(v) ((uint32)(v) * 1000UL)
#define TIMER_TIME_MSEC(v) ((uint32)(v) * 1UL)

/* Flags for timer configuration */
#define TIMER_FLAG_ALLOW_SLEEP     0
#define TIMER_FLAG_PREVENT_SLEEP   (1 << 0)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    E_TIMER_STATE_CLOSED,
    E_TIMER_STATE_STOPPED,
    E_TIMER_STATE_RUNNING,
    E_TIMER_STATE_EXPIRED,    
} TIMER_teState;

typedef void (*TIMER_tpfCallback)(void *pvParam);

typedef struct
{
    uint8               u8Flags;
    TIMER_teState      eState;
    uint32                u32Time;
    void                *pvParameters;
    TIMER_tpfCallback    pfCallback;
} TIMER_tsTimer;

typedef enum
{
    E_TIMER_OK,
    E_TIMER_FAIL
} TIMER_teStatus;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

TIMER_teStatus TIMER_eInit(TIMER_tsTimer *psTimers, uint8 u8NumTimers);
#if (JENNIC_CHIP_FAMILY == JN516x)
void ISR_vTickTimer(void);
#else
PUBLIC void TIMER_vAhiCallback ( uint32 u32Device, uint32 u32ItemBitmap);
#endif
void TIMER_vSleep(void);
void TIMER_vWake(void);
void TIMER_vTask(void);
TIMER_teStatus TIMER_eOpen(uint8 *pu8TimerIndex, TIMER_tpfCallback pfCallback, void *pvParams, uint8 u8Flags);
TIMER_teStatus TIMER_eClose(uint8 u8TimerIndex);
TIMER_teStatus TIMER_eStart(uint8 u8TimerIndex, uint32 u32Time);
TIMER_teStatus TIMER_eStop(uint8 u8TimerIndex);
TIMER_teState TIMER_eGetState(uint8 u8TimerIndex);

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

#endif /*TIMER_H_*/
