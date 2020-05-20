/****************************************************************************
 *
 * MODULE:    Queue.c
 *
 * DESCRIPTION:
 *
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
/***        Include files                                                 ***/
/****************************************************************************/

#include "chip_selection.h"
#include "Queue.h"
//#include "dbg.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifdef DEBUG_QUEUE
#define TRACE_QUEUE    TRUE
#else
#define TRACE_QUEUE    FALSE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

void QUEUE_vCreate ( tsQueue*       psQueueHandle, 
                    const uint32    u32QueueLength, 
                    const uint32    u32ItemSize, 
                    uint8*          pu8StartQueue )
{
        psQueueHandle->pvHead =  pu8StartQueue;
        psQueueHandle->u32ItemSize =  u32ItemSize;
        psQueueHandle->u32Length =  u32QueueLength;
        psQueueHandle->pvWriteTo =  psQueueHandle->pvHead;
        psQueueHandle->u32MessageWaiting =  0;
        psQueueHandle->pvReadFrom =  psQueueHandle->pvHead;
//        DBG_vPrintf(TRACE_QUEUE, "QUEUE: Initialised a queue: Handle=%08x Length=%d ItemSize=%d\n", (uint32)psQueueHandle, u32QueueLength, u32ItemSize);

}

bool_t QUEUE_bSend ( void*          pvQueueHandle, 
                    const void*    pvItemToQueue )
{

    tsQueue *psQueueHandle = (tsQueue *)pvQueueHandle;
    bool bReturn = FALSE;
    /*TODO: disable and save interrupt*/
    
    if(psQueueHandle->u32MessageWaiting >= psQueueHandle->u32Length)
    {
//        DBG_vPrintf(TRACE_QUEUE, "QUEUE: Queue overflow: Handle=%08x\n", (uint32)pvQueueHandle);
    }
    else
    {        
        if( psQueueHandle->pvWriteTo >= (psQueueHandle->pvHead+(psQueueHandle->u32Length*psQueueHandle->u32ItemSize)))
        {
             psQueueHandle->pvWriteTo = psQueueHandle->pvHead;
        }
        ( void ) memcpy( psQueueHandle->pvWriteTo, pvItemToQueue, psQueueHandle->u32ItemSize );
        psQueueHandle->u32MessageWaiting++;
        psQueueHandle->pvWriteTo += psQueueHandle->u32ItemSize;
        
        /* TODO: Increase power manager activity count */
        
        bReturn = TRUE;
    }
    /*TODO: restore interrupt*/
    
    return bReturn;
}

bool_t QUEUE_bReceive ( void*    pvQueueHandle, 
                        void*    pvItemFromQueue )
{
    tsQueue *psQueueHandle = (tsQueue *)pvQueueHandle;
    bool bReturn = FALSE;
    /*TODO: disable and save interrupt*/

    if( psQueueHandle->u32MessageWaiting >  0)
    {
        if( psQueueHandle->pvReadFrom >= (psQueueHandle->pvHead+(psQueueHandle->u32Length*psQueueHandle->u32ItemSize) ))
        {
            psQueueHandle->pvReadFrom = psQueueHandle->pvHead;
        }
        ( void ) memcpy( pvItemFromQueue, psQueueHandle->pvReadFrom, psQueueHandle->u32ItemSize );
        psQueueHandle->pvReadFrom += psQueueHandle->u32ItemSize;
        psQueueHandle->u32MessageWaiting--;
        
        /*TODO: Decrease power manager activity count */
        
        bReturn = TRUE;        
    }
    else
    {
        bReturn =  FALSE;
    }
    /*TODO: restore interrupt*/
    
    return bReturn;
}

bool_t QUEUE_bIsEmpty ( void*    pu8QueueHandle )
{
    tsQueue *psQueueHandle = (tsQueue *)pu8QueueHandle;
    bool bReturn = FALSE;
    /*TODO: disable and save interrupt*/

    if (psQueueHandle->u32MessageWaiting == 0)
    {
        bReturn = TRUE;
    }
    else
    {
        bReturn = FALSE;
    }
    /*TODO: restore interrupt*/

    return (bReturn);
}


uint32 QUEUE_u32GetQueueSize ( void*    pu8QueueHandle )
{
    tsQueue *psQueueHandle = (tsQueue *)pu8QueueHandle;
    return psQueueHandle->u32Length;
}

uint32 QUEUE_u32GetQueueMessageWaiting ( void*    pu8QueueHandle )
{
    tsQueue *psQueueHandle = (tsQueue *)pu8QueueHandle;
    return psQueueHandle->u32MessageWaiting;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
