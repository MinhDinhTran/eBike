#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "stm32f4xx_hal.h"

__weak void SDCard_SPI_TxCpltCallback(void){}
__weak void SDCard_SPI_RxCpltCallback(void){}
__weak void SDCard_SPI_TxRxCpltCallback(void){}
__weak void SDCard_SPI_ErrorCallback(void){}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI2)
		SDCard_SPI_TxCpltCallback();
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI2)
		SDCard_SPI_RxCpltCallback();
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI2)
		SDCard_SPI_ErrorCallback();
}
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI2)
		SDCard_SPI_TxRxCpltCallback();
}




