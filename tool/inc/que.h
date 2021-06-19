#ifndef __QUE_H__
#define __QUE_H__

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "stm32f4xx.h"

#define QUE_NAME_MAX    (16)
typedef struct 
{
   char name[QUE_NAME_MAX];
   uint8_t     *pbuf;                            /*queue buffer pointer*/
   uint16_t    len;                              /*numbers of data in the queue buffer*/ 
   uint16_t    size;                             /*queue buffer size*/                  
   uint16_t    front;                            /*input index*/
   uint16_t    rear;                             /*output index*/
}Que_t;


void         CreateQue(char *name,Que_t *Que,uint16_t size) ;
void         DestroyQue(Que_t *Que) ;
uint8_t      IsQueueEmpty(Que_t *que);
uint8_t      IsQueueFull(Que_t *que);
uint8_t      EnQueue(Que_t *que, uint8_t data);
uint8_t      EnQueueEx(Que_t *que, uint8_t *buf, uint16_t size);
uint8_t      DeQueue(Que_t *que, uint8_t *pdata);
uint16_t      CpyQueData(Que_t *que, uint8_t *buf, uint16_t size);
uint8_t      DeQueueEx(Que_t *que,uint16_t size);
uint16_t     GetQueLength(Que_t *que);
void         FlushQueue(Que_t *que);
void         QueToBuff(uint8_t buffer[], Que_t *que, uint16_t size);
void         BufferToQue(Que_t *que, uint8_t buffer[], uint16_t size);
uint16_t     Que_data_read(Que_t * Que,uint8_t *pbuf);

#endif
