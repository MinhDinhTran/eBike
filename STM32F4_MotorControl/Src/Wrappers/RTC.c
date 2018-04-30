#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "Bluetooth_Msg.h"
#include "MC.h"
uint8_t RTC_Prescaler = 0;
extern QueueHandle_t xQueueTX;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{

    if (RTC_Prescaler++ < 5)
    	return;
    RTC_Prescaler = 0;

/*	if (xQueueTX != NULL)
	{

		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);

		MyMsg_t* msg = MyMsg_CreateString(BIKE_BATTERY_LEVEL_ID, &MotorControl.ADC_VBAT, sizeof(uint16_t));
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);
	}*/
}
