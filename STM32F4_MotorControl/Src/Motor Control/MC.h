#ifndef MOTOR_CONTROL_MC_H_
#define MOTOR_CONTROL_MC_H_

#include "arm_math.h"
typedef enum
{
	PWMSequencesNotInit,
	ForwardCommutation,
	FreeWheeling,
	Regeneration,
}PWMSequences;

typedef struct MotorControlFlags_t MotorControlFlags_t;
struct MotorControlFlags_t
{
	uint8_t ClosedLoop:1;
	uint8_t OverCurrent:1;
};

typedef struct MotorControlLimits_t MotorControlLimits_t;
struct MotorControlLimits_t
{
	uint64_t Integral;
};

typedef struct PWM_Switching_t PWM_Switching_t;
struct PWM_Switching_t
{
	PWMSequences ActiveSequence;

	uint8_t IsRisingFront:1;
	uint8_t IsChangingSequence:1;
	uint8_t UseComplementaryPWM:1;
	uint8_t UsePWMOnPWMN:1;
	uint8_t ForceTurnOffActive:1;

};

typedef struct MotorControl_t MotorControl_t;
struct MotorControl_t
{
    MotorControlLimits_t Limits;
    MotorControlFlags_t Flags;
    PWM_Switching_t PWM_Switching;
	uint8_t DutyCycle;
	uint8_t Wanted_DutyCycle;
	uint32_t V_Treshold;
	uint16_t ADC_V[3];
	uint16_t ADC_I[3];
    uint16_t ADC_VBAT;

    uint64_t Integral;

    int8_t pwm_phase;

    uint16_t pwmCountThisPhase;
    uint16_t pwmCountToChangePhase;// used in open loop

    float Wanted_RPM;
    float RPM;
    float Energy;

    arm_pid_instance_f32 PID;
};

extern MotorControl_t MotorControl;



void TurnAllPWMsOFF(void);
void ChangePhase(void);
void ChangePWMSwitchingSequence(PWMSequences newSequences );
void ChangePWMDutyCycle(uint8_t newDutyCycle, int8_t maxStep);
void TurnOnRegeneration();


#define POLIU_SKAICIUS 24
#endif /* MOTOR_CONTROL_MC_H_ */
