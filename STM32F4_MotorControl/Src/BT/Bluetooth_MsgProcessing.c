#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"
#include "MC.h"
#include "InstDefs.h"

//extern TIM_HandleTypeDef htim9;

void ProccesReceivedMessage(Bluetooth_MSG_t *msg) {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	switch (msg->UUID) {
	case PWM_DUTY_CYCLE_ID:
		MotorControl.Wanted_DutyCycle = (uint8_t) (msg->MSG[0] >> 16);
		break;
	case V_THRESHOLD_ID:
		MotorControl.V_Treshold = msg->MSG[0];
		if (MotorControl.V_Treshold > 5 && MotorControl.V_Treshold < 100)
			MotorControl.Limits.Integral = MotorControl.V_Treshold * 100;
		break;
	}
}

