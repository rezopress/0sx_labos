#include "Bouton.h"

Bouton::Bouton(int p) {
  pin = p;
  pinMode(pin, INPUT_PULLUP);
  dernierEtat = HIGH;
  debutLow = 0;
  enAttente = false;
}

bool Bouton::estAppuye() {
  int lecture = digitalRead(pin);

  if (lecture == LOW) {
    if (!enAttente && dernierEtat == HIGH) {
      debutLow = millis();
      enAttente = true;
    } else if (enAttente && millis() - debutLow >= 50) {
      dernierEtat = LOW;
      enAttente = false;
      return true;
    }
  } else {
    enAttente = false;
    dernierEtat = HIGH;
  }
  return false;
}
