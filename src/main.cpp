#include <Arduino.h>
#include "rgb_lcd.h"

// Initialisation de l'écran LCD RGB
rgb_lcd lcd;

// Déclaration des broches des boutons
int BP0 = 0;  // Bouton pour sens horaire
int BP1 = 2;  // (Non utilisé dans cette version, peut servir pour arrêter ou une autre fonction)
int BP2 = 12; // Bouton pour sens antihoraire

// Variables pour stocker l'état des boutons
int Valeur1;
int Valeur2;
int Valeur3;

// Broches pour le contrôle du moteur
int antihorraires = 26; // Sens antihoraire
int pwmA = 27;          // Signal PWM (vitesse du moteur)

// Caractéristiques de la PWM
int frequence = 25000; // Fréquence du signal PWM (25 kHz)
int canal0 = 0;        // Canal PWM utilisé
int resolution = 11;   // Résolution en bits du signal PWM (2^11 = 2048 niveaux)

void setup() {
  // Initialise la communication avec le terminal série
  Serial.begin(115200);

  // Configure les broches des boutons comme entrées avec pull-up interne
  pinMode(BP0, INPUT_PULLUP);
  pinMode(BP1, INPUT_PULLUP);
  pinMode(BP2, INPUT_PULLUP);

  // Configure les broches pour le contrôle du moteur
  pinMode(antihorraires, OUTPUT);

  // Configure les broches pour l'écran LCD
  Wire1.setPins(15, 5);                     // Définir les broches SDA et SCL
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);    // Initialisation de l'écran LCD
  lcd.setColor(BLUE);                      // Définit la couleur de l'écran à bleu

  // Initialise le canal PWM pour le moteur
  ledcSetup(canal0, frequence, resolution); // Configure le canal avec fréquence et résolution
  ledcAttachPin(pwmA, canal0);             // Associe la broche pwmA au canal PWM

  // Définit un rapport cyclique de 0% (moteur à l'arrêt au démarrage)
  ledcWrite(canal0, 0);
}

void loop() {
  // Lecture de l'état des boutons
  Valeur1 = digitalRead(BP0); // État du bouton BP0 (sens horaire)
  Valeur2 = digitalRead(BP1); // État du bouton BP1 (non utilisé dans ce code)
  Valeur3 = digitalRead(BP2); // État du bouton BP2 (sens antihoraire)

  // Contrôle du moteur en fonction des boutons
  if (Valeur1 == LOW) { // Bouton BP0 appuyé : sens horaire
    digitalWrite(antihorraires, LOW);  // Désactive le sens antihoraire
    ledcWrite(canal0, 600);          // Définit la vitesse (50% de la puissance)
    lcd.setCursor(0, 0);
    lcd.print("Sens Horaire   ");
  } else if (Valeur3 == LOW) { // Bouton BP2 appuyé : sens antihoraire
    digitalWrite(antihorraires, HIGH); // Active le sens antihoraire
    ledcWrite(canal0, 600);           // Définit la vitesse (50% de la puissance)
    lcd.setCursor(0, 0);
    lcd.print("Sens Antihor.  ");
  } else { // Aucun bouton appuyé : moteur à l'arrêt
    ledcWrite(canal0, 0);             // Arrête le moteur
    lcd.setCursor(0, 0);
    lcd.print("Moteur Arret   ");
  }

  // Affichage des états des boutons sur l'écran LCD (ligne 2)
  lcd.setCursor(0, 1);
  lcd.printf("BP0:%d BP2:%d", Valeur1, Valeur3);
}
