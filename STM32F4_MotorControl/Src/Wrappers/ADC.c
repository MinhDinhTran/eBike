#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"


extern void OnVBAT_ADC_Measured(ADC_HandleTypeDef* hadc);
extern void OnPWM_ADC_Measured(ADC_HandleTypeDef* hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC2)
		OnVBAT_ADC_Measured(hadc);
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	OnPWM_ADC_Measured(hadc);
}
