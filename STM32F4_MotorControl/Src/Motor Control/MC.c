#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "string.h"
#include "stdlib.h"
#include <stdbool.h>
#include "stdio.h"
#include <stdint.h>
#include <SDCard_Buffer.h>
#include "InstDefs.h"
#include "MC.h"
#include "Bluetooth_Msg.h"

extern DAC_HandleTypeDef hdac;

osThreadId MotorControlThreadHandle;

extern TIM_HandleTypeDef PWM_INSTANCE;
extern TIM_HandleTypeDef TIM_PWM_ADC_V_DELAY;
extern ADC_HandleTypeDef ADC_INSTANCE_V1;
extern ADC_HandleTypeDef ADC_INSTANCE_V2;
extern ADC_HandleTypeDef ADC_INSTANCE_V3;
extern TIM_HandleTypeDef TIM_MC_WATCHDOG;

MotorControl_t MotorControl = {
		.ADC_V = { 0, 0, 0 },
		.DutyCycle = 20,
		.Wanted_DutyCycle = 20,
		.V_Treshold = 30,
		.Integral = 0,
		.Limits = { .Integral = 3000 // su ~50% duty cycle testuota
		},
		.Flags = { .ClosedLoop = 1 },
		.PWM_Switching = { .IsRisingFront = 1, .ActiveSequence = PWMSequencesNotInit, .UseComplementaryPWM = 0, .UsePWMOnPWMN = 0 },
		.pwmCountToChangePhase = 25, //355;
		.PID = { .Kp = 0.35, .Ki = -0.01, .Kd = -0.05 } }; //{ .Kp = 0.3, .Ki = -0.07, .Kd = -0.1 } }; // { .Kp = 0.3, .Ki = -0.07, .Kd = -0.1

static uint32_t GetActualBEMF();
static void Integrate();

extern void ChangePhase(void);
static void MotorControlThread(void const * argument);

static uint8_t processUserBtn = 0;

