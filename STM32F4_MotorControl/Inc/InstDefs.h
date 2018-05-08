#ifndef INSTDEFS_H_
#define INSTDEFS_H_

#define BT_UART huart3

#define TIM_PWM_ADC_V_DELAY htim9
#define TIM_PWM_ADC_V_DELAY_CHN TIM_CHANNEL_1
#define TIM_MC_WATCHDOG htim10
#define TIM_MC_WATCHDOG_CHN TIM_CHANNEL_1


#define ADC_V_INJ_RANK ADC_INJECTED_RANK_1
#define ADC_I_INJ_RANK ADC_INJECTED_RANK_2

#define ADC_INSTANCE_V  hadc1
#define ADC_INSTANCE_I  hadc2
#define ADC_INSTANCE_VBAT  hadc1

#define PWM_INSTANCE  htim1
#define BLE_INSTANCE  USART3
#define SD_INSTANCE  SPI2

#define GetADCIndex(ADC) ADC == ADC1 ? 0 : (ADC == ADC2 ? 2 : 1)
#endif /* INSTDEFS_H_ */
