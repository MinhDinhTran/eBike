/**
* UART Message structure
* 1 byte(uint8)	- 1 bytes(uint8)	- x bytes	- 2 bytes  [ sum of bytes ==  msg length ]
* msg length    - msg UUID  		- message 	- \r\n
*
* !!! whole msg should not contain terminating null-characters
*/

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Bluetooth_Msg.h"


extern void Receive_TEST();
extern void ProccesReceivedMessage(Bluetooth_MSG_t *msg);


void Start_BT_Task(void);
static void BT_Task(void *pvParameters);


TaskHandle_t BT_Task_Handle = NULL;
QueueHandle_t xQueueTX;
QueueHandle_t xQueueRX;


void Start_BT_Task(void) {
	BT_Task_Handle = (TaskHandle_t) xTaskCreate(BT_Task,// The function that implements the task.
			"BT_Task", 		// Text name for the task, just to help debugging.
			configMINIMAL_STACK_SIZE, // The size (in words) of the stack that should be created for the task.
			NULL, 	// A parameter that can be passed into the task.  Not used
			configMAX_PRIORITIES ,// The priority to assign to the task.  tskIDLE_PRIORITY (which is 0) is the lowest priority.  configMAX_PRIORITIES - 1 is the highest priority.
			NULL);						// TaskHandle_t * const pxCreatedTask

}

static void BT_Task(void *pvParameters) {
	UNUSED(pvParameters);
	xQueueTX = xQueueCreate(10, sizeof(Bluetooth_MSG_t*));
	if (xQueueTX == NULL)
		Error_Handler();
	xQueueRX = xQueueCreate(5, sizeof(Bluetooth_MSG_t*));
	if (xQueueRX == NULL)
		Error_Handler();



	uint8_t str[50];
	Init_BT_UART_Driver();
	Bluetooth_MSG_t *rxedMessage;
	Bluetooth_MSG_t *txedMessage;
	for (;;) {

		if (xQueueReceive(xQueueTX, &(rxedMessage), (TickType_t ) 5)) {
			uint8_t len = UUID_LEN;
			str[MSG_LEN_SIZE] = rxedMessage->UUID;
			for (int i = 0; i < rxedMessage->length; i++)
			{
				memcpy(&str[len+MSG_LEN_SIZE], &rxedMessage->MSG[i], 4);
				len +=4;
			}

			len += sprintf((char*)&str[len+MSG_LEN_SIZE], "\r\n");

			len += MSG_LEN_SIZE;
			str[0] = len & 0xff;

			BT_UART_Driver.Send((uint8_t*) str, (int)len);

			free(rxedMessage);
		}

		if (xQueueReceive(xQueueRX, &(txedMessage), (TickType_t ) 10)) {
			ProccesReceivedMessage(txedMessage);
			free(txedMessage);
			Receive_TEST();
		}
	}
}

