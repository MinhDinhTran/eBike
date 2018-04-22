#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"

extern void Receive_TEST();
extern void ProccesReceivedMessage(MyMsg_t *msg);

void Start_BT_Task(void);
static void BT_Task(void *pvParameters);

TaskHandle_t BT_Task_Handle = NULL;
QueueHandle_t xQueueTX;
QueueHandle_t xQueueRX;

void Start_BT_Task(void) {
	BT_Task_Handle = (TaskHandle_t) xTaskCreate(BT_Task, // The function that implements the task.
	"BT_Task", 		// Text name for the task, just to help debugging.
	configMINIMAL_STACK_SIZE, // The size (in words) of the stack that should be created for the task.
	NULL, 	// A parameter that can be passed into the task.  Not used
	configMAX_PRIORITIES, // The priority to assign to the task.  tskIDLE_PRIORITY (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1 is the highest priority.
	NULL);						// TaskHandle_t * const pxCreatedTask

}

static void BT_Task(void *pvParameters) {
	UNUSED(pvParameters);
	xQueueTX = xQueueCreate(100, sizeof(MyMsg_t*));
	if (xQueueTX == NULL)
		Error_Handler();
	xQueueRX = xQueueCreate(100, sizeof(MyMsg_t*));
	if (xQueueRX == NULL)
		Error_Handler();

	Init_BT_UART_Driver();
	MyMsg_t *msgToSend;
	for (;;) {

		if (xQueueReceive(xQueueTX, &(msgToSend), (TickType_t ) 5)) {
			BT_UART_Driver.Send((uint8_t*) msgToSend->pData, msgToSend->length +1 );
			free(msgToSend->pData);
			free(msgToSend);
		}
		osDelay(1);
		MyMsg_t *msg = MyMsg_ProcessCache();
		if (msg != NULL) {
			ProccesReceivedMessage(msg);
			free(msg->pData);
			free(msg);
		}
	}
}


