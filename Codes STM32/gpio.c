#include "stm32f10x.h"
#include "gpio.h"

// Initialisation des GPIO pour les broches SPI
void init_gpioA(unsigned char num_bit, unsigned int quartet_config) {
    unsigned char bit_ref = (num_bit * 4) & 31;

    // Activer l'horloge pour GPIOA : RCC->APB2ENR bit 2
    RCC->APB2ENR |= (1 << 2);

    // Limiter quartet_config a 4 bits
    quartet_config &= 0xF;

    if (num_bit < 8) {
        GPIOA->CRL &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRL
        GPIOA->CRL |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    } else {
        GPIOA->CRH &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRH
        GPIOA->CRH |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    }
}

void init_gpioB(unsigned char num_bit, unsigned int quartet_config) {
    unsigned char bit_ref = (num_bit * 4) & 31;

    // Activer l'horloge pour GPIOB : RCC->APB2ENR bit 3
    RCC->APB2ENR |= (1 << 3);

    // Limiter quartet_config a 4 bits
    quartet_config &= 0xF;

    if (num_bit < 8) {
        GPIOB->CRL &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRL
        GPIOB->CRL |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    } else {
        GPIOB->CRH &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRH
        GPIOB->CRH |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    }
}

void init_gpioC(unsigned char num_bit, unsigned int quartet_config) {
    unsigned char bit_ref = (num_bit * 4) & 31;

    // Activer l'horloge pour GPIOC : RCC->APB2ENR bit 4
    RCC->APB2ENR |= (1 << 4);

    // Limiter quartet_config a 4 bits
    quartet_config &= 0xF;

    if (num_bit < 8) {
        GPIOC->CRL &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRL
        GPIOC->CRL |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    } else {
        GPIOC->CRH &= ~(0xF << bit_ref);  // Effacer les anciens bits dans CRH
        GPIOC->CRH |= (quartet_config << bit_ref);  // Configurer les nouveaux bits
    }
}
