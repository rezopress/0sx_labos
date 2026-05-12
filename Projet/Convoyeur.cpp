#include "Convoyeur.h"

const int MIN_SPEED = 80;
const int MAX_SPEED = 255;
const int PAS_VITESSE = 10;
const int SEUIL_HAUT = 700;
const int SEUIL_BAS = 300;
const int SEUIL_DROITE = 700;
const int SEUIL_GAUCHE = 300;
const unsigned long DELAI_VITESSE = 150;

Convoyeur::Convoyeur(int pinIn1, int pinIn2, int pinLedMarche, int pinJoyX, int pinJoyY) {
  this->pinIn1 = pinIn1;
  this->pinIn2 = pinIn2;
  this->pinLedMarche = pinLedMarche;
  this->pinJoyX = pinJoyX;
  this->pinJoyY = pinJoyY;

  etat = CONV_ARRET;
  urgenceActive = false;
  speedMotor = 150;
  joyX = 512;
  joyY = 512;
  lTimeVitesse = 0;
}

void Convoyeur::init() {
  pinMode(pinIn1, OUTPUT);
  pinMode(pinIn2, OUTPUT);
  pinMode(pinLedMarche, OUTPUT);
  stopMoteur();
}

void Convoyeur::setUrgence(bool actif) {
  urgenceActive = actif;
  if (actif) {
    stopMoteur();
    etat = CONV_ARRET;
  }
}

EtatConvoyeur Convoyeur::getEtat() { return etat; }

int Convoyeur::getVitesse() {
  if (etat == CONV_ARRET) return 0;
  return speedMotor;
}

void Convoyeur::stopMoteur() {
  analogWrite(pinIn1, 0);
  analogWrite(pinIn2, 0);
}

void Convoyeur::update(unsigned long currentTime) {
  if (urgenceActive) {
    digitalWrite(pinLedMarche, LOW);
    return;
  }

  lireJoystick();
  ajusterVitesse(currentTime);

  switch (etat) {
    case CONV_ARRET: etatArret(); break;
    case CONV_AVANCE: etatAvance(); break;
    case CONV_RECULE: etatRecule(); break;
  }

  digitalWrite(pinLedMarche, (etat != CONV_ARRET) ? HIGH : LOW);
}

void Convoyeur::lireJoystick() {
  joyX = analogRead(pinJoyX);
  joyY = analogRead(pinJoyY);
}

void Convoyeur::ajusterVitesse(unsigned long currentTime) {
  if (currentTime - lTimeVitesse < DELAI_VITESSE) return;

  if (joyX > SEUIL_DROITE) {
    speedMotor += PAS_VITESSE;
    lTimeVitesse = currentTime;
  } else if (joyX < SEUIL_GAUCHE) {
    speedMotor -= PAS_VITESSE;
    lTimeVitesse = currentTime;
  }

  if (speedMotor > MAX_SPEED) speedMotor = MAX_SPEED;
  if (speedMotor < MIN_SPEED) speedMotor = MIN_SPEED;
}

void Convoyeur::etatArret() {
  stopMoteur();
  if (joyY > SEUIL_HAUT) etat = CONV_AVANCE;
  else if (joyY < SEUIL_BAS) etat = CONV_RECULE;
}

void Convoyeur::etatAvance() {
  analogWrite(pinIn1, speedMotor);
  analogWrite(pinIn2, LOW);

  if (joyY < SEUIL_BAS) etat = CONV_RECULE;
  else if (joyY >= SEUIL_BAS && joyY <= SEUIL_HAUT) etat = CONV_ARRET;
}

void Convoyeur::etatRecule() {
  analogWrite(pinIn1, LOW);
  analogWrite(pinIn2, speedMotor);

  if (joyY > SEUIL_HAUT) etat = CONV_AVANCE;
  else if (joyY >= SEUIL_BAS && joyY <= SEUIL_HAUT) etat = CONV_ARRET;
}
