#include <Arduino.h>                  // Inclusion de la bibliothèque principale Arduino pour la gestion des E/S.
#include "rgb_lcd.h"                   // Inclusion de la bibliothèque pour le contrôle de l'écran LCD RGB.
#include <SPI.h>                        // Inclusion de la bibliothèque pour la communication SPI (Serial Peripheral Interface).
#include "Adafruit_TCS34725.h"          // Inclusion de la bibliothèque pour le capteur de couleur TCS34725.

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);  // Initialisation du capteur TCS34725 avec des valeurs spécifiques de temps d'intégration et de gain.

rgb_lcd lcd;                          // Création d'un objet pour l'écran LCD RGB.

int BP0 = 0;                           // Définition de la broche pour le bouton 0 (sens horaire).
int BP1 = 2;                           // Définition de la broche pour le bouton 1 (non utilisé ici).
int BP2 = 12;                          // Définition de la broche pour le bouton 2 (sens antihoraire).

int Valeur1;                           // Variable pour stocker l'état du bouton BP0.
int Valeur2;                           // Variable pour stocker l'état du bouton BP1 (non utilisé ici).
int Valeur3;                           // Variable pour stocker l'état du bouton BP2.

int antihorraires = 26;                // Broche pour contrôler le sens antihoraire du moteur.
int pwmA = 27;                         // Broche pour générer le signal PWM pour la vitesse du moteur.

int frequence = 35000;                 // Définition de la fréquence du signal PWM (25 kHz).
int canal0 = 0;                        // Canal PWM utilisé pour le signal de contrôle moteur.
int resolution = 11;                   // Résolution du signal PWM (2^11 = 2048 niveaux de précision).

void setup()
{
  Serial.begin(115200);                // Initialisation de la communication série à 115200 bps pour le débogage.

  pinMode(BP0, INPUT_PULLUP);          // Configurer la broche BP0 comme entrée avec pull-up interne.
  pinMode(BP1, INPUT_PULLUP);          // Configurer la broche BP1 comme entrée avec pull-up interne.
  pinMode(BP2, INPUT_PULLUP);          // Configurer la broche BP2 comme entrée avec pull-up interne.

  pinMode(antihorraires, OUTPUT);      // Configurer la broche pour le sens antihoraire comme sortie.

  Wire1.setPins(15, 5);                // Définir les broches SDA (15) et SCL (5) pour la communication I2C.
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1); // Initialisation de l'écran LCD avec 16 colonnes et 2 lignes.
  lcd.setColor(BLUE);                  // Définir la couleur de fond de l'écran à bleu.

  ledcSetup(canal0, frequence, resolution); // Configuration du canal PWM avec la fréquence et la résolution.
  ledcAttachPin(pwmA, canal0);             // Attacher la broche PWM au canal pour contrôler la vitesse du moteur.

  ledcWrite(canal0, 0);                  // Arrêter le moteur au démarrage (rapport cyclique à 0%).

  if (tcs.begin())                      // Initialiser le capteur TCS34725.
  {
    Serial.println("Found sensor");     // Afficher un message si le capteur est détecté.
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections"); // Afficher un message d'erreur si le capteur n'est pas trouvé.
    while (1);                           // Bloquer le programme si le capteur est absent.
  }
}

void loop()
{
  Valeur1 = digitalRead(BP0);           // Lire l'état du bouton BP0 (sens horaire).
  Valeur2 = digitalRead(BP1);           // Lire l'état du bouton BP1 (non utilisé ici).
  Valeur3 = digitalRead(BP2);           // Lire l'état du bouton BP2 (sens antihoraire).

  if (Valeur1 == LOW)                   // Si BP0 est appuyé (circuit fermé).
  {
    digitalWrite(antihorraires, LOW);   // Désactiver le sens antihoraire.
    ledcWrite(canal0, 600);             // Réglage du moteur à une vitesse de 50% (rapport cyclique à 600).
    lcd.setCursor(0, 0);                // Définir le curseur LCD à la première ligne.
    lcd.print("Sens Horaire   ");        // Afficher "Sens Horaire" sur l'écran LCD.
  }
  else if (Valeur3 == LOW)              // Si BP2 est appuyé (circuit fermé).
  {
    digitalWrite(antihorraires, HIGH);  // Activer le sens antihoraire.
    ledcWrite(canal0, 600);             // Réglage du moteur à une vitesse de 50% (rapport cyclique à 600).
    lcd.setCursor(0, 0);                // Définir le curseur LCD à la première ligne.
    lcd.print("Sens Antihor.  ");        // Afficher "Sens Antihoraire" sur l'écran LCD.
  }
  else                                  // Si aucun bouton n'est appuyé.
  {
    ledcWrite(canal0, 0);               // Arrêter le moteur.
    lcd.setCursor(0, 0);                // Définir le curseur LCD à la première ligne.
    lcd.print("Moteur Arret   ");        // Afficher "Moteur Arrêt" sur l'écran LCD.
  }

  uint16_t r, g, b, c;                  // Variables pour stocker les valeurs des couleurs (rouge, vert, bleu et lumière).
  tcs.getRawData(&r, &g, &b, &c);       // Lecture des données du capteur de couleur.

  float sum = c;                        // Normalisation des données du capteur.
  float red = r / sum * 255.0;          // Calcul de la composante rouge normalisée.
  float green = g / sum * 255.0;        // Calcul de la composante verte normalisée.
  float blue = b / sum * 255.0;         // Calcul de la composante bleue normalisée.

  lcd.setCursor(0, 1);                  // Définir le curseur LCD à la deuxième ligne.
  if (red > green && red > blue)        // Si le rouge est la couleur dominante.
  {
    lcd.print("Couleur: Rouge ");        // Afficher "Couleur: Rouge" sur l'écran LCD.
  }
  else if (green > red && green > blue) // Si le vert est la couleur dominante.
  {
    lcd.print("Couleur: Vert  ");        // Afficher "Couleur: Vert" sur l'écran LCD.
  }
  else if (blue > red && blue > green)  // Si le bleu est la couleur dominante.
  {
    lcd.print("Couleur: Bleu  ");        // Afficher "Couleur: Bleu" sur l'écran LCD.
  }
  else                                  // Si aucune couleur n'est dominante.
  {
    lcd.print("Couleur: Incon.");        // Afficher "Couleur: Inconnue" sur l'écran LCD.
  }

  red = red > 255 ? 255 : red;          // S'assurer que les valeurs des couleurs ne dépassent pas 255.
  green = green > 255 ? 255 : green;
  blue = blue > 255 ? 255 : blue;
  lcd.setRGB((int)red, (int)green, (int)blue); // Changer la couleur d'arrière-plan de l'écran LCD.

  delay(500);                           // Attendre 500 ms pour stabiliser la lecture des données du capteur.
}
