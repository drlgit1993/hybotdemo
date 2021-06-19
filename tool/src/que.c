#include "que.h"
#include "FreeRTOS.h"
#include "task.h"

void CreateQue(char *name,Que_t *Que,uint16_t size) 
{
   Que->pbuf = (uint8_t *)malloc(size);
   if (Que->pbuf == NULL) 
   {
       USART6_printf("ERROR:%s que create fail\r\n",name);
       return ;
   }
   strncpy(Que->name,name,QUE_NAME_MAX);
   memset(Que->pbuf, 0, size);
   Que->front = 0;
   Que->rear = 0;
   Que->size = size;
   Que->len = 0;
}

void DestroyQue(Que_t *Que) 
{
   if (Que->pbuf == NULL) 
   {
       USART6_printf("ERROR:%s que is not created\r\n",Que->name);
       return ;
   }
   free(Que->pbuf);
   Que->pbuf =NULL;
   memset(Que->name, 0,QUE_NAME_MAX);
   Que->front = 0;
   Que->rear = 0;
   Que->size = 0;
   Que->len = 0;
}

/******************************************************************************
*@fn        IsQueueEmpty
*
*@brief     judge wether queue is empty 
*
*@param     Que_t :Identify Of Queue
*
*@return    true :Queue is empty, or else it is not empty
*
*****************************************************************************/
uint8_t IsQueueEmpty(Que_t *que) 
{
  if (que->len == 0) 
  {
    USART6_printf("WARNING:Que %s is empty\r\n",que->name);
    return 1;
  } 
  else 
  {
    return 0;
  }
}


/******************************************************************************
*@fn        IsQueueFull
*
*@brief     judge wether queue is full 
*
*@param     Que_t :Identify Of Queue 
*
*@return    true :Queue is full, or else it is not full
*
*****************************************************************************/
uint8_t IsQueueFull(Que_t *que) 
{
  if (que->size == que->len) 
  {
    USART6_printf("ERROR:Que %s is full\r\n",que->name);
    return 1;
  } 
  else 
  {
    return 0;
  }
}

/******************************************************************************
*@fn        EnQueue
*
*@brief     insert data to the queue one bytes by byte
*
*@param     Que_t :Identify Of Queue 
*
*@return    0:queue is full and data can't be insert to queue
*           1:data has been inserted to the queue
*
*****************************************************************************/
uint8_t EnQueue(Que_t *que, uint8_t data) 
{
  if (que->pbuf == NULL) 
  {
    return 0;
  } 
  
  if (IsQueueFull(que)) 
  {
    return 0;
  }
  
  /*append data to the queue buffer*/
  que->pbuf[que->rear] = data;       
  /*make rear point to the next data position*/
  que->rear = (que->rear + 1) % que->size;      
  que->len++;
  
  return 1;
}

/******************************************************************************
*@fn        EnQueueEx
*
*@brief     insert datas to the queue by multi bytes
*           if there is no enugh space in the circular buffer, the operation will
            be faild.
*@param     Que_t :Identify Of Queue 
*
*           buf:datas buffer to be inserted
*           size:data length to be inserted
*@return    0:queue is full and data can't be insert to queue
*           1:data has been inserted to the queue
*
*****************************************************************************/


uint8_t EnQueueEx(Que_t *que, uint8_t *buf, uint16_t size) 
{
  uint16_t offset = 0;

  if (que->pbuf == NULL) 
  {
      return 0;
  } 
  
  if (IsQueueFull(que)) 
  {
      return 0;
  }
  
  if (que->len + size > que->size) 
  {
      USART6_printf("ERROR:%s Queue has not enough space,%d,%d,%d\r\n",que->name,que->len,size,que->size);
      return 0;
  }

  if (que->front <= que->rear) 
  {
    if (que->rear + size > que->size) 
    {
      offset = que->size - que->rear;
      memcpy(que->pbuf + que->rear, (uint8_t *)buf, offset);
      que->len += offset;
      que->rear += offset;
      que->rear %= que->size;
      memcpy(que->pbuf + que->rear, (uint8_t *)buf + offset,  size - offset);
      que->len += (size - offset);
      que->rear += (size - offset);
      que->rear %= que->size;
    } 
    else 
    {
      memcpy(que->pbuf + que->rear, (uint8_t *)buf, size);
      que->len += size;
      que->rear += size;
      que->rear %= que->size;
    } 
  } 
  else 
  {
    memcpy(que->pbuf + que->rear, (uint8_t *)buf, size);
    que->len += size;
    que->rear %= que->size;
    que->rear += size;
    que->rear %= que->size;
  }

  return 1;
}

