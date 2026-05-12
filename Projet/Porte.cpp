#include "Porte.h"

const int ANGLE_FERME = 10;
const int ANGLE_OUVERT = 170;
const int DISTANCE_SEUIL = 8;
const unsigned long DELAI_OUVERT = 10000;
const unsigned long DELAI_SERVO = 20;
const unsigned long DELAI_CAPTEUR = 100;
const unsigned long DEBOUNCE_MS = 200;

Porte::Porte(int pinServo, int pinTrigger, int pinEcho, int pinBouton) : capteur(pinTrigger, pinEcho) {
  this->pinServo = pinServo;
  this->pinBouton = pinBouton;
  angleActuel = ANGLE_FERME;
  etat = PORTE_FERMEE;
  urgenceActive = false;
  compteurClients = 0;
  dernierMouvement = 0;
  debutOuvert = 0;
  dernierEtatBtn = HIGH;
  debounceBtn = 0;
}

void Porte::init() {
  pinMode(pinBouton, INPUT_PULLUP);
  servo.attach(pinServo);
  servo.write(ANGLE_FERME);
  delay(500);
  servo.detach();
}

void Porte::setUrgence(bool actif) {
  urgenceActive = actif;
  if (actif) {
    servo.detach();
  } else {
    servo.attach(pinServo);
    etat = PORTE_FERMETURE;
  }
}

EtatPorte Porte::getEtat() { return etat; }
unsigned int Porte::getCompteurClients() { return compteurClients; }

void Porte::update(unsigned long currentTime) {
  if (urgenceActive) return;

  lireBouton(currentTime);

  switch (etat) {
    case PORTE_FERMEE: etatFermee(currentTime);    break;
    case PORTE_OUVERTURE: etatOuverture(currentTime); break;
    case PORTE_OUVERTE: etatOuverte(currentTime);   break;
    case PORTE_FERMETURE: etatFermeture(currentTime); break;
  }
}

void Porte::lireBouton(unsigned long currentTime) {
  bool lecture = digitalRead(pinBouton);

  if (lecture == LOW && dernierEtatBtn == HIGH) {
    if (currentTime - debounceBtn > DEBOUNCE_MS) {
      debounceBtn = currentTime;
      if (etat == PORTE_FERMEE) {
        servo.attach(pinServo);
        etat = PORTE_OUVERTURE;
        compteurClients++;
      } else if (etat == PORTE_FERMETURE) {
        etat = PORTE_OUVERTURE;
        compteurClients++;
      }
    }
  }
  dernierEtatBtn = lecture;
}

void Porte::etatFermee(unsigned long currentTime) {
  static unsigned long derniereLecture = 0;
  if (currentTime - derniereLecture < DELAI_CAPTEUR) return;
  derniereLecture = currentTime;

  float distance = capteur.dist();
  if (distance > 0 && distance < DISTANCE_SEUIL) {
    servo.attach(pinServo);
    etat = PORTE_OUVERTURE;
    compteurClients++;
  }
}

void Porte::etatOuverture(unsigned long currentTime) {
  if (currentTime - dernierMouvement < DELAI_SERVO) return;
  dernierMouvement = currentTime;

  angleActuel = angleActuel + 1;
  servo.write(angleActuel);

  if (angleActuel >= ANGLE_OUVERT) {
    debutOuvert = currentTime;
    etat = PORTE_OUVERTE;
  }
}

void Porte::etatOuverte(unsigned long currentTime) {
  if (currentTime - debutOuvert >= DELAI_OUVERT) {
    etat = PORTE_FERMETURE;
  }
}

void Porte::etatFermeture(unsigned long currentTime) {
  static unsigned long derniereLecture = 0;
  if (currentTime - derniereLecture >= DELAI_CAPTEUR) {
    derniereLecture = currentTime;
    float distance = capteur.dist();
    if (distance > 0 && distance < DISTANCE_SEUIL) {
      servo.attach(pinServo);
      etat = PORTE_OUVERTURE;
      compteurClients++;
      return;
    }
  }

  if (currentTime - dernierMouvement < DELAI_SERVO) return;
  dernierMouvement = currentTime;

  angleActuel = angleActuel - 1;
  servo.write(angleActuel);

  if (angleActuel <= ANGLE_FERME) {
    servo.detach();
    etat = PORTE_FERMEE;
  }
}
