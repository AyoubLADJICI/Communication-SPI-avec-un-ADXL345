import processing.serial.*;

// Déclarations des variables globales
Serial myPort;
int[] accelData = new int[3];  // Pour stocker les valeurs X, Y, Z
int checksum = 0;             // Checksum calculé pendant la réception
int receivedChecksum = 0;     // Checksum reçu
boolean packetStarted = false; // Indique si un paquet est en cours de réception
int index = 0;                // Indice pour les données reçues

void setup() {
  size(1280, 1000);  // Taille de la fenêtre graphique
  
  // Lister les ports série disponibles
  println("Ports disponibles :");
  printArray(Serial.list());
  
  // Sélectionner le port série correct (adapter l'indice selon votre machine)
  String portName = Serial.list()[0];  // Remplacez 0 par le bon index si nécessaire
  myPort = new Serial(this, portName, 9600);
  println("Port sélectionné : " + portName);
  
  // Configurer la fenêtre graphique
  background(255);
  fill(0);
}

void draw() {
  // Vérifier s'il y a des données disponibles sur le port série
  while (myPort.available() > 0) {
    int inByte = myPort.read();  // Lire l'octet reçu
    //println("Octet reçu : " + hex(inByte, 2));  // Afficher l'octet reçu en hexadécimal
    if (inByte == 0x7E) {  // Début du paquet (Start Byte)
      packetStarted = true;
      index = 0;          // Réinitialiser l'indice
      checksum = 0;       // Réinitialiser le checksum
      accelData = new int[3];
    } else if (inByte == 0x7F && packetStarted) {  // Fin du paquet (End Byte)
      // Comparer le checksum calculé et reçu
      if (checksum % 256 == receivedChecksum) {
        if (accelData[0] > 32767) {
          accelData[0] -= 65536;
        }
        if (accelData[1] > 32767) {
          accelData[1] -= 65536;
        }
        if (accelData[2] > 32767) {
          accelData[2] -= 65536;
        }
        // Afficher les données si elles sont valides
        println("Données valides. X: " + accelData[0]*0.0039 + " Y: " + accelData[1]*0.0039 + " Z: " + accelData[2]*0.0039);
        // Mettre à jour l'affichage graphique
        updateGraph(accelData[0], accelData[1], accelData[2]);
      } else {
        println("Erreur de checksum. Données corrompues.");
        println("Checksum calculé : " + (checksum % 256));
        println("Checksum reçu : " + receivedChecksum);
      }
      packetStarted = false;  // Réinitialiser pour le prochain paquet
    } else if (packetStarted) {
      // Traiter les octets du paquet
      if (index == 0) {
        println("ID Étudiant : " + inByte);
      } else if (index == 1) {
        println("Code de Session : " + inByte);
      } else if (index >= 2 && index <= 7) {
        // Données d'accélération X, Y, Z (16 bits par axe, donc 6 octets au total)
        int shift = (index % 2 == 0) ? 8 : 0;
        accelData[(index - 2) / 2] |= (inByte << shift);  // Combiner les octets faibles et forts
      } else if (index == 8) {
        // Checksum reçu
        receivedChecksum = inByte;
      }
      // Calculer le checksum à chaque réception d'octet (sauf le checksum lui-même)
      if (index < 8) {
        checksum += inByte;
      }
      index++;  // Passer à l'octet suivant
    }
  }
}

// Fonction pour mettre à jour l'affichage graphique
void updateGraph(int x, int y, int z) {
  background(255);  // Effacer l'écran avec un fond blanc
  textSize(20);  // Taille du texte pour les étiquettes
  // Dessiner des barres pour représenter X, Y et Z
  fill(0, 100, 200);  // Couleur des barres
  text("X", 200, 40);  // Label pour l'axe X
  rect(200, 50, x * 0.0039,30);  // Axe X
  
  fill(200, 100, 0);
  text("Y", 200, 140);  // Label pour l'axe Y
  rect(200, 150, y * 0.0039,30);  // Axe Y
  
  fill(0, 200, 100);
  text("Z", 200, 240);  // Label pour l'axe Z
  rect(200, 250, z * 0.0039,30);  // Axe Z
}
