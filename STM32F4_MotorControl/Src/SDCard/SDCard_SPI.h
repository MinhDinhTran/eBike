#ifndef SDCARD_SDCARD_SPI_H_
#define SDCARD_SDCARD_SPI_H_


void SD_SPI_Send(const unsigned char *data, unsigned int size);
void SD_SPI_Receive(unsigned char *rxData, unsigned int size);
void SD_SPI_TransmitReceive(unsigned char *txData, unsigned char *rxData, unsigned int size);

#endif /* SDCARD_SDCARD_SPI_H_ */
