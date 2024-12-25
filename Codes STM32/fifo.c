#include "stm32f10x.h"
#include "fifo.h"
#include "usart2.h"

#define TAILLE_FIFO 255  // Taille du buffer circulaire
#define BIT_INDIQUANT_ENVOI_POSSIBLE (1 << 7)

// FIFO circulaire pour stocker les données avant l'envoi via USART
char fifo_env[TAILLE_FIFO];
char pw_fifo = 0;  // Pointeur d'écriture dans la FIFO
char pr_fifo = 0;  // Pointeur de lecture dans la FIFO
char place_libre = TAILLE_FIFO;  // Espace disponible dans la FIFO

// Fonction pour écrire dans la FIFO
void write_fifo(char data) {
    if (place_libre > 0) {
        fifo_env[pw_fifo] = data;
        pw_fifo = (pw_fifo + 1) % TAILLE_FIFO;  // Reboucler si nécessaire
        place_libre--;
    }
}

// Fonction pour envoyer depuis la FIFO via USART
void send_from_fifo(void) {
    if (place_libre < TAILLE_FIFO) {
        if (USART2->SR & BIT_INDIQUANT_ENVOI_POSSIBLE) {  // Vérifier si l'USART est prêt
            USART2->DR = fifo_env[pr_fifo];
            pr_fifo = (pr_fifo + 1) % TAILLE_FIFO;  // Reboucler si nécessaire
            place_libre++;
        }
    }
}