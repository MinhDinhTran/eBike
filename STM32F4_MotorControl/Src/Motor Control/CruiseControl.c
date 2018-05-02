#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "MC.h"

osThreadId CruiseControlTaskHandle;

#define CruiseControl_RPM_Toleration 3

static void PID();
static void CruiseControlTask(void const * argument);

uint8_t CruiseControl_Count = 0;
int CruiseControl_RPM = 0;
uint8_t CruiseControl_IsActive = 0;
void CruiseControl_Reset() { // reset when user changes duty cycle
	CruiseControl_Count = 0;
	CruiseControl_IsActive = 0;
}
void CruiseControl_Start() {
	//osThreadDef(cruiseControl, CruiseControlTask, osPriorityNormal, 0, 128);
	//CruiseControlTaskHandle = osThreadCreate(osThread(cruiseControl), NULL);
}
static void CruiseControlTask(void const * argument) {
	UNUSED(argument);
	for (;;) {
		osDelay(10);
		if (MotorControl.PWM_Switching.ActiveSequence == ForwardCommutation) {
			if (CruiseControl_IsActive) {
				PID();
			} else {
				int rpm = (int) MotorControl.RPM;
				if (rpm == 0 || rpm > CruiseControl_RPM + CruiseControl_RPM_Toleration || rpm < CruiseControl_RPM - CruiseControl_RPM_Toleration) {
					CruiseControl_Count = 0;
					CruiseControl_RPM = rpm;
				} else {
					if (CruiseControl_Count < 100) {
						CruiseControl_Count++;
						if (CruiseControl_Count == 100) {
							arm_pid_reset_f32(&MotorControl.PID);
							CruiseControl_IsActive = 1;
						}
					}
				}
			}
		}
	}
}

static void PID() {
	if (MotorControl.PWM_Switching.ActiveSequence != ForwardCommutation)
		return;
	float32_t output = arm_pid_f32(&MotorControl.PID, (float32_t) MotorControl.RPM - CruiseControl_RPM);
	int newDutyCycle = (int) output;

	if (newDutyCycle < 20)
		newDutyCycle = 20;
	if (newDutyCycle > 95)
		newDutyCycle = 95;
	MotorControl.Wanted_DutyCycle = newDutyCycle;
	//ChangePWMDutyCycle((uint8_t) newDutyCycle, 1);
}
