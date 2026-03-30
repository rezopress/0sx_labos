// ============================================================
// LABO 03
// Étudiant : GACHARD - DA 6357132 (pair -> thermistance)
// Cours : 420-0SG-SW - Développement de systèmes embarqués
// ============================================================

#include <Wire.h>
#include <LCD_I2C.h>

// === LCD I2C ===
LCD_I2C lcd(0x27, 16, 2);

// === Broches ===
const int PIN_CAPTEUR  = A0;
const int PIN_JOY_X    = A1;
const int PIN_JOY_Y    = A2;
const int PIN_JOY_BTN  = 2;
const int PIN_LED      = 8;

// === Info étudiant ===
const char NOM[]    = "GACHARD";
const char NUM_ETD[] = "6357132";

// === Caractère personnalisé "32" (5x8) ===
byte charPerso[8] = {
  0b11100,  // ***
  0b00100,  //   * 
  0b11100,  // ***
  0b00111,  //   ***
  0b11101,  // *** *
  0b00111,  //   ***
  0b00100,  //   *
  0b00111   //   ***
};

// === Thermistance ===
float R1 = 10000;
float c1 = 1.129148e-03;
float c2 = 2.34125e-04;
float c3 = 8.76741e-08;

// === Variables globales ===
unsigned long currentTime = 0;

float altitude       = 0.0;
bool coolingOn       = false;
int pageActuelle     = 0;
bool dernierEtatBtn  = HIGH;
unsigned long tDernierDebounce = 0;
bool demarrageFini   = false;

// === Constantes ===
const int JOY_CENTRE   = 512;
const int JOY_DEADZONE = 50;
const float ALTITUDE_MAX = 200.0;
const float VITESSE_MS   = 1.0;
const unsigned long DEBOUNCE_MS  = 200;
const unsigned long SERIAL_RATE  = 500;
const unsigned long LCD_RATE     = 250;
const unsigned long DEMARRAGE_MS = 3000;

// ============================================================
//  Masque le numéro étudiant : "*****32"
// ============================================================
void masquerNumEtd(char resultat[]) {
  for (int i = 0; i < 7; i++) {
    if (i < 5) {
      resultat[i] = '*';
    } else {
      resultat[i] = NUM_ETD[i];
    }
  }
}

// ============================================================
//  Lecture de la température (thermistance)
// ============================================================
float lireTemperature(int raw) {
  if (raw <= 0) raw = 1;

  float R2 = R1 * (1023.0 / (float)raw - 1.0);
  float logR2 = log(R2);
  float T = 1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2);
  float Tc = T - 273.15;

  return Tc;
}

// ============================================================
//  Mise à jour de l'altitude selon le joystick Y
// ============================================================
void majAltitude(int rawY) {
  static unsigned long lastTime = 0;

  if (lastTime == 0) {
    lastTime = currentTime;
    return;
  }

  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  if (rawY > JOY_CENTRE + JOY_DEADZONE) {
    altitude = altitude + VITESSE_MS * dt;
  } else if (rawY < JOY_CENTRE - JOY_DEADZONE) {
    altitude = altitude - VITESSE_MS * dt;
  }

  // Limiter entre 0 et 200 m
  if (altitude < 0.0) {
    altitude = 0.0;
  }
  
  if (altitude > ALTITUDE_MAX) {
    altitude = ALTITUDE_MAX;
  }
}

// ============================================================
//  Gestion du bouton du joystick (changement de page)
// ============================================================
void gererBouton() {
  bool lecture = digitalRead(PIN_JOY_BTN);

  if (lecture == LOW && dernierEtatBtn == HIGH) {
    if (currentTime - tDernierDebounce > DEBOUNCE_MS) {
      pageActuelle = (pageActuelle + 1) % 2;
      tDernierDebounce = currentTime;
    }
  }
  dernierEtatBtn = lecture;
}

