// ============================================================
// LABO 05 - Convoyeur de caisse
// Étudiant : Lionel G. - DA 6357132
// Cours : 420-0SX - Développement de systèmes embarqués
// ============================================================
#include "Bouton.h"
#include "LCDManager.h"

// Pins
const int input1 = 44;
const int input2 = 45;
const int PIN_LED_MARCHE  = 7;
const int PIN_LED_URGENCE = 8;
const int PIN_BTN_URGENCE = 4;
const int PIN_JOY_X = A0;
const int PIN_JOY_Y = A1;

// Machine à états
enum Etat { ARRET, AVANCE, RECULE, URGENCE };
Etat cState = ARRET;

// Variables contrôle moteur
int speedMotor = 150;
const int minSpeedMotor = 80;    
const int maxSpeedMotor = 255;
const int pasVitesse = 10;

// Seuils joystick
const int SEUIL_HAUT = 700;
const int SEUIL_BAS = 300;
const int SEUIL_DROITE = 700;
const int SEUIL_GAUCHE = 300;

// Timing
unsigned long cTime = 0;
unsigned long lTimeVitesse = 0;
const unsigned long DELAI_VITESSE = 150;

// Lectures joystick
int joyX = 512;
int joyY = 512;

// POO
Bouton btnUrgence(PIN_BTN_URGENCE);
LCDManager lcdManager(2000);


// Setup
void setup() {
  Serial.begin(9600);

  pinMode(input1, OUTPUT);
  pinMode(input2, OUTPUT);
  pinMode(PIN_LED_MARCHE, OUTPUT);
  pinMode(PIN_LED_URGENCE, OUTPUT);

  stopMoteur();
  lcdManager.init();
}

// Loop
void loop() {
  cTime = millis();

  lireJoystick();
  verifierUrgence();

  switch (cState) {
    case ARRET: etatArret();   break;
    case AVANCE: etatAvance();  break;
    case RECULE: etatRecule();  break;
    case URGENCE: etatUrgence(); break;
  }

  updateLEDs();
  updateLCD();
}

// Commandes moteur
void stopMoteur() {
  analogWrite(input1, 0);
  analogWrite(input2, 0);
}

// Lecture joystick + gestion urgence
void lireJoystick() {
  joyX = analogRead(PIN_JOY_X);
  joyY = analogRead(PIN_JOY_Y);
}

void verifierUrgence() {
  if (btnUrgence.estAppuye()) {
    if (cState == URGENCE) {
      cState = ARRET;
    } else {
      stopMoteur();
      cState = URGENCE;
    }
  }
}

// Ajuster la vitesse via le joystick 
void ajusterVitesse() {
  if (cTime - lTimeVitesse < DELAI_VITESSE) return;

  if (joyX > SEUIL_DROITE) {
    speedMotor += pasVitesse;
    lTimeVitesse = cTime;
  } else if (joyX < SEUIL_GAUCHE) {
    speedMotor -= pasVitesse;
    lTimeVitesse = cTime;
  }

  if (speedMotor > maxSpeedMotor) speedMotor = maxSpeedMotor;
  if (speedMotor < minSpeedMotor) speedMotor = minSpeedMotor;
}

// État : ARRET
void etatArret() {
  static bool firstTime = true;
  if (firstTime) {
    Serial.println("ARRET");
    stopMoteur();
    firstTime = false;
  }

  // Transition
  if (joyY > SEUIL_HAUT) {
    firstTime = true;
    cState = AVANCE;
  } else if (joyY < SEUIL_BAS) {
    firstTime = true;
    cState = RECULE;
  }
}

// État : AVANCE
void etatAvance() {
  static bool firstTime = true;
  if (firstTime) {
    Serial.println("AVANCE");
    firstTime = false;
  }

  // Exécution - sens 1
  ajusterVitesse();
  analogWrite(input1, speedMotor);
  analogWrite(input2, LOW);

  // Transition
  if (joyY < SEUIL_BAS) {
    firstTime = true;
    cState = RECULE;
  } else if (joyY >= SEUIL_BAS && joyY <= SEUIL_HAUT) {
    firstTime = true;
    cState = ARRET;
  }
}

// État : RECULE
void etatRecule() {
  static bool firstTime = true;
  if (firstTime) {
    Serial.println("RECULE");
    firstTime = false;
  }

  // Exécution - sens 2
  ajusterVitesse();
  analogWrite(input1, LOW);
  analogWrite(input2, speedMotor);

  // Transition
  if (joyY > SEUIL_HAUT) {
    firstTime = true;
    cState = AVANCE;
  } else if (joyY >= SEUIL_BAS && joyY <= SEUIL_HAUT) {
    firstTime = true;
    cState = ARRET;
  }
}

// État : URGENCE
void etatUrgence() {
  static bool firstTime = true;
  if (firstTime) {
    Serial.println("URGENCE");
    firstTime = false;
  }

  if (cState != URGENCE) {
    firstTime = true;
  }
}

// Gestion LEDs
void updateLEDs() {
  bool enMouvement = (cState == AVANCE || cState == RECULE);
  digitalWrite(PIN_LED_MARCHE, enMouvement ? HIGH : LOW);
  digitalWrite(PIN_LED_URGENCE, (cState == URGENCE) ? HIGH : LOW);
}

// Gestion LCD
void updateLCD() {
  bool actif = (cState == AVANCE || cState == RECULE);
  bool avant = (cState == AVANCE);
  bool urgence = (cState == URGENCE);
  lcdManager.update(actif, avant, speedMotor, urgence);
}
