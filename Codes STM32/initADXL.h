#ifndef INITADXL_H
#define INITADXL_H

void init_adxl_345(void);
void config_regADXL(unsigned char reg, unsigned char data);
int ADXL345_GetDeviceID(void);
void read_ADXL_sensors(char *tableau);

#endif