#pragma once
#include <Arduino.h>
#include <Servo.h>
#include <HCSR04.h>

enum EtatPorte { PORTE_FERMEE, PORTE_OUVERTURE, PORTE_OUVERTE, PORTE_FERMETURE };

class Porte {
  private:
    Servo servo;
    HCSR04 capteur;

    int pinServo;
    int pinBouton;
    int angleActuel;

    EtatPorte etat;
    bool urgenceActive;
    unsigned int compteurClients;

    unsigned long dernierMouvement;
    unsigned long debutOuvert;
    bool dernierEtatBtn;
    unsigned long debounceBtn;

    void etatFermee(unsigned long currentTime);
    void etatOuverture(unsigned long currentTime);
    void etatOuverte(unsigned long currentTime);
    void etatFermeture(unsigned long currentTime);
    void lireBouton(unsigned long currentTime);

  public:
    Porte(int pinServo, int pinTrigger, int pinEcho, int pinBouton);
    void init();
    void update(unsigned long currentTime);
    void setUrgence(bool actif);

    EtatPorte getEtat();
    unsigned int getCompteurClients();
};
