#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

__weak void Tim9PulseFinished(){}
__weak void Tim10PulseFinished(){}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM10)
	{
		Tim10PulseFinished();
	}
}
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM9)
	{
		Tim9PulseFinished();
	}
}

