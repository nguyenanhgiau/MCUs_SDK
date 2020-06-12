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
#include "serial.h"
#include <string.h>
#include "Queue.h"

#ifdef SERIAL_TOTAL_NUMBER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

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
/***        Global Variables                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static SERIAL_tsSerial asSerial[SERIAL_TOTAL_NUMBER];

tsQueue SERIAL_msgTx[SERIAL_TOTAL_NUMBER];
tsQueue SERIAL_msgRx[SERIAL_TOTAL_NUMBER];

uint8 au8SerialBufTx[SERIAL_TOTAL_NUMBER][SERIAL_TX_QUEUE_SIZE];
uint8 au8SerialBufRx[SERIAL_TOTAL_NUMBER][SERIAL_RX_QUEUE_SIZE];
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
SERIAL_teStatus SERIAL_eInit(void)
{
    memset(asSerial, 0, sizeof(SERIAL_tsSerial) * SERIAL_TOTAL_NUMBER);
    return E_SERIAL_OK;
}

SERIAL_teStatus SERIAL_eOpen(uint8 *pu8SerialIndex, SERIAL_tsSerial *psSerial)
{
    if (psSerial != NULL)
    {
        int i;
        SERIAL_tsSerial *psSerials;

        for ( i = 0; i < SERIAL_TOTAL_NUMBER; i++)
        {
            psSerials = &asSerial[i];

            if (psSerials->pfOpen == NULL)
            {
                /* copy value */
                memcpy(psSerials, psSerial, sizeof(SERIAL_tsSerial));

                /* call function initialize hardware serial */
                psSerials->pfOpen();
                /* return the index of the serial */
                *pu8SerialIndex = i;

                /* create queue save buffer */
                QUEUE_vCreate(&SERIAL_msgTx[i], SERIAL_TX_QUEUE_SIZE, sizeof(uint8), (uint8*)&au8SerialBufTx[i][0]);
                QUEUE_vCreate(&SERIAL_msgRx[i], SERIAL_RX_QUEUE_SIZE, sizeof(uint8), (uint8*)&au8SerialBufRx[i][0]);

                return E_SERIAL_OK;
            }
        }
    }

    return E_SERIAL_FAIL;
}

SERIAL_teStatus SERIAL_eClose(uint8 u8SerialIndex)
{
    SERIAL_tsSerial *psSerials;
    psSerials = &asSerial[u8SerialIndex];

    if (u8SerialIndex > SERIAL_TOTAL_NUMBER || psSerials->pfClose == NULL)
    {
        return E_SERIAL_FAIL;
    }

    /* release hardware serial */
    psSerials->pfClose();
    /* reset all method of serial */
    memset(psSerials, 0, sizeof(SERIAL_tsSerial));

    return E_SERIAL_OK;
}

void SERIAL_vSend(uint8 u8SerialIndex, uint8 u8Byte)
{
    SERIAL_tsSerial *psSerials;
    psSerials = &asSerial[u8SerialIndex];

    if (u8SerialIndex > SERIAL_TOTAL_NUMBER || psSerials->pfSend == NULL)
    {
        return;
    }

    /* call function hardware send */
    psSerials->pfSend(u8Byte);
}

uint8 SERIAL_u8Receive(uint8 u8SerialIndex)
{
    SERIAL_tsSerial *psSerials;
    psSerials = &asSerial[u8SerialIndex];

    if (u8SerialIndex > SERIAL_TOTAL_NUMBER || psSerials->pfReceive == NULL)
    {
        return 0xFF;
    }

    /* call function hardware receive */
    return psSerials->pfReceive();
}

void SERIAL_vStopSend(uint8 u8SerialIndex)
{
    SERIAL_tsSerial *psSerials;
    psSerials = &asSerial[u8SerialIndex];

    if (u8SerialIndex > SERIAL_TOTAL_NUMBER || psSerials->pfStopSend == NULL)
    {
        return;
    }

    /* call function hardware stop send */
    psSerials->pfStopSend();
}

void SERIAL_vStartReceive(uint8 u8SerialIndex)
{
    SERIAL_tsSerial *psSerials;
    psSerials = &asSerial[u8SerialIndex];

    if (u8SerialIndex > SERIAL_TOTAL_NUMBER || psSerials->pfStartReceive == NULL)
    {
        return;
    }

    /* call function hardware stop send */
    psSerials->pfStartReceive();
}

SERIAL_teStatus SERIAL_eGet(uint8 u8SerialIndex, uint8 *pu8Byte)
{
    if (u8SerialIndex > SERIAL_TOTAL_NUMBER ||
        !QUEUE_bReceive(&SERIAL_msgTx[u8SerialIndex], pu8Byte))
    {
        /* call function stop send */
        asSerial[u8SerialIndex].pfStopSend();

        return E_SERIAL_FAIL;
    }

    return E_SERIAL_OK;
}

SERIAL_teStatus SERIAL_ePut(uint8 u8SerialIndex, uint8 u8Byte)
{
    if (u8SerialIndex > SERIAL_TOTAL_NUMBER ||
        !QUEUE_bSend(&SERIAL_msgRx[u8SerialIndex], &u8Byte))
    {
        return E_SERIAL_FAIL;
    }

    return E_SERIAL_OK;
}

SERIAL_teStatus SERIAL_eWrite(uint8 u8SerialIndex, uint8 *pau8Byte)
{
    if (u8SerialIndex > SERIAL_TOTAL_NUMBER)
    {
        return E_SERIAL_FAIL;
    }

    uint8 i;
    for ( i = 0; i < strlen((char*)pau8Byte); i++)
    {
        if (!QUEUE_bSend(&SERIAL_msgTx[u8SerialIndex], &pau8Byte[i]))
        {
            return E_SERIAL_FAIL;
        }
    }

    /* call function start send */
    asSerial[u8SerialIndex].pfStartSend();
    
    return E_SERIAL_OK;
}

uint32 SERIAL_u32Read(uint8 u8SerialIndex, uint8 *pau8Byte)
{
    int i;
    /* loop until queue empty */
    while (!QUEUE_bIsEmpty(&SERIAL_msgRx[u8SerialIndex]))
    {
        if (!QUEUE_bReceive(&SERIAL_msgRx[u8SerialIndex], &pau8Byte[i]))
        {
            break;
        }

        /* next item */
        i++;
    }
    
    return i;
}

/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/

#endif /*SERIAL_TOTAL_NUMBER*/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
