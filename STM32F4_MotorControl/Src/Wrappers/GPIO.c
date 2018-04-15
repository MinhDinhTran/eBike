#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

__weak void OnButtonClick(void) {
}
extern void OnPWMTriggeredEXT();

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case GPIO_PIN_0:
		OnButtonClick();
		break;
	case GPIO_PIN_1:
		if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) == GPIO_PIN_SET)
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);

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

