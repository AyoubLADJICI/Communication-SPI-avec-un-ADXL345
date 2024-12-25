#include "stm32f10x.h"
#include "initSPI2.h"
#include "gpio.h"

// Initialisation du SPI2 (maitre)
void init_SPI2(void) {
    // Activer les horloges AFIO et SPI2
    RCC->APB2ENR |= (1 << 0);  // Horloge AFIO page 113
    RCC->APB1ENR |= (1 << 14);  // Horloge SPI2 page 115

    // Configuration des broches SPI (PB12=SS, PB13=SCLK, PB14=MISO, PB15=MOSI)
    init_gpioB(12, 0b0011);  // PB12 (SS) : Output push-pull, max speed 50 MHz
    init_gpioB(13, 0b1011);  // PB13 (SCLK) : Alternate function output push-pull
    init_gpioB(14, 0b0100);  // PB14 (MISO) : Floating input
    init_gpioB(15, 0b1011);  // PB15 (MOSI) : Alternate function output push-pull

    //Configuration du SPI2
    SPI2->CR1 = 0;  // Reinitialisation
    SPI2->CR1 |= (1 << 2);  // MSTR=1 Activation du mode maitre 
		SPI2->CR1 |= (3 << 3);  // BR[2:0] = 011, Diviseur d'horloge SPI (fpclk/8) (page 742)
		//Les donnees sont echantillonnees au milieu du cycle d'horloge sur le front montant
    SPI2->CR1 |= (1 << 1);  // CPOL=1, 
		SPI2->CR1 |= (1 << 0);  // CPHA=1 
    SPI2->CR1 |= (1 << 11);  // DFF=1 pour Communication en 16 bits (page 742)
		
		//Activer SS output enable (SSOE) dans CR2 pour controler SS automatiquement
		SPI2->CR1 |= (1 << 8); //SSI = 1
		SPI2->CR1 |= (1 << 9); //SSM = 1 (gestion logicielle du SS)
    SPI2->CR2 &= ~(1 << 2);  //SSOE = 0

    SPI2->CR1 |= (1 << 6);  // Activation du SPI2 (SPE = 1)
}

void SPI2_SetCS(int state) {
    if (state == 0) {
        GPIOB->ODR &= ~(1 << 12);  // CS = 0
    }else{
        GPIOB->ODR |= (1 << 12);   // CS = 1
    }
}

void spi2_send_byte_sans_CS(int data) {
    volatile int poubelle;

    // 1. Attendre que le registre de donnees (TXE) soit vide
    while (!(SPI2->SR & (1 << 1))); // TXE = 1 : Le tampon est prêt à recevoir des données

    // 2. Envoyer l'octet
    SPI2->DR = data;

    // 3. Attendre que la réception soit complète (RXNE = 1)
    while (!(SPI2->SR & (1 << 0))); // RXNE = 1 : Une donnée est reçue

    // 4. Lire l'octet reçu pour purger le tampon SPI IN (le mettre dans la poubelle)
    poubelle = SPI2->DR;
}

int spi2_send_receive_byte_sans_CS(int data) {
    int retour;

    // 1. Attendre que le tampon de transmission soit prêt (TXE)
		while (!(SPI2->SR & (1 << 1))){}
	
		// 2. Envoyer l'octet 
    SPI2->DR = data;

    // 3. Attendre que la réception soit complète (bit RXNE de SR) 
		// car cela indique qu on a fini l envoi aussi
		while (!(SPI2->SR & (1 << 0))); // RXNE = 1 : Une donnée est reçue
	
		// 4. lire renvoyer l'octet reçu
		retour = SPI2->DR;
	
    return retour;
}

