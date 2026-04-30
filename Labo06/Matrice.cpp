#include "Matrice.h"

Matrice::Matrice(U8G2* display) {
  u8g2 = display;
  decalage = 0;
  lastScroll = 0;
  erreurAllumee = false;
  lastBlink = 0;
}

void Matrice::draw(int mode) {
  u8g2->clearBuffer();

  switch (mode) {
    case 0: modeNormal(); break;
    case 1: modeRabais(); break;
    case 2: modeErreur(); break;
    case 3: modeFermer(); break;
    case 4: modeUrgence(); break;
  }

  u8g2->sendBuffer();
}

// Mode Normal : smiley
void Matrice::modeNormal() {
  // Yeux
  u8g2->drawPixel(1, 1);
  u8g2->drawPixel(2, 1);
  u8g2->drawPixel(5, 1);
  u8g2->drawPixel(6, 1);
  u8g2->drawPixel(1, 2);
  u8g2->drawPixel(2, 2);
  u8g2->drawPixel(5, 2);
  u8g2->drawPixel(6, 2);

  // Sourire
  u8g2->drawPixel(0, 4);
  u8g2->drawPixel(1, 5);
  u8g2->drawPixel(2, 6);
  u8g2->drawPixel(3, 6);
  u8g2->drawPixel(4, 6);
  u8g2->drawPixel(5, 6);
  u8g2->drawPixel(6, 5);
  u8g2->drawPixel(7, 4);
}

// Mode Rabais
void Matrice::modeRabais() {
  unsigned long currentTime = millis();

  if (currentTime - lastScroll >= 200) {
    lastScroll = currentTime;
    decalage--;
    // largeur du texte : 7 lettres x 4 px = 28 px + marge apres 8
    if (decalage < -36) decalage = 8;
  }

  u8g2->setFont(u8g2_font_4x6_tr);
  u8g2->drawStr(decalage, 7, "SPECIAL");
}


// Mode Erreur : tout clignote
void Matrice::modeErreur() {
  unsigned long currentTime = millis();

  if (currentTime - lastBlink >= 300) {
    lastBlink = currentTime;
    erreurAllumee = !erreurAllumee;
  }

  if (erreurAllumee) {
    u8g2->drawBox(0, 0, 8, 8);
  }
}


// Mode Fermer : tout eteint
void Matrice::modeFermer() {
  // buffer reste vide
}


// Mode Urgence : X
void Matrice::modeUrgence() {
  u8g2->drawLine(0, 0, 7, 7);
  u8g2->drawLine(0, 7, 7, 0);
}