/******************************************************************************
*@fn        DeQueue
*
*@brief     remove a byte from the queue
*           
*@param     Que_t :Identify Of Queue 
*           pdata: the data to be removed
*@return    0:queue is full and data can't be insert to queue
*           1:data has been inserted to the queue
*
*****************************************************************************/
uint8_t DeQueue(Que_t *que, uint8_t *pdata) 
{
  if (que == NULL) 
  {
    return 0;
  }
  
  if (IsQueueEmpty(que)) 
  {
    return 0;
  }
  
  /*get data */
  *pdata = que->pbuf[que->front];           
  /*make the front point to the next data to be read position*/
  que->front = (que->front + 1) % que->size;    
  /*decrece the numbers of the data in the queue buffer*/
  que->len--;                                   
  
  return 1;
}

/******************************************************************************
*@fn        CpyQueData
*
*@brief     copy the datas to buffer without remove the datas in the circular buffer
*
*@param     Que_t :Identify Of Queue 
*
*           buf:datas buffer to be inserted
*           size:data length to be inserted
*@return    0:param error or queue is empty
*           >0: copy data ok .and return value is the real size of copied datas
*
*****************************************************************************/
uint16_t CpyQueData(Que_t *que, uint8_t *buf, uint16_t size) 
{
  uint16_t rSize,len,front,rear;

  if ((que == NULL) || (NULL == buf))
  {
    return 0;
  }
  
  if (IsQueueEmpty(que)) 
  {
    return 0;
  }
  
  len=que->len;
  front=que->front;
  rear=que->rear;

  rSize = ((size <= len) ? size : len);

  if (rSize == 0) 
  {
    return 0;
  }

  if (front < rear) 
  {
    memcpy((uint8_t *)buf, que->pbuf + front, rSize);
  } 
  else 
  {
    if (rSize <= que->size - front) 
    {
      memcpy((uint8_t *)buf, que->pbuf + front, rSize);
    } 
    else 
    {
      memcpy((uint8_t *)buf, que->pbuf + front, que->size - front);
      memcpy((uint8_t *)buf + que->size - front, que->pbuf, rSize + front - que->size);
    }
  }
  return rSize;
}

/******************************************************************************
*@fn        DeQueueEx
*
*@brief     Remove datas to buffer
*           it often used with CpyQueData() function
*@param     Que_t :Identify Of Queue 
*           size:data length to be inserted
*@return    0:param error or queue is empty
*           1: remove ok
*
*****************************************************************************/
uint8_t DeQueueEx(Que_t *que, uint16_t size) 
{
  uint16_t rSize = 0;

  if (que == NULL) 
  {
    return 0;
  }
  
  if (IsQueueEmpty(que)) 
  {
    return 0;
  }

  rSize = ((size <= que->len) ? size : que->len);
  
  if (rSize == 0) 
  {
    return 0;
  }
  
  que->front += rSize;
  que->front %= que->size;
  que->len -= rSize;

  return 1;
}
uint16_t Que_data_read(Que_t * Que,uint8_t *pbuf)
{
  uint16_t len=0;

  if((NULL == Que) || (NULL == pbuf))
  {
    return 0;
  }
		
  len=Que->len;

  if(0 == len)
	 {
			 return 0;
	 }
  len=CpyQueData(Que,pbuf,len);

  return len;
}
/******************************************************************************
*@fn        GetQueLength
*
*@brief     get the current length of the queue
*           
*@param     Que_t :Identify Of Queue 
*           size:data length to be inserted
*@return    0:param error
*           >0: length of the queue
*
*****************************************************************************/
uint16_t GetQueLength(Que_t *que) 
{
  if (que == NULL) 
  {
    return 0;
  }
  
  return que->len;
}

void FlushQueue(Que_t *que) 
{
  if (que == NULL) 
  {
    return ;
  }
  
  memset(que->pbuf, 0, que->size);
  que->front = 0;
  que->rear = 0;
  que->len = 0;
}

void QueToBuff(uint8_t *buf, Que_t *que, uint16_t size)
{
  uint16_t i = 0;
  
  for (i = 0; i < size;i++)
  {
    DeQueue(que, buf + i);
  }
}

void BufferToQue(Que_t *que, uint8_t *buf, uint16_t size)
{
  uint16_t i = 0;
  
  for (i = 0; i < size;i++)
  {
    EnQueue(que,buf[i]);
  }
}
