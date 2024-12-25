#include "stm32f10x.h"
#include "stdio.h"
#include "initADXL.h"
#include "initSPI2.h"
#include "gpio.h"

#define ADXL345_POWER_CTL 0x2D
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_BW_RATE 0x2C
#define ADXL345_FIFO_CTL 0x38


void config_regADXL(unsigned char reg, unsigned char data) { 
		//activer   le CS de l adxl
		GPIOB->ODR &= ~(1<<12);
	 
    //envoyer l adresse en positionnant bien le bit demande d ecriture pour l adxl
		spi2_send_byte_sans_CS(reg);
    //envoyer la data 
		spi2_send_byte_sans_CS(data);
    //desactiver le CS de l adxl
		GPIOB->ODR |= (1<<12);
 }

void init_adxl_345(void){

    //initialiser power : sequence en plusieurs �tapes conseill�e
    config_regADXL(ADXL345_POWER_CTL, 0x00);// Wakeup  
    config_regADXL(ADXL345_POWER_CTL, 0x08);// Measure
 //*******************************************DATA FORMAT ******************************************************************   
    // FORMAT DES DONNES  :  passer en 16G justifi� droit 
    // B7 : self test : garder � 0
    // B6 : mode SPI 3 ou SPI 4 fils : mettre 0 pour selectionner le mode 4 FILS
    // B5 : niveau logique des ITs   : mettre 1 = /int         mettre 0   =  int (actif haut) : mettre 1
    // B4 : laisser � 0
    // B3 : Full_ RES : mettre 1 pour disposer de plus de bits possibles
    // B2 : JUSTIFICATION DES DATAs  :  1 left justify, 0 right justify avec extension de signe : mettre 0 
    // B1 B0 : sensibilit� : mettre 00 pour 2G , 01 pour 4G, 10 pour 8G, 11 pour 16G :   11
    // choix  choisir /IT et Full resolution et 16G   
    config_regADXL(ADXL345_DATA_FORMAT,0x00);
 
 //***************************************inactivit�/ choc niveau 1****************************************************************
    //D7 : activit� DC ou AC   (absolu ou relatif aux pr�c�dentes mesures) :mettre 1 pour choisir AC
    //D6 : activit� sur X
    //D5 : activit� sur Y 
    //D4 : activit� sur Z  
    //D3 : inactivit� DC ou AC   (absolu ou relatif aux pr�c�dentes mesures) :mettre 1 pour choisir AC
    //D2 : inactivit� sur X
    //D1 : inactivit� sur Y 
    //D0 : inactivit� sur Z  
    // on va dire qu'il y a activit� si on d�tecte une activit� sur X Y ou Z
    // on va dire qu'il y a inactivit� si on ne d�tecte rien ni sur X ni sur Y ni sur Z en mode AC
  //  config_regADXL(ADXL345_ACT_INACT_CTL, 0xFF);
    
    // la valeur est par pas de 62.5mG  ainsi  256 correspondrait � 16G : mettre 
  //  config_regADXL(ADXL345_THRESH_ACT,16 );
  //  config_regADXL(ADXL345_THRESH_INACT,8 );
  //  config_regADXL(ADXL345_TIME_INACT, 1 );//sec 1-255
    
 //*******************************************choc**********************************************************   
    //D7 � D4 non definis : 0
    //D3 : suppress double tap si acceleration reste elev�e entre les TAP avec valeur 1
    //D2 : tap X enable si 1
    //D1 : tap Y enable si 1
    //D0 : tap Z enable si 1
    
  // config_regADXL(ADXL345_TAP_AXES, 0x0F); // detection choc de tous les cot�s
  // config_regADXL(ADXL345_THRESH_TAP, 0xA0); // detection choc r�gl�e � 10G
  // config_regADXL(ADXL345_DUR, 16); // duree minimale du choc 625us increment ici 10ms
 //  config_regADXL(ADXL345_LATENT, 0x00); // ecart minimum entre tap pas 1.25ms : 0 desactive
 //  config_regADXL(ADXL345_TAP_AXES, 0x00); // fenetre seconde frappe pas 1.25ms : 0 desactive
 
 //*****************************************************************************************************************
   
  // config_regADXL(ADXL345_THRESH_FF, ); // seuil detection FREE FALL  pas 62.5mG 0.6G
  // config_regADXL(ADXL345_TIME_FF,); // duree minimale de chute pas 5ms  : 100 ms =20
  
 //***************************************************************************************************************
 //interruptions  activer les ITs : ADXL345_INT_ENABLE  bit � 0 = INT1 , bit � 1 = INT2
 // selectionner patte INT1 ou INT2: ADXL345_INT_MAP
 // en cas de selection multiple, la lecture de ADXL345_INT_SOURCE
   // pour les deux registres, meme emplacement :
   // B7 = DATA_READY
   // B6 = SINGLE_TAP
   // B5 = DOUBLE_TAP
   // B4 = ACTIVITY
   // B3 = INACTIVITY
   // B2 = FREE_FALL
   // B1 = WATER_MARK  (niveau remplissage FIFO)
   // B0 = OVERRUN     (pas lu assez frequemment)
    config_regADXL(ADXL345_INT_ENABLE, 0x00);
    config_regADXL(ADXL345_INT_MAP, 0x7F); // tout le monde en INT 2 sauf DATA_READY
    config_regADXL(ADXL345_INT_ENABLE, 0x80); // Juste data rdy
	
  //***********************************************************************************************************
  //gestion par FIFO pour stocker sans danger  
   //ADXL345_BW_RATE : les 4 bits de poids faible choisissent le d�bit (voir doc))
    //F : 3200Hz, E:1600, D:800, C:400, B:200, A:100, 9:50, 8:25, 7:12.5, 6:6.25, 5:3.125, 
   // ADXL345_FIFO_CTL : 
    // bits B7 B6   : 
    // 00 bypass (no fifo)
    // 01 FIFO (blocage plein) 
    // 10 STREAM (ecrasement) 
    // 11 Trigger (photo evenement)
    // bit B5  Trigger sur INT1 (0) ou INT2 (1)
    // bit B4 � B0 : niveau remplissage pour watermark
    
    config_regADXL(ADXL345_BW_RATE, 0x0A);  // Fonctionnement � 100 HZ (page 6 et 16)
    config_regADXL(ADXL345_FIFO_CTL, 0x90); // stream, trig int1, avertissement sur mi remplissage (16)

}

