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
#include "spi.h"
#include <string.h>
#include "Queue.h"

// #ifdef SPI_TOTAL_NUMBER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef struct
{
    uint8           u8NumSPIs;
    SPI_tsSpi       *psSPIs;
}SPI_tsCommon;
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
static SPI_tsCommon SPI_sCommon;
/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
SPI_teStatus SPI_eInit(SPI_tsSpi *psSPIs, const uint8 u8NumSPIs)
{
    if (psSPIs == NULL || u8NumSPIs == 0)
    {
        return E_SPI_FAIL;
    }

    SPI_sCommon.u8NumSPIs = u8NumSPIs;
    SPI_sCommon.psSPIs = psSPIs;
    memset(psSPIs, 0, sizeof(SPI_tsSpi) * u8NumSPIs);
    
    return E_SPI_OK;
}

SPI_teStatus SPI_eOpen(uint8 *pu8SpiIndex, SPI_tsSpi *psSPI)
{
    if (psSPI != NULL)
    {
        int i;
        SPI_tsSpi *psSPIs;

        for ( i = 0; i < SPI_sCommon.u8NumSPIs; i++)
        {
            psSPIs = &SPI_sCommon.psSPIs[i];

            if (psSPIs->pfOpen == NULL)
            {
                /* copy value */
                memcpy(psSPIs, psSPI, sizeof(SPI_tsSpi));

                /* call function initialize hardware serial */
                psSPIs->pfOpen();
                /* return the index of the serial */
                *pu8SpiIndex = i;

                return E_SPI_OK;
            }
        }
    }

    return E_SPI_FAIL;
}

SPI_teStatus SPI_eClose(uint8 u8SpiIndex)
{
    SPI_tsSpi *psSPIs;
    psSPIs = &SPI_sCommon.psSPIs[u8SpiIndex];

    if (u8SpiIndex > SPI_sCommon.u8NumSPIs || psSPIs->pfClose == NULL)
    {
        return E_SPI_FAIL;
    }

    /* release hardware serial */
    psSPIs->pfClose();
    /* reset all method of serial */
    memset(psSPIs, 0, sizeof(SPI_tsSpi));

    return E_SPI_OK;
}

void SPI_vSelect(uint8 u8SpiIndex)
{
    SPI_tsSpi *psSPIs;
    psSPIs = &SPI_sCommon.psSPIs[u8SpiIndex];

    if (u8SpiIndex > SPI_sCommon.u8NumSPIs || psSPIs->pfSelect == NULL)
    {
        return;
    }

    /* pull-down pin latch */
    psSPIs->pfSelect(FALSE);
}

void SPI_vDeselect(uint8 u8SpiIndex)
{
    SPI_tsSpi *psSPIs;
    psSPIs = &SPI_sCommon.psSPIs[u8SpiIndex];

    if (u8SpiIndex > SPI_sCommon.u8NumSPIs || psSPIs->pfSelect == NULL)
    {
        return;
    }

    /* pull-up pin latch */
    psSPIs->pfSelect(TRUE);
}

uint8 SPI_u8ExchangeByte(uint8 u8SpiIndex, uint8 u8Byte)
{
    SPI_tsSpi *psSPIs;
    psSPIs = &SPI_sCommon.psSPIs[u8SpiIndex];

    if (u8SpiIndex > SPI_sCommon.u8NumSPIs || psSPIs->pfXchgByte == NULL)
    {
        return 0xFF;
    }

    return psSPIs->pfXchgByte(u8Byte);
}
/****************************************************************************/
/***        Local Function                                                ***/
/****************************************************************************/

// #endif /*SPI_TOTAL_NUMBER*/
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
