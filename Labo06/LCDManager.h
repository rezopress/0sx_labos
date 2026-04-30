#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <LCD_I2C.h>

class LCDManager {
  private:
    LCD_I2C lcd;
    int dernierMode;

  public:
    LCDManager();
    void init();
    void update(int mode);
};
