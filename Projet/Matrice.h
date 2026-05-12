#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

class Matrice {
  private:
    U8G2* u8g2;

    // animation defilement "SPECIAL"
    int decalage;
    unsigned long lastScroll;

    // animation erreur (clignotement)
    bool erreurAllumee;
    unsigned long lastBlink;

    // smiley
    void modeNormal();   
    // SPECIAL qui defile
    void modeRabais();   
    // tout clignote
    void modeErreur();   
    // tout eteint
    void modeFermer();   
    // X
    void modeUrgence();  


  public:
    Matrice(U8G2* display);
    void draw(int mode);
};
