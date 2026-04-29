#pragma once

#include <Arduino.h>

class Bouton {
  private:
    int pin;
    int dernierEtat;
    unsigned long dernierTemps;
    unsigned long debounceDelay;

  public:
    Bouton(int p);
    bool estAppuye();
};
