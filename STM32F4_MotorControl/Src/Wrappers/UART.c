#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "Bluetooth_Msg.h"
#include "InstDefs.h"


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == BLE_INSTANCE)
		BT_UART_Driver.TxCpltCallback();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == BLE_INSTANCE)
		BT_UART_Driver.RxCpltCallback();
}


