#include "LCDManager.h"

LCDManager::LCDManager(unsigned long i) : lcd(0x27, 16, 2) {
  dernierTemps = 0;
  interval = i;
  indexAffichage = 0;
  urgence = false;
  premier = true;
}

void LCDManager::init() {
  lcd.begin();
  lcd.backlight();
}

void LCDManager::update(bool actif, bool avant, int vitesse, bool urgenceActive) {
  unsigned long tempsActuel = millis();

  // Mode urgence
  if (urgenceActive) {
    if (!urgence) {
      urgence = true;
      afficherUrgence();
    }
    return;
  }

  // Sortie du mode urgence
  if (urgence) {
    urgence = false;
    premier = true;
  }

  // Défilement
  if (premier || tempsActuel - dernierTemps >= interval) {
    dernierTemps = tempsActuel;
    premier = false;

    afficherEcran(actif, avant, vitesse);
    indexAffichage = (indexAffichage + 1) % 3;
  }
}

void LCDManager::afficherEcran(bool actif, bool avant, int vitesse) {
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (indexAffichage) {
    case 0:
      lcd.print("Convoyeur actif");
      lcd.setCursor(0, 1);
      lcd.print(actif ? "OUI" : "NON");
      break;

    case 1:
      lcd.print("Sens convoyeur");
      lcd.setCursor(0, 1);
      if (actif) {
        lcd.print(avant ? "AVANT" : "ARRIERE");
      } else {
        lcd.print("---");
      }
      break;

    case 2:
      lcd.print("Vitesse");
      lcd.setCursor(0, 1);
      lcd.print(vitesse);
      break;
  }
}

void LCDManager::afficherUrgence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("URGENCE");
}
