#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "MC.h"
#include "Bluetooth_Msg.h"

#include "string.h"
#include "stdlib.h"
#include <stdbool.h>
#include "stdio.h"
#include <stdint.h>

__weak void OnButtonClick(void) {
}
extern void OnPWMTriggeredEXT();

extern void CruiseControl_Reset();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_0:
		OnButtonClick();
		break;
	case GPIO_PIN_1:
		CruiseControl_Reset();
		uint32_t flag = 0;
		if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == GPIO_PIN_SET)
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
			MotorControl.Flags.OverCurrent = 1;
			flag = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
			MotorControl.Flags.OverCurrent = 0;
		}

		MyMsg_t *msg = malloc(sizeof(MyMsg_t));
		msg->UUID = PWM_DUTY_CYCLE_ID;
		*(uint32_t*)msg->pData = 20;
		msg->length = PWM_DUTY_CYCLE_LEN;
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);

		msg = malloc(sizeof(MyMsg_t));
		msg->UUID = BIKE_FLAGS_ID;
		*(uint32_t*)msg->pData = flag;
		msg->length = BIKE_FLAGS_LEN;
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);


		break;
	case GPIO_PIN_10:
		OnPWMTriggeredEXT();
		break;
	case GPIO_PIN_12:
	case GPIO_PIN_14:
		break;
	case GPIO_PIN_15:
		break;
	default:
		break;
	}
}

