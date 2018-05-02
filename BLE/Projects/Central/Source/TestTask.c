#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TestTask.h"
#include "hal_types.h"
#include "hal_adc.h"
#include "bcomdef.h"
#include "OSAL.h"
#include "npi.h"
#include "OnBoard.h"
#include "hal_led.h"

#include "SIMPLEgattPROFILE.h"

#define TestTask_MeasureADC                               0x0001
#define TestTask_MeasureADC_Period                        500

//--------------- UART defs/vars
//---------------
extern bool Connected;
extern bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value );
static void SetParameter();
uint8 TestTask_ID;
void npiCBack ( uint8 port, uint8 event );

void TestTask_Init( uint8 task_id )
{
  NPI_InitTransport( npiCBack );
  TestTask_ID = task_id;
  HalAdcSetReference ( HAL_ADC_REF_AVDD );
  osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
 
}
uint8 UART_suffixCnt = 0;
uint8 UART_uuid = 0;
char UART_data[4] = {0};
void processUART()
{  
  uint16 len = NPI_RxBufLen();
  if (len != 0) 
  {
    char tempBuffer[NPI_UART_RX_BUF_SIZE];
    len = NPI_ReadTransport((uint8*)tempBuffer, len );
   
    
    for ( char* rxTail = tempBuffer; rxTail < &tempBuffer[len]; rxTail++){
      switch(UART_suffixCnt)
      {
      case 0:
          UART_suffixCnt = *rxTail == 'B' ? 1 : 0;
        break;
      case 1:
          UART_suffixCnt = *rxTail == 'e' ? 2 : 0;
        break;
      case 2:
          UART_suffixCnt = *rxTail == 'g' ? 3 : 0;
        break;
      case 3:
          UART_suffixCnt = *rxTail == 'i' ? 4 : 0;
        break;
      case 4:
          UART_suffixCnt = *rxTail == 'n' ? 5 : 0;
        break;
      case 5:
          UART_suffixCnt++;
          UART_uuid=*rxTail;
        break;
      case 6:
          UART_suffixCnt++;
          UART_data[0]=*rxTail;
          SetParameter();
        break;
      case 7:
          UART_suffixCnt++;
          UART_data[1]=*rxTail;
          SetParameter();
        break;
      case 8:
          UART_suffixCnt++;
          UART_data[2]=*rxTail;
          SetParameter();
        break;
      case 9:
          UART_suffixCnt++;
          UART_data[3]=*rxTail;
          SetParameter();
        break;
      default:
        UART_suffixCnt = 0;
        break;
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



static void SetParameter()
{ 
  uint8 isParameterSet = 0;
  //if (!Connected) return ;
  switch(UART_uuid)
  {
  case BIKE_BATTERY_LEVEL_ID:
    if (UART_suffixCnt == 6+BIKE_BATTERY_LEVEL_LEN)
    {
      SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &UART_data );
      isParameterSet = 1;
    }
    break;
  case CURRENT_ID:
    if (UART_suffixCnt == 6+CURRENT_LEN)
    {
      SimpleProfile_SetParameter( CURRENT_ID, CURRENT_LEN, &UART_data);
      isParameterSet = 1;
    }
    break;
  case PWM_DUTY_CYCLE_ID:
    if (UART_suffixCnt == 6+PWM_DUTY_CYCLE_LEN)
    {
      SimpleProfile_SetParameter( PWM_DUTY_CYCLE_ID, PWM_DUTY_CYCLE_LEN, &UART_data);
      isParameterSet = 1;
    }
    break;
  case BIKE_SPEED_ID:
    if (UART_suffixCnt == 6+BIKE_SPEED_LEN)
    {
      SimpleProfile_SetParameter( BIKE_SPEED_ID, BIKE_SPEED_LEN, &UART_data);
      isParameterSet = 1;
    }
    break;
  case BIKE_FLAGS_ID:
    if (UART_suffixCnt == 6+BIKE_FLAGS_LEN)
    {
      SimpleProfile_SetParameter( BIKE_FLAGS_ID, BIKE_FLAGS_LEN, &UART_data);
      isParameterSet = 1;
    }
    break;
  }
  
  
  
  if (isParameterSet)
  {
    UART_suffixCnt = 0;
    UART_uuid = 0;
    memset(UART_data, 0, 4);
  }
}

void Error_Handler()
{
  SystemReset();
}
	

