#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "InstDefs.h"
#include "PWM_Switching.h"

static const PWMActions ForwardCommutationSequence[6][3] = {
		{ OnH, None, OffH },
		{ None, OffL, OnL },
		{ OffH, OnH, None },
		{ OnL, None, OffL },
		{ None, OffH, OnH },
		{ OffL, OnL, None } };

static const PWMActions FreeWheelingSequence[6][3] = {
		{ None, None, None },
		{ None, OffL, OnL },
		{ None, None, None },
		{ OnL, None, OffL },
		{ None, None, None },
		{ OffL, OnL, None } };

static const PWMActions (*ActiveSequence)[6][3] = &ForwardCommutationSequence;

static void Enable_State(PWMActions action, uint32_t Channel);
static void Set_PWM_CHN(PWMStates state, uint32_t Channel);
static void Set_PWM_CHN1N(PWMStates state, uint32_t Channel);
static void Set_PWM_CHN1N_GPIO(PWMStates state, uint32_t Channel);
static void ChangePWMPinType();
__weak void OnPhaseChanged(void) {
}

static void ChangePWMPinType() {
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	GPIO_InitStruct.Mode =
			MotorControl.PWM_Switching.UsePWMOnPWMN ? GPIO_MODE_AF_PP : GPIO_MODE_OUTPUT_PP;

	GPIO_InitStruct.Pin = PWMN_CH1_Pin;
	HAL_GPIO_Init(PWMN_CH1_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	GPIO_InitStruct.Mode =
			MotorControl.PWM_Switching.UsePWMOnPWMN ? GPIO_MODE_AF_PP : GPIO_MODE_OUTPUT_PP;

	GPIO_InitStruct.Pin = PWMN_CH2_Pin;
	HAL_GPIO_Init(PWMN_CH2_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	GPIO_InitStruct.Mode =
			MotorControl.PWM_Switching.UsePWMOnPWMN ? GPIO_MODE_AF_PP : GPIO_MODE_OUTPUT_PP;

	GPIO_InitStruct.Pin = PWMN_CH3_Pin;
	HAL_GPIO_Init(PWMN_CH3_GPIO_Port, &GPIO_InitStruct);
}

static void Enable_State(PWMActions action, uint32_t Channel) {
	switch (action) {
	case OnH:
		Set_PWM_CHN(On, Channel);
		break;
	case OffH:
		Set_PWM_CHN(Off, Channel);
		break;
	case OnL:
		if (MotorControl.PWM_Switching.UsePWMOnPWMN)
			Set_PWM_CHN1N(On, Channel);
		else
			Set_PWM_CHN1N_GPIO(On, Channel);
		break;
	case OffL:
		if (MotorControl.PWM_Switching.UsePWMOnPWMN)
			Set_PWM_CHN1N(Off, Channel);
		else
			Set_PWM_CHN1N_GPIO(Off, Channel);
		break;
	case None:
		break;
	}
}

static void Set_PWM_CHN(PWMStates state, uint32_t Channel) {
	if (state == On)
		HAL_TIM_PWM_Start_IT(&PWM_INSTANCE, Channel);
	else
		HAL_TIM_PWM_Stop_IT(&PWM_INSTANCE, Channel);
}

static void Set_PWM_CHN1N(PWMStates state, uint32_t Channel) {
	if (state == On)
		HAL_TIMEx_PWMN_Start(&PWM_INSTANCE, Channel);
	else
		HAL_TIMEx_PWMN_Stop(&PWM_INSTANCE, Channel);
}

static void Set_PWM_CHN1N_GPIO(PWMStates state, uint32_t Channel) {
	switch (Channel) {
	case TIM_CHANNEL_1:
		HAL_GPIO_WritePin(PWMN_CH1_GPIO_Port, PWMN_CH1_Pin,
				state == On ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case TIM_CHANNEL_2:
		HAL_GPIO_WritePin(PWMN_CH2_GPIO_Port, PWMN_CH2_Pin,
				state == On ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	case TIM_CHANNEL_3:
		HAL_GPIO_WritePin(PWMN_CH3_GPIO_Port, PWMN_CH3_Pin,
				state == On ? GPIO_PIN_SET : GPIO_PIN_RESET);
		break;
	}
}

void TurnAllPWMsOFF(void) {
	MotorControl.PWM_Switching.ForceTurnOffActive = 1;
	Set_PWM_CHN(Off, TIM_CHANNEL_4);

	Enable_State(OffH, TIM_CHANNEL_1);
	Enable_State(OffH, TIM_CHANNEL_2);
	Enable_State(OffH, TIM_CHANNEL_3);

	Enable_State(OffL, TIM_CHANNEL_1);
	Enable_State(OffL, TIM_CHANNEL_2);
	Enable_State(OffL, TIM_CHANNEL_3);
	MotorControl.PWM_Switching.ForceTurnOffActive = 0;
}

void ChangePWMSwitchingSequence(PWMSequences newSequences) {
	//if (MotorControl.PWM_Switching.ActiveSequence == newSequences)
	//	return;

	TurnAllPWMsOFF();

	MotorControl.PWM_Switching.IsChangingSequence = 1;

	ChangePWMDutyCycle(20, 100);

	MotorControl.PWM_Switching.ActiveSequence = newSequences;
	switch (newSequences) {
	case ForwardCommutation:
		//	__HAL_TIM_SET_PRESCALER(&PWM_INSTANCE, 6);
		MotorControl.PWM_Switching.UsePWMOnPWMN = 0;
		ChangePWMPinType();
		ActiveSequence = &ForwardCommutationSequence;
		//MotorControl.Flags.ClosedLoop = 0;
		break;
	case Regeneration:
		//	__HAL_TIM_SET_PRESCALER(&PWM_INSTANCE, 150);
		MotorControl.PWM_Switching.UsePWMOnPWMN = 1;
		ChangePWMPinType();

		Enable_State(OnL, TIM_CHANNEL_1);
		Enable_State(OnL, TIM_CHANNEL_2);
		Enable_State(OnL, TIM_CHANNEL_3);
		ActiveSequence = NULL;
		break;
	case FreeWheeling:
		MotorControl.PWM_Switching.UsePWMOnPWMN = 1;
		ChangePWMPinType();
		ActiveSequence = &FreeWheelingSequence;
		break;
	case PWMSequencesNotInit:
		break;
	}
	MotorControl.PWM_Switching.IsChangingSequence = 0;

	Set_PWM_CHN(On, TIM_CHANNEL_4);
	ChangePhase();
}

void ChangePhase(void) {

	if (MotorControl.PWM_Switching.IsChangingSequence || MotorControl.PWM_Switching.ForceTurnOffActive)
		return;
	if (MotorControl.PWM_Switching.ActiveSequence == Regeneration)
		goto ENDOF_ChangePhase;
	uint8_t pwm_phase = MotorControl.pwm_phase;

	if ((*ActiveSequence)[pwm_phase][0] == OffH || (*ActiveSequence)[pwm_phase][0] == OffL)
		Enable_State((*ActiveSequence)[pwm_phase][0], TIM_CHANNEL_1);
	else if ((*ActiveSequence)[pwm_phase][1] == OffH || (*ActiveSequence)[pwm_phase][1] == OffL)
		Enable_State((*ActiveSequence)[pwm_phase][1], TIM_CHANNEL_2);
	else if ((*ActiveSequence)[pwm_phase][2] == OffH || (*ActiveSequence)[pwm_phase][2] == OffL)
		Enable_State((*ActiveSequence)[pwm_phase][2], TIM_CHANNEL_3);

	for(int i = 500; i>0;i--)
		__NOP();


	if ((*ActiveSequence)[pwm_phase][0] == OnH || (*ActiveSequence)[pwm_phase][0] == OnL)
		Enable_State((*ActiveSequence)[pwm_phase][0], TIM_CHANNEL_1);
	else if ((*ActiveSequence)[pwm_phase][1] == OnH || (*ActiveSequence)[pwm_phase][1] == OnL)
		Enable_State((*ActiveSequence)[pwm_phase][1], TIM_CHANNEL_2);
	else if ((*ActiveSequence)[pwm_phase][2] == OnH || (*ActiveSequence)[pwm_phase][2] == OnL)
		Enable_State((*ActiveSequence)[pwm_phase][2], TIM_CHANNEL_3);

	MotorControl.pwm_phase = (MotorControl.pwm_phase + 1) % 6;
	ENDOF_ChangePhase: OnPhaseChanged();
}

