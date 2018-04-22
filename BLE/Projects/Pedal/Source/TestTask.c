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
//static void SetParameter(uint16 ID, char* data, uint8 len);
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
  TestTask_ID = task_id;
  HalAdcSetReference ( HAL_ADC_REF_AVDD );//HAL_ADC_REF_AVDD );//
  osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
 
}


uint16 adc0;
uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events )
{
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_0, HAL_ADC_RESOLUTION_12);
      printf ( "0-%d\n", adc0);
   /*adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_1, HAL_ADC_RESOLUTION_12);
      printf ( "1-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_2, HAL_ADC_RESOLUTION_12);
      printf ( "2-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_3, HAL_ADC_RESOLUTION_12);
      printf ( "3-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_12);
      printf ( "4-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_5, HAL_ADC_RESOLUTION_12);
      printf ( "5-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_6, HAL_ADC_RESOLUTION_12);
      printf ( "6-%d\n", adc0);
   adc0 =  HalAdcRead ( HAL_ADC_CHANNEL_7, HAL_ADC_RESOLUTION_12);
      printf ( "7-%d\n", adc0);*/
   
   

   //printf("%d, ", (buffer[0] + buffer[1] + buffer[2] + buffer[3] + buffer[4])/5);


   events = ( events ^ TestTask_MeasureADC);
   osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
   return events;//( events ^ TestTask_MeasureADC);
}



/*
static void SetParameter(uint16 ID, char* data, uint8 len)
{
  
  //long int num = strtol (data, NULL,10);
  //if (num > 4096) printf("%wtf SetParameter\n");
  int32 u32;
  //int32 u32m[10];
  switch(ID)
  {
  case BIKE_BATTERY_LEVEL_ID:
    u32 = *(int32*)data;
    SimpleProfile_SetParameter( BIKE_BATTERY_LEVEL_ID, BIKE_BATTERY_LEVEL_LEN, &u32 );
   // printf("%li \n", u32);
    break;
  }
}

*/