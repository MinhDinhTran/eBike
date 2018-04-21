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
#include "OnBoard.h"

#include "SIMPLEgattPROFILE.h"

#define TestTask_MeasureADC                               0x0001
#define TestTask_MeasureADC_Period                        50

//--------------- UART defs/vars
static void SetParameter(MyMsg_t* msg);
//---------------
extern bool Connected;
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
void processUART()
{  
  uint16 len = NPI_RxBufLen();
  if (len != 0) 
  {
    char tempBuffer[NPI_UART_RX_BUF_SIZE];
    if (tempBuffer != NULL)
    {
      len = NPI_ReadTransport((uint8*)tempBuffer, NPI_UART_RX_BUF_SIZE );
      for (uint8 i = 0; i< len; i++){
        MyMsg_CacheStringPiece_ISR(tempBuffer[i]);
      }
      
      MyMsg_t* msg = MyMsg_ProcessCache();
      if (msg != NULL)
      {
        SetParameter(msg);
        free(msg->pData);
        free(msg);
      }
    }
  }
}
uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events )
{
   osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );

  //processUART();
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
processUART();
}



static void SetParameter(MyMsg_t* msg)
{ 
  if (!Connected) return ;
  switch(msg->UUID)
  {
  case BIKE_BATTERY_LEVEL_ID:
    SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, (uint16*)(msg->pData) );
    break;
  case CURRENT_ID:
    SimpleProfile_SetParameter( CURRENT_ID, CURRENT_LEN, (uint16*)(msg->pData) );
    break;
  case BIKE_SPEED_ID:
    SimpleProfile_SetParameter( BIKE_SPEED_ID, BIKE_SPEED_LEN, (float*)(msg->pData) );
    break;
  case BIKE_FLAGS_ID:
    SimpleProfile_SetParameter( BIKE_FLAGS_ID, BIKE_FLAGS_LEN, (uint32*)(msg->pData) );
    break;
  }
}

void Error_Handler()
{
  SystemReset();
}
	