// ============================================================
//  Affichage LCD - Tâche périodique (250 ms)
//  Page 0 : Altitude + Direction
//  Page 1 : Température + Ventilation
// ============================================================
void afficherLCD(float tempC, int angle, int rawY) {
  // Static pour garder la valeur entre chaque tour
  static unsigned long lastTime = 0;

  if (currentTime - lastTime < LCD_RATE) return;
  lastTime = currentTime;

  lcd.clear();

  if (pageActuelle == 0) {
    // Ligne 1 : ALT: XXXm UP/DOWN
    lcd.setCursor(0, 0);
    lcd.print("ALT: ");
    lcd.print((int)altitude);
    lcd.print("m ");
    if (rawY > JOY_CENTRE + JOY_DEADZONE) {
      lcd.print("UP");
    } else if (rawY < JOY_CENTRE - JOY_DEADZONE) {
      lcd.print("DOWN");
    }

    // Ligne 2 : DIR: XX(D) ou DIR: -XX(G)
    lcd.setCursor(0, 1);
    lcd.print("DIR: ");
    lcd.print(angle);
    if (angle >= 0) {
      lcd.print("(D)");
    } else {
      lcd.print("(G)");
    }

  } else {
    // Ligne 1 : température
    lcd.setCursor(0, 0);
    lcd.print(tempC, 1);
    lcd.print(" C");

    // Ligne 2 : état ventilation
    lcd.setCursor(0, 1);
    lcd.print("COOL: ");
    if (coolingOn) {
      lcd.print("ON");
    } else {
      lcd.print("OFF");
    }
  }
}

// ============================================================
//  Transmission série - Tâche périodique (500 ms)
// ============================================================
void envoyerSerie(int rawX, int rawY) {
  // Static pour garder la valeur entre chaque tour
  static unsigned long lastTime = 0;

  if (currentTime - lastTime < SERIAL_RATE) return;
  lastTime = currentTime;

  Serial.print(F("etd:"));
  Serial.print(NUM_ETD);
  Serial.print(F(",x:"));
  Serial.print(rawX);
  Serial.print(F(",y:"));
  Serial.print(rawY);
  Serial.print(F(",sys:"));
  if (coolingOn) {
    Serial.println(1);
  } else {
    Serial.println(0);
  }
}

// ============================================================
//  Écran de démarrage (3 secondes)
// ============================================================
void ecranDemarrage() {
  // Static pour garder la valeur entre chaque tour
  static unsigned long debutDemarrage = 0;

  if (debutDemarrage == 0) {
    debutDemarrage = currentTime;

    char numMasque[7];
    masquerNumEtd(numMasque);

    lcd.setCursor(0, 0);
    lcd.print(NOM);

    lcd.setCursor(0, 1);
    lcd.write(0);
    lcd.setCursor(9, 1);
    lcd.print(numMasque);
  }

  if (currentTime - debutDemarrage >= DEMARRAGE_MS) {
    demarrageFini = true;
    lcd.clear();
  }
}

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_JOY_BTN, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  lcd.begin();
  lcd.createChar(0, charPerso);
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  currentTime = millis();

  // Écran de démarrage (3 secondes)
  if (!demarrageFini) {
    ecranDemarrage();
    return;
  }

  // Lectures analogiques
  int rawCapteur = analogRead(PIN_CAPTEUR);
  int rawX       = analogRead(PIN_JOY_X);
  int rawY       = analogRead(PIN_JOY_Y);

  // Température
  float tempC = lireTemperature(rawCapteur);

  // Logique ventilation
  if (tempC > 35.0) {
    coolingOn = true;
  } else if (tempC < 30.0) {
    coolingOn = false;
  }
  digitalWrite(PIN_LED, coolingOn);

  // Altitude
  majAltitude(rawY);

  // Direction : angle -90° à +90°
  int angle = map(rawX, 0, 1023, -90, 90);

  // Bouton changement de page
  gererBouton();

  // Affichage LCD
  afficherLCD(tempC, angle, rawY);

  // Transmission série
  envoyerSerie(rawX, rawY);


}
