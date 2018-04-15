#include "../SDCard/SDCard_SPI.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#include "../SDCard/SDCard.h"

extern SPI_HandleTypeDef hspi2;
extern TaskHandle_t SDCard_Task_Handle;
TaskHandle_t xTaskToNotify_FromSPI = NULL;


void Before_SPI_Action(void);
void After_SPI_Action(void);
void SDCardNotify_Responce_FromISR();



void SDCard_SPI_TxCpltCallback(void)
{
	SDCardNotify_Responce_FromISR();
}
void SDCard_SPI_RxCpltCallback(void)
{
	SDCardNotify_Responce_FromISR();
}
void SDCard_SPI_TxRxCpltCallback(void)
{
	SDCardNotify_Responce_FromISR();
}
void SDCard_SPI_ErrorCallback(void)
{

}


void SD_SPI_Send(const unsigned char *data, unsigned int size)
{
	Before_SPI_Action();
	if(HAL_SPI_Transmit_IT(&hspi2, (uint8_t*)data, size) != HAL_OK)
	    Error_Handler();
	After_SPI_Action();
}
void SD_SPI_Receive(unsigned char *rxData, unsigned int size)
{
	Before_SPI_Action();
	unsigned char txDataDummy[512] = {0xff};
	if(HAL_SPI_TransmitReceive_IT(&hspi2, txDataDummy, rxData, size) != HAL_OK)
	    Error_Handler();
	After_SPI_Action();
}
void SD_SPI_TransmitReceive(unsigned char *txData, unsigned char *rxData, unsigned int size)
{
	Before_SPI_Action();
	if(HAL_SPI_TransmitReceive_IT(&hspi2, txData, rxData, size) != HAL_OK)
	    Error_Handler();
	After_SPI_Action();
}


void Before_SPI_Action(void)
{
	configASSERT( xTaskToNotify_FromSPI == NULL );
	configASSERT( SDCard_Task_Handle != NULL );
	xTaskToNotify_FromSPI = SDCard_Task_Handle;//xTaskGetCurrentTaskHandle();
}
void After_SPI_Action(void)
{
	ulTaskNotifyTake( pdTRUE, osWaitForever);// pdMS_TO_TICKS( 200 ) );
}
void SDCardNotify_Responce_FromISR()
{
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;// pdFALSE;
	configASSERT( xTaskToNotify_FromSPI != NULL );
	vTaskNotifyGiveFromISR( xTaskToNotify_FromSPI, &xHigherPriorityTaskWoken );
	xTaskToNotify_FromSPI = NULL;
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}





