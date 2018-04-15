#ifndef MOTOR_CONTROL_PWM_SWITCHING_H_
#define MOTOR_CONTROL_PWM_SWITCHING_H_


#include "MC.h"
extern TIM_HandleTypeDef PWM_INSTANCE;

typedef enum
{
	None = 0,
	OnH = 1,
	OffH = 2,
	OnL = 3,
	OffL = 4,
}PWMActions;

typedef enum
{
	Off = 0,
	On = 1,
}PWMStates;


#endif /* MOTOR_CONTROL_PWM_SWITCHING_H_ */
