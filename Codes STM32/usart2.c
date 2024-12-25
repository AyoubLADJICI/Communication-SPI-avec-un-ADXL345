#include "stm32f10x.h"
#include "usart2.h"
#include "gpio.h"

// Fonction d'initialisation de l'USART2
void init_USART2(void) {
    RCC->APB1ENR |= (1<<17); //Activation de l'horloge pour l'USART2

    /* Configuration des broches PA2 (TX) et PA3 (RX) */
    // PA2 (TX) doit Etre configuree en mode alternate function output push-pull 10 MHz
    init_gpioA(2, 0b1001); 
    // PA3 (RX) doit etre configuree en mode floating input
    init_gpioA(3, 0b0100); 
		
		USART2->CR1 = 0;
    /* Configurer le baudrate ? 9600 bps */
    // Le baudrate est calcul? avec Fpclk / Baudrate
    // Fpclk pour USART2 est sur APB1, donc 36 MHz, et le baudrate est 9600
    USART2->BRR = 3750;
    USART2->CR1 |= (1<<13); // Activation de l'USART2
		USART2->CR1 |= (1<<2); //Activer la recepetion
		USART2->CR1 |= (1<<3); //Activer la transmission

    /* Configurer 1 bit de stop */
    // Bits 13:12 dans CR2 doivent etre mis a 00 pour configurer 1 bit de stop
    USART2->CR2 &= ~(3<<12);
}

void UART_Write_Char_User(char c) {
	while (!(USART2->SR & (1 << 7))) {
		//Tant que l'etat du drapeau TXE est ? '0'
		//nous pouvons pas ecrire une donnee sur le registre DR
  }
  USART2->DR = c; //Ecrire le caract?re dans le registre USART_DR pour l'envoyer
}

char UART_Read_Char_User(void) {
	while (!(USART2->SR & (1 << 5))){
		//Tant que l'etat du drapeau RXNE est ? '0'
		//nous pouvons pas recevoir des donnees
	}
	return USART2->DR; //Retourner le caractere re?u
}

void UART_SendString(char* str) {
    while (*str) { // Tant qu'on n'a pas atteint la fin de la chaîne
        UART_Write_Char_User(*str); // Envoyer le caractère
        str++; // Passer au caractère suivant
    }
}