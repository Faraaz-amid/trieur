#include <Arduino.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include "Adafruit_TCS34725.h"
#include "MFRC522_I2C.h"
#include <CAN.h>

MFRC522 mfrc522(0x28);  // Création de l'instance MFRC522 pour le lecteur RFID.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);  // Initialisation du capteur de couleur TCS34725.
rgb_lcd lcd;  // Création de l'objet LCD.

int BP0 = 0;  // Bouton 0 (sens horaire).
int BP1 = 2;  // Bouton 1 (non utilisé).
int BP2 = 12; // Bouton 2 (sens antihoraire).
int pot = 33; // Potentiomètre.
int lecture_pot; // Variable pour la lecture du potentiomètre.

int antihorraires = 26; // Broche moteur sens antihoraire.
int pwmA = 27; // Broche PWM moteur.

int frequence = 50;  // Fréquence PWM.
int canal0 = 0;  // Canal PWM moteur.
int resolution = 11;  // Résolution PWM.

byte authorizedUID[] = {0xE9, 0x44, 0x20, 0x7A};  // UID du badge autorisé.
unsigned long previousMillis = 0; // Variable pour stocker le temps écoulé.
bool moteurActif = false;  // Variable pour vérifier si le moteur est actif.
unsigned long moteurTime = 0; // Durée pendant laquelle le moteur reste allumé.



void setup() {
  Serial.begin(115200);  // Initialisation de la communication série.

  pinMode(BP0, INPUT_PULLUP);
  pinMode(BP1, INPUT_PULLUP);
  pinMode(BP2, INPUT_PULLUP);
  pinMode(antihorraires, OUTPUT);

  Wire1.setPins(15, 5); // Communication I2C.
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1); // Initialisation de l'écran LCD.
  lcd.setColor(BLUE);  // Couleur de fond de l'écran.

  ledcSetup(canal0, frequence, resolution); // Configuration PWM moteur.
  ledcAttachPin(pwmA, canal0);

  ledcWrite(canal0, 0);  // Arrêter le moteur au démarrage.

  Serial.println("CAN OBD-II VIN reader");
  if (!CAN.begin(500E3)) {
    Serial.println("Starting CAN failed!");
    while (1);
  }

  // add filter to only receive the CAN bus ID's we care about
  if (useStandardAddressing) {
    CAN.filter(0x7e8);
  } else {
    CAN.filterExtended(0x18daf110);
  }
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  mfrc522.PCD_Init();
}




void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(200);
    return;
  }

  // Comparer l'UID lu avec l'UID autorisé.
  bool isAuthorized = true;
  for (byte i = 0; i < sizeof(authorizedUID); i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
      isAuthorized = false;
      break;
    }
  }

  if (isAuthorized && !moteurActif) {
    // Si le badge est autorisé et que le moteur n'est pas déjà actif
    ledcWrite(canal0, 1800);  // Augmenter la vitesse du moteur à 1800 (environ 90% de la vitesse max).
    lcd.setCursor(0, 0);
    lcd.print("Moteur ON    ");
    
    // Démarrer un compte à rebours de 7 secondes pour éteindre le moteur
    moteurTime = millis();
    moteurActif = true;
  }

  if (moteurActif) {
    // Afficher la durée restante du moteur sur l'écran LCD
    unsigned long elapsedTime = millis() - moteurTime;
    unsigned long remainingTime = 7000 - elapsedTime;
    if (remainingTime > 0) {
      lcd.setCursor(0, 1);
      lcd.printf("Temps restant: %lu", remainingTime / 1000);  // Afficher les secondes restantes
    } else {
      // Après 7 secondes, éteindre le moteur et revenir en mode détection
      ledcWrite(canal0, 0);  // Moteur éteint
      lcd.setCursor(0, 1);
      lcd.print("Moteur OFF   ");
      moteurActif = false;
      delay(500);  // Attendre un peu avant de revenir en mode détection.
    }
  }

  // Contrôle de la direction du moteur avec les boutons BP0 et BP2.
  int Valeur1 = digitalRead(BP0); // Sens horaire.
  int Valeur2 = digitalRead(BP1); // Non utilisé.
  int Valeur3 = digitalRead(BP2); // Sens antihoraire.

  if (Valeur1 == LOW) {
    digitalWrite(antihorraires, LOW);
    ledcWrite(canal0, 600); // Sens horaire.
    lcd.setCursor(0, 0);
    lcd.print("Sens Horaire");
  } else if (Valeur3 == LOW) {
    digitalWrite(antihorraires, HIGH);
    ledcWrite(canal0, 600); // Sens antihoraire.
    lcd.setCursor(0, 0);
    lcd.print("Sens Antihor.");
  } else {
    ledcWrite(canal0, 0); // Arrêt du moteur.
    lcd.setCursor(0, 0);
    lcd.print("Moteur Arret");
  }

  delay(200);  // Petit délai pour éviter de surcharger le processeur.
} 