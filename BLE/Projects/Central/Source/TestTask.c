/**
* UART Message structure 
* 1 byte(uint8)	- 1 bytes(uint8)	- x bytes	- 2 bytes  [ sum of bytes ==  msg length ]
* msg length    - msg UUID  		- message 	- \r\n
* 
* !!! whole msg should not contain terminating null-characters
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TestTask.h"
#include "hal_types.h"
#include "hal_adc.h"
#include "bcomdef.h"
#include "OSAL.h"
#include "npi.h"
#include "MyMsg.h"

#include "SIMPLEgattPROFILE.h"

#define TestTask_MeasureADC                               0x0001
#define TestTask_MeasureADC_Period                        200

//--------------- UART defs/vars
static void SetParameter(MyMsg_t* msg);
//---------------

extern bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value );
uint8 TestTask_ID;
void npiCBack ( uint8 port, uint8 event );

void TestTask_Init( uint8 task_id )
{
  NPI_InitTransport( npiCBack );
  TestTask_ID = task_id;
  HalAdcSetReference ( HAL_ADC_REF_AVDD );
  osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
 
}

uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events )
{
   osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );

  /* uint32 data = 123;
    SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &data );
    SimpleProfile_SetParameter( CURRENT_ID, CURRENT_LEN, &data );*/
    

  
   events = ( events ^ TestTask_MeasureADC);
   return events;//( events ^ TestTask_MeasureADC);
}

/* UART Events *//*
#define HAL_UART_RX_FULL         0x01
#define HAL_UART_RX_ABOUT_FULL   0x02
#define HAL_UART_RX_TIMEOUT      0x04
#define HAL_UART_TX_FULL         0x08
#define HAL_UART_TX_EMPTY        0x10
*/

void npiCBack ( uint8 port, uint8 event )
{  
  uint16 len = NPI_RxBufLen();
  if (len == 0) 
    return;
  char* tempBuffer = malloc (len);
  if (tempBuffer == NULL)
    return;
  
  
  len = NPI_ReadTransport((uint8*)tempBuffer, len );
  for (uint8 i = 0; i< len; i++){
    MyMsg_CacheStringPiece_ISR(*tempBuffer);
    tempBuffer++;
  }
  free(tempBuffer);
  
  
  MyMsg_t* msg = MyMsg_ProcessCache();
  if (msg != NULL)
  {
    printf("%d\n",msg->pData);
    //SetParameter(msg);
    free(msg->pData);
    free(msg);
  }
}



static void SetParameter(MyMsg_t* msg)
{
  int32 u32;
  switch(msg->UUID)
  {
  case BIKE_BATTERY_LEVEL_ID:
    u32 = *(int32*)(msg->pData);
    
   printf("%d\n",u32);
    SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &u32 );
   // printf("%li \n", u32);
    break;
  case CURRENT_ID:
    u32 = *(int32*)(msg->pData);
    SimpleProfile_SetParameter( CURRENT_ID, CURRENT_LEN, &u32 );
    break;
  }
}


	
	

