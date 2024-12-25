#include "stm32f10x.h"
#include "stdio.h"
#include "gpio.h"
#include "main.h"
#include "usart2.h"
#include "initADXL.h"
#include "initSPI2.h"
#include "fifo.h"

//Parametres de notre paquet de donnees personnalises
#define START_BYTE 0x7E
#define END_BYTE 0x7F
#define ID_ETUDIANT 0x01
#define CODE_SESSION 0x93
#define ID_CAPTEUR 0xE5

int main(void) {
		init_SPI2();
		init_adxl_345();
		init_USART2();
		init_gpioA(5, 0b0011);  
		GPIOA->ODR &= ~(1<<5);
		char tableau[6]; //Tableau pour stocker les 6 octets de donnees de X Y Z
		// Données de test
    uint8_t test_byte = 0xAA;    // Octet à envoyer
    uint8_t received_byte;        // Octet reçu
    while (1) {
			/*
			//test SPI2
			// 1. Début de la communication
      SPI2_SetCS(0);
			// 2. Envoi d'un seul octet et réception
      received_byte = spi2_send_receive_byte_sans_CS('A');
			// 3. Fin de la communication
      SPI2_SetCS(1);
      // Délai simple
      for (volatile int i = 0; i < 100000; i++);
      // Pour le test, on incrémente l'octet à envoyer
      test_byte++;  // L'octet changera à chaque envoi
			*/
			/*
			//Test de l'ID du capteur
			char devid = ADXL345_GetDeviceID(); // Lire l'ID du capteur
      if (devid == 0xE5) {
				GPIOA->ODR |= (1<<5);
				
			}else{
				GPIOA->ODR &= ~(1<<5);
			}*/
			
			//Lecture des donnees de l'ADXL345
			read_ADXL_sensors(tableau);
			//Combiner les LSB et MSB pour chaque axe (on aura une donnee sur 16 bits non signees)
			int axeX = (tableau[1]<<8) | tableau[0];
			int axeY = (tableau[3]<<8) | tableau[2];
			int axeZ = (tableau[5]<<8) | tableau[4];
			//En mode ±2g, les donnes doivent etre situe dans une plage de valeur variant entre 
			//-32768 à 32767 donc la valeur de chaque axe doit etre signe sur 16 bits
			if(axeX > 32767) {
				axeX -= 65536;
			}
      if(axeY > 32767) {
				axeY -= 65536;
			}
      if(axeZ > 32767) {
				axeZ -= 65536;
			}
			
      // Afficher les valeurs décodées
			char buffer[50];
			for(int i = 0; i < 50; i++) {
				buffer[i] = 0;
			}
			//On travaille avec une sensibilite de +/- 2G donc la plage totale est 4G
			//L accelerometre retourne des valeurs numeriques codees sur 10 bits 
			//(entre -512 et 511 pour chaque axe) dans le mode par défaut
			//donc la resolution est 4G/2^(10) = 0.0039
			sprintf(buffer, "x:%.3f y:%.3f z:%.3f \r\n", axeX*0.0039, axeY*0.0039, axeZ*0.0039);
			UART_SendString(buffer);
		  char checksum = calculate_checksum(tableau);
			char paquet[12] = {START_BYTE, ID_ETUDIANT, CODE_SESSION, tableau[0], tableau[1],
			tableau[2], tableau[3], tableau[4], tableau[5], checksum, END_BYTE};
			// Ajouter le paquet à la FIFO
			for (char i = 0; i < 12; i++) {
        write_fifo(paquet[i]);
			}
			send_from_fifo();
      DELAY(50); // Attendre un peu entre chaque affichage
    }
}

// Temporisation simple
void DELAY(volatile unsigned int delay) {
    volatile unsigned int i;
    for(i = 0; i < (delay * 5400); i++);
}

// Fonction pour calculer le checksum
char calculate_checksum(char *tableau) {
    uint16_t somme = ID_ETUDIANT + CODE_SESSION;
    for (int i = 0; i < 6; i++) {  // Ajouter les données X, Y, Z (6 octets)
        somme += tableau[i];
    }
    return (char)(somme % 256);
}


