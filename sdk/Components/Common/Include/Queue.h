/****************************************************************************
 *
 * MODULE: Queue.h
 *
 * DESCRIPTION:
 * MODIFY:             giauna
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

#ifndef QUEUE_H_
#define QUEUE_H_

//#include "mcu"
#include "string.h"


typedef struct
{
    uint32 u32Length;                    /*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
    uint32 u32ItemSize;                /*< The size of each items that the queue will hold. */
    uint32 u32MessageWaiting;
    void  *pvHead;                    /*< Points to the beginning of the queue storage area. */
    void  *pvWriteTo;                    /*< Points to the free next place in the storage area. */
    void  *pvReadFrom;                /*< Points to the free next place in the storage area. */
}tsQueue;

void QUEUE_vCreate (tsQueue *psQueueHandle, const uint32 uiQueueLength, const uint32 uiItemSize, uint8* pu8StartQueue);
bool_t QUEUE_bSend(void *pvQueueHandle, const void *pvItemToQueue);
bool_t QUEUE_bReceive(void *pvQueueHandle, void *pvItemFromQueue);
bool_t QUEUE_bIsEmpty(void *pvQueueHandle);
uint32 QUEUE_u32GetQueueSize(void *pvQueueHandle);
uint32 QUEUE_u32GetQueueMessageWaiting ( void*    pu8QueueHandle );
#endif /*QUEUE_H_*/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
