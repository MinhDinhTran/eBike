#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"
#include "InstDefs.h"

 #include "stm32f4xx_hal_uart.h"
UART_Driver BT_UART_Driver;
extern UART_HandleTypeDef BT_UART;
extern TaskHandle_t BT_Task_Handle;

static const TickType_t xMaxBlockTime = osWaitForever;// pdMS_TO_TICKS( 200 );
static uint8_t RxBuffer[3];


static void BT_UART_TxCpltCallback();
static void BT_UART_RxCpltCallback();
static uint32_t Send(uint8_t* data, uint16_t Size);
void Receive_TEST();

extern QueueHandle_t xQueueRX;

void Init_BT_UART_Driver(void)
{
	BT_UART_Driver.RxCpltCallback = BT_UART_RxCpltCallback;
	BT_UART_Driver.TxCpltCallback = BT_UART_TxCpltCallback;
	BT_UART_Driver.Send = Send;

	__HAL_UART_FLUSH_DRREGISTER(&BT_UART);
	__HAL_UART_GET_FLAG(&BT_UART, UART_FLAG_RXNE);
	Receive_TEST();

}
static void BT_UART_TxCpltCallback()
{
	configASSERT( BT_UART_Driver.xTaskToNotify != NULL );

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR( *BT_UART_Driver.xTaskToNotify, &xHigherPriorityTaskWoken );
	BT_UART_Driver.xTaskToNotify = NULL;
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void BT_UART_RxCpltCallback()
{
	MyMsg_CacheStringPiece_ISR(RxBuffer[0]);
	Receive_TEST();
}

static uint32_t Send(uint8_t* data, uint16_t Size)
{
	configASSERT( BT_UART_Driver.xTaskToNotify == NULL );
	BT_UART_Driver.xTaskToNotify = &BT_Task_Handle;//xTaskGetCurrentTaskHandle();
	if (HAL_UART_Transmit_IT(&BT_UART, data, Size) != HAL_OK)
		  Error_Handler();
	return ulTaskNotifyTake( pdTRUE, xMaxBlockTime );
}

void Receive_TEST()
{
	if (HAL_UART_Receive_IT(&BT_UART, (uint8_t*)&RxBuffer, 1) != HAL_OK)
		Error_Handler();
}








