#pragma once
#include <Arduino.h>

class Bouton {
  private:
    int pin;
    int dernierEtat;
    unsigned long debutLow;
    bool enAttente;

  public:
    Bouton(int p);
    bool estAppuye();
};
