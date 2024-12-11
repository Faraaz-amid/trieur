#include <Arduino.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
int BP0=0,BP1=2,BP2=12;
int Valeur1;
int Valeur2;
int Valeur3;
void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);
  pinMode(BP0,INPUT_PULLUP);
  pinMode(BP1,INPUT_PULLUP);
  pinMode(BP2,INPUT_PULLUP);
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  lcd.setColor(BLUE);
}


void loop() {
  Valeur1=digitalRead(BP0);
  Valeur2=digitalRead(BP1);
  Valeur3=digitalRead(BP2);
 // Initialise l'écran LCD
  lcd.setCursor(1, 0);
  lcd.printf("bp0 %d, bp2 %d,bp1 %d",Valeur1,Valeur2,Valeur3);
 


}
