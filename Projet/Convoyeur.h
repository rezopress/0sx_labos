#pragma once
#include <Arduino.h>

enum EtatConvoyeur { CONV_ARRET, CONV_AVANCE, CONV_RECULE };

class Convoyeur {
  private:
    int pinIn1;
    int pinIn2;
    int pinLedMarche;
    int pinJoyX;
    int pinJoyY;

    EtatConvoyeur etat;
    bool urgenceActive;

    int speedMotor;
    int joyX;
    int joyY;
    unsigned long lTimeVitesse;

    void lireJoystick();
    void ajusterVitesse(unsigned long currentTime);
    void stopMoteur();
    void etatArret();
    void etatAvance();
    void etatRecule();

  public:
    Convoyeur(int pinIn1, int pinIn2, int pinLedMarche, int pinJoyX, int pinJoyY);
    void init();
    void update(unsigned long currentTime);
    void setUrgence(bool actif);

    EtatConvoyeur getEtat();
    int getVitesse();
};