void Start_MotorControlThread(void) {

	osThreadDef(MotorControlThread, MotorControlThread, osPriorityHigh, 0, 512); // definition and creation of MotorControlThread
	MotorControlThreadHandle = osThreadCreate(osThread(MotorControlThread), NULL);

	if (MotorControlThreadHandle == NULL)
		Error_Handler();
}
void MotorControlThread(void const * argument) // MotorControlThread function
{
	arm_pid_init_f32(&MotorControl.PID, 1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
	UNUSED(argument);
	HAL_ADC_Start_IT(&ADC_INSTANCE_VBAT);
	osDelay(1000);
	ChangePWMSwitchingSequence(Regeneration);
	for (;;) {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(500);

		if (processUserBtn) {

			if (MotorControl.PWM_Switching.ActiveSequence != ForwardCommutation)
				ChangePWMSwitchingSequence(ForwardCommutation);
			else
				ChangePWMSwitchingSequence(Regeneration);

			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
			osDelay(200);

			processUserBtn = 0;
		}
		MyMsg_t *msg = malloc(sizeof(MyMsg_t));
		msg->UUID = BIKE_BATTERY_LEVEL_ID;
		*(uint16_t*)msg->pData = MotorControl.ADC_VBAT;
		msg->length = BIKE_BATTERY_LEVEL_LEN;
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(500);

		msg = malloc(sizeof(MyMsg_t));
		msg->UUID = CURRENT_ID;
		*(uint16_t*)msg->pData = MotorControl.ADC_I[0];
		msg->length = CURRENT_LEN;
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(500);

		msg = malloc(sizeof(MyMsg_t));
		msg->UUID = BIKE_SPEED_ID;
		*(float*)msg->pData = MotorControl.RPM;
		msg->length = BIKE_SPEED_LEN;
		xQueueSendFromISR(xQueueTX, (void * ) &msg, (TickType_t ) 0);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(500);

	}
}

void Tim10PulseFinished() {
	if (HAL_TIM_OC_Stop_IT(&TIM_MC_WATCHDOG, TIM_MC_WATCHDOG_CHN) != HAL_OK)
		Error_Handler();
	//MotorControl.Flags.ClosedLoop = 0;
}

void Tim9PulseFinished(void) {
	if (HAL_TIM_OC_Stop_IT(&TIM_PWM_ADC_V_DELAY, TIM_PWM_ADC_V_DELAY_CHN) != HAL_OK)
		Error_Handler();
	switch (MotorControl.pwm_phase) {
	case 0:
	case 3:
		HAL_ADCEx_InjectedStart_IT(&ADC_INSTANCE_V1); // ADC_CHANNEL_11;	// PE9-10 Channel1
		break;
	case 2:
	case 5:
		HAL_ADCEx_InjectedStart_IT(&ADC_INSTANCE_V2); //ADC_CHANNEL_12;	// PE11-12 Channel2
		break;
	case 1:
	case 4:
		HAL_ADCEx_InjectedStart_IT(&ADC_INSTANCE_V3); //ADC_CHANNEL_1;	// PE13-14 Channel3
		break;
	}

	HAL_ADC_Start_IT(&ADC_INSTANCE_VBAT);
}

void OnPWMTriggeredEXT(void) {
	if (HAL_GPIO_ReadPin(GPIO_PWM_TRIGGER_GPIO_Port, GPIO_PWM_TRIGGER_Pin) == GPIO_PIN_SET) {
		if (HAL_TIM_OC_Start_IT(&TIM_PWM_ADC_V_DELAY, TIM_PWM_ADC_V_DELAY_CHN) != HAL_OK)
			Error_Handler();
	} else {
		MotorControl.pwmCountThisPhase++;
		if (!MotorControl.Flags.ClosedLoop && MotorControl.pwmCountThisPhase >= MotorControl.pwmCountToChangePhase) {
			ChangePhase();
		}
		Integrate();
	}
}
void OnPWM_ADC_Measured(ADC_HandleTypeDef* hadc) {
	uint8_t index = GetADCIndex(hadc->Instance);
	MotorControl.ADC_V[index] = HAL_ADCEx_InjectedGetValue(hadc, ADC_V_INJ_RANK);
	MotorControl.ADC_I[index] = HAL_ADCEx_InjectedGetValue(hadc, ADC_I_INJ_RANK);

	Buffer_AddValue(MotorControl.ADC_V[index], MotorControl.ADC_I[index]);
}

void OnVBAT_ADC_Measured(ADC_HandleTypeDef* hadc) {
	MotorControl.ADC_VBAT = HAL_ADC_GetValue(hadc);

	/*if (MotorControl.ADC_VBAT > 2743) //2743=42V | 2600 = +/-40V
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	 else
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);*/
}

void OnButtonClick(void) {
	processUserBtn = 1;
}

static uint32_t GetActualBEMF() {
	switch (MotorControl.pwm_phase) {
	case 0:
	case 3:
		return MotorControl.ADC_V[1]; // ADC_CHANNEL_11;	// PE9-10 Channel1
	case 2:
	case 5:
		return MotorControl.ADC_V[2]; //ADC_CHANNEL_12;	// PE11-12 Channel2
	case 1:
	case 4:
		return MotorControl.ADC_V[0]; //ADC_CHANNEL_1;	// PE13-14 Channel3
	}
	return 0;
}

static void Integrate() {
	uint16_t midPoint = 0;
	uint32_t PWM_Lv = 0;
	switch (MotorControl.PWM_Switching.ActiveSequence) {
	case PWMSequencesNotInit:
		break;
	case ForwardCommutation:
		midPoint = (uint16_t) (MotorControl.ADC_VBAT / 2);
		PWM_Lv = GetActualBEMF();
		if (PWM_Lv < 40)
			return;
		if (midPoint < 200)
			return;
		if (MotorControl.pwmCountThisPhase > 400) {
			ChangePhase();
			return;
		}
		if (MotorControl.PWM_Switching.IsRisingFront && PWM_Lv > midPoint) {
			MotorControl.Integral += (uint64_t) (PWM_Lv - midPoint);
		} else if (!MotorControl.PWM_Switching.IsRisingFront && PWM_Lv < midPoint) {
			MotorControl.Integral += (uint64_t) (midPoint - PWM_Lv);
		}

		if (MotorControl.Flags.ClosedLoop) {
			if (MotorControl.Integral >= MotorControl.Limits.Integral) {
				ChangePhase();
			}
		}
		break;
	case FreeWheeling:
		break;
	case Regeneration:

		break;
	default:
		break;
	}
}
void OnPhaseChanged() {
	switch (MotorControl.PWM_Switching.ActiveSequence) {
	case PWMSequencesNotInit:
		break;
	case ForwardCommutation:
		ChangePWMDutyCycle(MotorControl.Wanted_DutyCycle, 2);
		if (MotorControl.pwm_phase == 0) {
			Buffer_Init();
			if (HAL_TIM_OC_Stop_IT(&TIM_MC_WATCHDOG, TIM_MC_WATCHDOG_CHN) != HAL_OK)
				Error_Handler();
			MotorControl.RPM = (float) 2500000 / TIM_MC_WATCHDOG.Instance->CNT; // (168MHz/168) / x * 60 / 24;

			htim10.Instance->CNT = 0;
			if (HAL_TIM_OC_Start_IT(&TIM_MC_WATCHDOG, TIM_MC_WATCHDOG_CHN) != HAL_OK)
				Error_Handler();
			if (MotorControl.RPM > 1000 || MotorControl.RPM < 0)
				MotorControl.RPM = 0;

			MotorControl.Limits.Integral = MotorControl.V_Treshold * 100;
		}

		/*if (!MotorControl.Flags.ClosedLoop) {
		 phaseSwitchCount++;

		 if (phaseSwitchCount > 10)
		 MotorControl.Flags.ClosedLoop = 1;
		 }*/
		// Check if ready to switch to closed loop.
		/*if (!MotorControl.Flags.ClosedLoop) {
		 if (MotorControl.Integral >= MotorControl.Limits.Integral / 4) {
		 phaseSwitchCount++;
		 } else
		 phaseSwitchCount = 0;
		 if (phaseSwitchCount > 3)
		 MotorControl.Flags.ClosedLoop = 1;
		 }*/
		break;
	case FreeWheeling:
		break;
	case Regeneration:
		/*if (MotorControl.Wanted_DutyCycle != MotorControl.DutyCycle)
		 ChangePWMDutyCycle(MotorControl.Wanted_DutyCycle, 5);*/
		break;
	default:
		break;
	}
	//HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, MotorControl.pwm_phase * 600);

	MotorControl.Integral = 0;
	MotorControl.PWM_Switching.IsRisingFront = !(MotorControl.pwm_phase % 2);
	if (MotorControl.PWM_Switching.IsRisingFront)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	MotorControl.pwmCountThisPhase = 0;
}
void ChangePWMDutyCycle(uint8_t newDutyCycle, int8_t maxStep) {
	if (MotorControl.DutyCycle == newDutyCycle) {
		return;
	}
	if (newDutyCycle < 5)
		newDutyCycle = 5;
	if (newDutyCycle > 95)
		newDutyCycle = 95;
	int8_t diff = newDutyCycle - MotorControl.DutyCycle;
	if (diff > 0 && diff > maxStep)
		diff = maxStep;
	else if (diff < 0 && diff < -maxStep)
		diff = -maxStep;

	uint8_t dutyCycleToSet = MotorControl.DutyCycle + diff;

	//uint8_t dutyCycleToSet = newDutyCycle;
	__HAL_TIM_SET_COMPARE(&PWM_INSTANCE, TIM_CHANNEL_1, dutyCycleToSet * 10);
	__HAL_TIM_SET_COMPARE(&PWM_INSTANCE, TIM_CHANNEL_2, dutyCycleToSet * 10);
	__HAL_TIM_SET_COMPARE(&PWM_INSTANCE, TIM_CHANNEL_3, dutyCycleToSet * 10);

	MotorControl.DutyCycle = dutyCycleToSet;

}

