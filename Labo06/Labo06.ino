// ============================================================
// LABO 06
// Étudiant :  Lionel G. - DA 6357132
// Cours : 420-0SG-SW - Développement de systèmes embarqués
// ============================================================

#include <Arduino.h>
#include <U8g2lib.h>
#include <IRremote.hpp>
#include "Bouton.h"
#include "LCDManager.h"
#include "Matrice.h"

// Pins matrice 
#define CLK_PIN 30
#define DIN_PIN 34
#define CS_PIN  32

// Autres pins
const int IR_PIN = 2;
const int BTN_URGENCE = 4;

// Codes IR 

// Mode Normal
const uint16_t IR_TOUCHE_1 = 0xC;  
// Mode Rabais
const uint16_t IR_TOUCHE_2 = 0x18;  
// Mode Erreur
const uint16_t IR_TOUCHE_3 = 0x5E;  
// Mode Fermer
const uint16_t IR_TOUCHE_4 = 0x8;  

// Modes
enum Mode { NORMAL, RABAIS, ERREUR, FERMER, URGENCE };
Mode cMode = NORMAL;
Mode modePrecedent = NORMAL;

// Matrice
U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2(
  U8G2_R0,
  CLK_PIN,
  DIN_PIN,
  CS_PIN,
  U8X8_PIN_NONE,
  U8X8_PIN_NONE
);

// Objets
Matrice matrice(&u8g2);
LCDManager lcdManager;
Bouton btnUrgence(BTN_URGENCE);

void setup() {
  Serial.begin(9600);

  u8g2.begin();
  u8g2.setContrast(5);

  lcdManager.init();

  IrReceiver.begin(IR_PIN);

  Serial.println("Pret. Commandes : 1=Normal 2=Rabais 3=Erreur 4=Fermer");
}

void loop() {
  lireIR();
  verifierUrgence();

  matrice.draw(cMode);
  lcdManager.update(cMode);
}

// Lecture telecommande IR
void lireIR() {
  if (IrReceiver.decode()) {
    uint16_t code = IrReceiver.decodedIRData.command;

    Serial.print("IR recu : 0x");
    Serial.println(code, HEX);

    if (cMode != URGENCE) {
      if (code == IR_TOUCHE_1) cMode = NORMAL;
      else if (code == IR_TOUCHE_2) cMode = RABAIS;
      else if (code == IR_TOUCHE_3) cMode = ERREUR;
      else if (code == IR_TOUCHE_4) cMode = FERMER;
    }

    IrReceiver.resume();
  }
}

// Bouton urgence
void verifierUrgence() {
  if (btnUrgence.estAppuye()) {
    if (cMode == URGENCE) {
      // Sortie d'urgence
      cMode = modePrecedent;
    } else {
      // Entree en urgence
      modePrecedent = cMode;
      cMode = URGENCE;
    }
  }
}

// Reception serie automatique 
void serialEvent() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (cMode != URGENCE) {
      if (c == '1') cMode = NORMAL;
      else if (c == '2') cMode = RABAIS;
      else if (c == '3') cMode = ERREUR;
      else if (c == '4') cMode = FERMER;
    }
  }
}
