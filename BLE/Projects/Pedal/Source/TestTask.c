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

#include "customPedalProfile.h"

#define TestTask_MeasureADC                               0x0001
#define TestTask_MeasureADC_Period                        50

uint16 AdcData = 0;

uint16 countWhenMeasureBattery = 0;
extern bool Connected;

extern bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value );
uint8 TestTask_ID;


void TestTask_Init( uint8 task_id )
{
  TestTask_ID = task_id;
  HalAdcSetReference ( HAL_ADC_REF_AVDD );//
  osal_start_reload_timer( TestTask_ID, TestTask_MeasureADC, TestTask_MeasureADC_Period );
 
}
uint16 lastSentData = 0;
uint16 TestTask_ProcessEvent( uint8 task_id, uint16 events )
{
  if (Connected) 
  {
  //  AdcData[dataCount] = HalAdcRead ( HAL_ADC_CHANNEL_1, HAL_ADC_RESOLUTION_12);
  //  dataCount = (dataCount + 1 )%DataCountToAVG;
  // /* if (dataCount == DataCountToAVG)
  //  {*/
  /*    uint32 AdcDataSUM = 0;
      for(uint8 i = 0; i < DataCountToAVG; i++)
        AdcDataSUM += AdcData[i];

      AdcDataAVG = AdcDataSUM / DataCountToAVG;
      if (lastSentData+25 < AdcDataSUM  || lastSentData-25 > AdcDataSUM)
      {
        lastSentData = AdcDataSUM;
        customPedalProfile_SetParameter( RAW_DATA_ID, RAW_DATA_LEN, &AdcDataAVG );
      }

        */
        
        
    AdcData = HalAdcRead ( HAL_ADC_CHANNEL_1, HAL_ADC_RESOLUTION_12);
    customPedalProfile_SetParameter( RAW_DATA_ID, RAW_DATA_LEN, &AdcData);
        
        
        
    //}
    
   /* if (countWhenMeasureBattery++ >= 100)
    {
      countWhenMeasureBattery = 0;
      uint16 vbat = (uint16)(HalAdcRead ( HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_12)*3);
      customPedalProfile_SetParameter( BATTERY_LV_ID, BATTERY_LV_LEN, &vbat );
    }*/
  }
  
  //  BATTERY_LV_ID
  
  
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