#ifndef USART2_H
#define USART2_H

void init_USART2(void);
void UART_Write_Char_User(char c);
char UART_Read_Char_User(void);
void UART_SendString(char* str);

#endif