// Fonction pour lire l'ID du capteur
int ADXL345_GetDeviceID(void) {
    int deviceID;

    // Activer CS (baisser SS)
		SPI2_SetCS(0);
	
    // Envoyer l'adresse avec le bit de lecture activ�
    spi2_send_byte_sans_CS(0x80);

    // Envoyer un octet vide et recevoir l'ID du capteur
    deviceID = spi2_send_receive_byte_sans_CS(0x00);

    // D�sactiver CS (remonter SS)
    SPI2_SetCS(1);

    return deviceID;
}

//Fonction pour lire les donnees d'acceleration
void read_ADXL_sensors(char *tableau) {
	unsigned char boucle;
  //declencher une lecture multiple � l adresse du registre ADXL345_DATAX0 (attention aux bits B7 et B6)
	//https://eseo-tech.github.io/CoCiNum/soc/soc-projet-pmod-acl.html
	//Les 6 bits de poids faible indiquent l�adresse A du registre de l�acc�l�rom�tre que vous souhaitez lire ou modifier.
	//Le bit 7 (RW pour Read/Write) indique si l�op�ration est une lecture (RW = 1) ou une �criture (RW = � 0 �).
	//Le bit 6 (MB pour MultiByte) indique si la lecture ou l��criture doit se faire en s�quence sur plusieurs registres contigus :
	//Si MB = 0, le second octet de la s�quence transportera la valeur du registre s�lectionn�.
	//Si MB = 1, si A est l�adresse sp�cifi�e dans le premier octet, les N octets suivants transporteront les valeurs des registres aux adresses successives A, A+1, A+2, � A+N-1
	//On demarre a l'adresse 0x32 correspondant a DATAX0
	int adresse = (1 << 7) | (1 << 6) | 0x32;
	//activer le CS de l adxl
	SPI2_SetCS(0);
	// Envoyer l'adresse 
  SPI2->DR = adresse << 8;
  // Lire les 6 octets des donn�es d'acc�l�ration
  for (boucle = 0; boucle < 3; boucle++) {
		//Envoyer un "dummy byte" 
    int reponse = spi2_send_receive_byte_sans_CS(0x00);
		if (boucle == 0) {
			tableau[0] = reponse;
		} else {
			tableau[boucle*2 - 1] = reponse >> 8;
			tableau[boucle*2] = reponse& 0xFF;
		}
  }
	while (!(SPI2->SR & (1 << 0)));
	tableau[boucle*2 - 1] = SPI2->DR >> 8;
	while(!(SPI2->SR & (1 << 1)));
	//Attendre que le SPI ait termine sa communication
	while ((SPI2->SR & (1 << 7))){}; 
	// D�sactiver le CS de l'ADXL
  SPI2_SetCS(1);
}