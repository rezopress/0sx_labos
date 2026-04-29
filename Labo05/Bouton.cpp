#include "Bouton.h"

Bouton::Bouton(int p) {
  pin = p;
  pinMode(pin, INPUT_PULLUP);
  dernierEtat = HIGH;
  dernierTemps = 0;
  debounceDelay = 200;
}

bool Bouton::estAppuye() {
  int lecture = digitalRead(pin);
  unsigned long tempsActuel = millis();

  if (lecture == LOW && dernierEtat == HIGH &&
      (tempsActuel - dernierTemps > debounceDelay)) {

    dernierTemps = tempsActuel;
    dernierEtat = lecture;
    return true;
  }

  dernierEtat = lecture;
  return false;
}
