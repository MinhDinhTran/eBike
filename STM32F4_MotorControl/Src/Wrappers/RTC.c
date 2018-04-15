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

	if (xQueueTX != NULL)
	{

		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		Bluetooth_MSG_t *msg = malloc(sizeof(*msg) + sizeof(msg->MSG[0]));
		msg->UUID = BIKE_BATTERY_LEVEL_ID;
		msg->length = 1;
		msg->MSG[0] = MotorControl.ADC_VBAT;

		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);
	}
}
