#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"
#include "InstDefs.h"

#include "stm32f4xx_hal_uart.h"
#include "MC.h"
UART_Driver BT_UART_Driver;
extern UART_HandleTypeDef BT_UART;
extern TaskHandle_t BT_Task_Handle;

static const TickType_t xMaxBlockTime = osWaitForever; // pdMS_TO_TICKS( 200 );
static uint8_t RxBuffer[3];

static void BT_UART_TxCpltCallback();
static void BT_UART_RxCpltCallback();
static uint32_t Send(uint8_t* data, uint16_t Size);
static void SetParameter();
void Receive_TEST();

extern QueueHandle_t xQueueRX;

void Init_BT_UART_Driver(void) {
	BT_UART_Driver.RxCpltCallback = BT_UART_RxCpltCallback;
	BT_UART_Driver.TxCpltCallback = BT_UART_TxCpltCallback;
	BT_UART_Driver.Send = Send;

	__HAL_UART_FLUSH_DRREGISTER(&BT_UART);
	__HAL_UART_GET_FLAG(&BT_UART, UART_FLAG_RXNE);
	Receive_TEST();

}
static void BT_UART_TxCpltCallback() {
	configASSERT(BT_UART_Driver.xTaskToNotify != NULL);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(*BT_UART_Driver.xTaskToNotify, &xHigherPriorityTaskWoken);
	BT_UART_Driver.xTaskToNotify = NULL;
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

uint8_t UART_suffixCnt = 0;
uint8_t UART_uuid = 0;
char UART_data[4] = { 0 };
static void BT_UART_RxCpltCallback() {
	switch (UART_suffixCnt) {
	case 0:
		UART_suffixCnt = *RxBuffer == 'B' ? 1 : 0;
		break;
	case 1:
		UART_suffixCnt = *RxBuffer == 'e' ? 2 : 0;
		break;
	case 2:
		UART_suffixCnt = *RxBuffer == 'g' ? 3 : 0;
		break;
	case 3:
		UART_suffixCnt = *RxBuffer == 'i' ? 4 : 0;
		break;
	case 4:
		UART_suffixCnt = *RxBuffer == 'n' ? 5 : 0;
		break;
	case 5:
		UART_suffixCnt++;
		UART_uuid = *RxBuffer;
		break;
	case 6:
		UART_suffixCnt++;
		UART_data[0] = *RxBuffer;
		SetParameter();
		break;
	case 7:
		UART_suffixCnt++;
		UART_data[1] = *RxBuffer;
		SetParameter();
		break;
	case 8:
		UART_suffixCnt++;
		UART_data[2] = *RxBuffer;
		SetParameter();
		break;
	case 9:
		UART_suffixCnt++;
		UART_data[3] = *RxBuffer;
		SetParameter();
		break;
	default:
		UART_suffixCnt = 0;
		break;
	}
	Receive_TEST();
}

static uint32_t Send(uint8_t* data, uint16_t Size) {
	configASSERT(BT_UART_Driver.xTaskToNotify == NULL);
	BT_UART_Driver.xTaskToNotify = &BT_Task_Handle; //xTaskGetCurrentTaskHandle();
	if (HAL_UART_Transmit_IT(&BT_UART, data, Size) != HAL_OK)
		Error_Handler();
	return ulTaskNotifyTake( pdTRUE, xMaxBlockTime);
}

void Receive_TEST() {
	if (HAL_UART_Receive_IT(&BT_UART, (uint8_t*) &RxBuffer, 1) != HAL_OK)
		Error_Handler();
}

extern void CruiseControl_Reset();
static void SetParameter() {

	uint8_t isParameterSet = 0;
	switch (UART_uuid) {
	case PWM_DUTY_CYCLE_ID:
		if (UART_suffixCnt == 6 + PWM_DUTY_CYCLE_LEN) {
			MotorControl.Wanted_DutyCycle = *(uint8_t*)&UART_data;
			CruiseControl_Reset();
			//ChangePWMDutyCycle(MotorControl.Wanted_DutyCycle, 100);
			isParameterSet = 1;
		}
		break;
	case V_THRESHOLD_ID:
		if (UART_suffixCnt == 6 + V_THRESHOLD_LEN) {
			MotorControl.V_Treshold = *(uint32_t*)&UART_data;
			if (MotorControl.V_Treshold < 5)
				MotorControl.V_Treshold = 5;
			if (MotorControl.V_Treshold > 100)
				MotorControl.V_Treshold = 100;
			isParameterSet = 1;
		}

		break;
	case MODE_ID:
		if (UART_suffixCnt == 6 + MODE_LEN) {
			if (UART_data[0])
				ChangePWMSwitchingSequence(ForwardCommutation);
			else
				ChangePWMSwitchingSequence(Regeneration);
			isParameterSet = 1;
		}
		break;
	default:
		break;
	}

	if (isParameterSet) {

		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		UART_suffixCnt = 0;
		UART_uuid = 0;
		memset(UART_data, 0, 4);
	}
}
