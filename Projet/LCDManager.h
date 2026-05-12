#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LCD_I2C.h>

class LCDManager {
  private:
    LCD_I2C lcd;
    unsigned long dernierTemps;
    unsigned long interval;
    int indexAffichage;
    bool urgence;
    bool premier;

    void afficherEcran(int etatPorte, bool convoyeurActif, bool convoyeurAvant, int vitesse, int mode);
    void afficherUrgence();

  public:
    LCDManager(unsigned long i);
    void init();
    void update(int etatPorte, bool convoyeurActif, bool convoyeurAvant, int vitesse, int mode, bool urgenceActive);
};
