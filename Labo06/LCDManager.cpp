#include "LCDManager.h"

LCDManager::LCDManager() : lcd(0x27, 16, 2) {
  dernierMode = -1;
}

void LCDManager::init() {
  lcd.begin();
  lcd.backlight();
}

void LCDManager::update(int mode) {
  if (mode == dernierMode) return;
  dernierMode = mode;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode :");
  lcd.setCursor(0, 1);

  switch (mode) {
    case 0: lcd.print("Normal");  break;
    case 1: lcd.print("Rabais");  break;
    case 2: lcd.print("Erreur");  break;
    case 3: lcd.print("Fermer");  break;
    case 4: lcd.print("Urgence"); break;
  }
}
