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

void LCDManager::update(int etatPorte, bool convoyeurActif, bool convoyeurAvant,int vitesse, int mode, bool urgenceActive) {
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

  // Defilement
  if (premier || tempsActuel - dernierTemps >= interval) {
    dernierTemps = tempsActuel;
    premier = false;

    afficherEcran(etatPorte, convoyeurActif, convoyeurAvant, vitesse, mode);
    indexAffichage = (indexAffichage + 1) % 5;
  }
}

void LCDManager::afficherEcran(int etatPorte, bool convoyeurActif, bool convoyeurAvant, int vitesse, int mode) {
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (indexAffichage) {
    case 0:
      lcd.print("Porte");
      lcd.setCursor(0, 1);
      if (etatPorte == 0) lcd.print("Fermee");
      else if (etatPorte == 1) lcd.print("Ouverture");
      else if (etatPorte == 2) lcd.print("Ouverte");
      else if (etatPorte == 3) lcd.print("Fermeture");
      break;

    case 1:
      lcd.print("Convoyeur actif");
      lcd.setCursor(0, 1);
      lcd.print(convoyeurActif ? "OUI" : "NON");
      break;

    case 2:
      lcd.print("Sens convoyeur");
      lcd.setCursor(0, 1);
      if (convoyeurActif) {
        lcd.print(convoyeurAvant ? "AVANT" : "ARRIERE");
      } else {
        lcd.print("---");
      }
      break;

    case 3:
      lcd.print("Vitesse");
      lcd.setCursor(0, 1);
      lcd.print(vitesse);
      break;

    case 4:
      lcd.print("Mode :");
      lcd.setCursor(0, 1);
      if (mode == 0) lcd.print("Normal");
      else if (mode == 1) lcd.print("Rabais");
      else if (mode == 2) lcd.print("Erreur");
      else if (mode == 3) lcd.print("Fermer");
      break;
  }
}

void LCDManager::afficherUrgence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("URGENCE");
}
