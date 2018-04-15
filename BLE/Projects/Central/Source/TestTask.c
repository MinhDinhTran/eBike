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

#include "SIMPLEgattPROFILE.h"

#define TestTask_MeasureADC                               0x0001
#define TestTask_MeasureADC_Period                        200

//--------------- UART defs/vars
#define UART_LEN_OFFSET 1
#define UART_SUFFIX_SIZE 2
#define UART_ID_SIZE 1
#define UART_BUFFER_SIZE 128
static void SetParameter(uint16 ID, char* data, uint8 len);
const uint16 UART_Suffix = 0x0D0A; // aka "\r\n".
char UART_buffer[UART_BUFFER_SIZE] = {0};
uint16 UART_RxActivePos = 0;
//---------------

extern bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value );
uint8 TestTask_ID;
uint16 ADCMeasureNo = 0;
uint16 buffer[5] = {0};
uint16 bufferIndex = 0;
void npiCBack ( uint8 port, uint8 event );

void TestTask_Init( uint8 task_id )
{
  NPI_InitTransport( npiCBack );
  TestTask_ID = task_id;
   return;
  HalAdcSetReference ( HAL_ADC_REF_AVDD );
  osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
 
}


uint16 adc0;
char adc[5];
uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events )
{
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_2, HAL_ADC_RESOLUTION_12 );
   ADCMeasureNo++;
   buffer[bufferIndex++%5] = adc0;
   //printf("%d, ", (buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4])/5);
   if (ADCMeasureNo > 9)
   {
     ADCMeasureNo = 0;
     uint32 aa = adc0;
     SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &aa );
   //  printf("%d\n",aa );
   }
   // sprintf ( adc , "%d", adc0);
    //SimpleProfile_SetParameter( ADC_ID, ADC_LEN, &adc );
 /*   char buf[50];
      buf[0] = 8;
      buf[1] = 0xf;
      buf[2] = 0;
      buf[3] = 0;   
      memcpy(&buf[4], &adc0, 2);
      buf[6] = '\r';
      buf[7] = '\n';
      NPI_WriteTransport((uint8 *)buf, 8 );*/
      
      
   events = ( events ^ TestTask_MeasureADC);
   osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
   return events;//( events ^ TestTask_MeasureADC);
}



void npiCBack ( uint8 port, uint8 event )
{  
  uint8 len = NPI_RxBufLen();
  uint16 lenToRead = ((UART_RxActivePos + len) < UART_BUFFER_SIZE) ? len : (UART_BUFFER_SIZE - UART_RxActivePos);
  NPI_ReadTransport((uint8*)&UART_buffer[UART_RxActivePos], lenToRead);
  UART_RxActivePos += lenToRead;
  char* ptr = (char*) memchr(UART_buffer, UART_Suffix, UART_RxActivePos);
  while (ptr != NULL)
  {
    ptr--; // because it returns pointer to \n .... need \r location. which is before it..
    uint16 dataLen = ptr - UART_buffer + UART_SUFFIX_SIZE;
    
    if (UART_RxActivePos < dataLen)
      break;
    
    uint8 UART_dataLength = UART_buffer[0]; 
    if (dataLen == UART_dataLength)
    {
      SetParameter(UART_buffer[UART_LEN_OFFSET], &UART_buffer[UART_LEN_OFFSET + UART_ID_SIZE], dataLen - UART_LEN_OFFSET - UART_SUFFIX_SIZE - UART_ID_SIZE);
    }
    memmove(UART_buffer, ptr + UART_SUFFIX_SIZE, UART_BUFFER_SIZE-dataLen);
    UART_RxActivePos -= dataLen;
    ptr = (char*) memchr(UART_buffer, UART_Suffix, UART_RxActivePos);
  }
}


static void SetParameter(uint16 ID, char* data, uint8 len)
{
  int32 u32;
  switch(ID)
  {
  case BIKE_BATTERY_LEVEL_ID:
    u32 = *(int32*)data;
    SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &u32 );
   // printf("%li \n", u32);
    break;
  case CURRENT_ID:
    u32 = *(int32*)data;
    SimpleProfile_SetParameter( CURRENT_ID, CURRENT_LEN, &u32 );
    break;
  }
}

