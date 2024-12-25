#ifndef INITSPI2_H
#define INITSPI2_H

void init_SPI2(void);
void SPI2_SetCS(int state);
void spi2_send_byte_sans_CS(int data);
int spi2_send_receive_byte_sans_CS(int data);


#endif