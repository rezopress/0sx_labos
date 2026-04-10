// ============================================================
// LABO 04
// Étudiant :  Lionel G. - DA 6357132
// Cours : 420-0SG-SW - Développement de systèmes embarqués
// ============================================================
#include <Wire.h>
#include <LCD_I2C.h>
#include <Servo.h>
#include <HCSR04.h>

const int TRIGGER_PIN = 9;
const int ECHO_PIN = 10;

Servo servo;
HCSR04 capteurDistance(TRIGGER_PIN, ECHO_PIN);  
LCD_I2C lcd(0x27, 16, 2);

const int PIN_SERVO = 3;
const int PIN_BTN_OUVRIR = 4;
const int PIN_BTN_URGENCE = 5;
const int ANGLE_FERME  = 10;
const int ANGLE_OUVERT = 170;
const int DISTANCE_SEUIL = 3;           
const unsigned int DELAI_OUVERT  = 10000; 
const unsigned int DELAI_SERVO   = 20;    
const unsigned int DELAI_CAPTEUR = 100;   
const unsigned int DEBOUNCE_MS   = 200;

enum Etat { FERMEE, OUVERTURE, OUVERTE, FERMETURE, URGENCE };
Etat etatCourant = FERMEE;

unsigned long currentTime = 0;
unsigned long dernierMouvement = 0;
unsigned long debutOuvert = 0;
int angleActuel = ANGLE_FERME;
bool dernierEtatBtnOuvrir  = HIGH;
bool dernierEtatBtnUrgence = HIGH;
unsigned long debounceBtnOuvrir  = 0;
unsigned long debounceBtnUrgence = 0;

// ============================================================
// Lecture des boutons avec debounce
// ============================================================
void lireBoutons() {

  // Bouton ouverture
  bool lectureBtnOuvrir = digitalRead(PIN_BTN_OUVRIR);

  if (lectureBtnOuvrir == LOW && dernierEtatBtnOuvrir == HIGH) {
    if (currentTime - debounceBtnOuvrir > DEBOUNCE_MS) {
      debounceBtnOuvrir = currentTime;

      if (etatCourant == FERMEE) {
        servo.attach(PIN_SERVO);
        etatCourant = OUVERTURE;
      } else if (etatCourant == FERMETURE) {
        etatCourant = OUVERTURE;
      }
    }
  }
  dernierEtatBtnOuvrir = lectureBtnOuvrir;

  // Bouton urgence 
  bool lectureBtnUrgence = digitalRead(PIN_BTN_URGENCE);

  if (lectureBtnUrgence == LOW && dernierEtatBtnUrgence == HIGH) {
    if (currentTime - debounceBtnUrgence > DEBOUNCE_MS) {
      debounceBtnUrgence = currentTime;

      if (etatCourant != URGENCE) {
        servo.detach();
        etatCourant = URGENCE;
      } else {
        servo.attach(PIN_SERVO);
        etatCourant = FERMETURE;
      }
    }
  }
  dernierEtatBtnUrgence = lectureBtnUrgence;
}

// ============================================================
// État : Porte fermée
// ============================================================
void etatFermee() {
  static unsigned long derniereLecture = 0;

  if (currentTime - derniereLecture < DELAI_CAPTEUR) return;
  derniereLecture = currentTime;

  float distance = capteurDistance.dist();

  if (distance > 0 && distance < DISTANCE_SEUIL) {
    servo.attach(PIN_SERVO);
    etatCourant = OUVERTURE;
  }
}

// ============================================================
// État : Ouverture en cours
// ============================================================
void etatOuverture() {
  if (currentTime - dernierMouvement < DELAI_SERVO) return;
  dernierMouvement = currentTime;

  angleActuel = angleActuel + 1;
  servo.write(angleActuel);

  if (angleActuel >= ANGLE_OUVERT) {
    debutOuvert = currentTime;
    etatCourant = OUVERTE;
  }
}

// ============================================================
// État : Porte ouverte
// ============================================================
void etatOuverte() {
  if (currentTime - debutOuvert >= DELAI_OUVERT) {
    etatCourant = FERMETURE;
  }
}

// ============================================================
// État : Fermeture en cours
// ============================================================
void etatFermeture() {
  static unsigned long derniereLecture = 0;
  if (currentTime - derniereLecture >= DELAI_CAPTEUR){
    derniereLecture = currentTime;

    float distance = capteurDistance.dist();

    if (distance > 0 && distance < DISTANCE_SEUIL) {
      servo.attach(PIN_SERVO);
      etatCourant = OUVERTURE;
      return;
    }
  }

  if (currentTime - dernierMouvement < DELAI_SERVO) return;
  dernierMouvement = currentTime;

  angleActuel = angleActuel - 1;
  servo.write(angleActuel);

  if (angleActuel <= ANGLE_FERME) {
    servo.detach();
    etatCourant = FERMEE;
  }
}

// ============================================================
// Affichage LCD
// ============================================================
void afficherLCD() {
  static Etat dernierEtat = URGENCE; 
  if (etatCourant == dernierEtat) return;
  dernierEtat = etatCourant;

  lcd.clear();
  lcd.setCursor(0, 0);

  switch (etatCourant) {
    case FERMEE:
      lcd.print("Porte fermee");
      break;
    case OUVERTURE:
      lcd.print("Ouverture...");
      break;
    case OUVERTE:
      lcd.print("Porte ouverte");
      break;
    case FERMETURE:
      lcd.print("Fermeture...");
      break;
    case URGENCE:
      lcd.print("ARRET URGENCE");
      break;
  }
}

// ============================================================
//  Setup
// ============================================================
void setup() {
  Serial.begin(115200);

  lcd.begin();
  lcd.backlight();

  pinMode(PIN_BTN_OUVRIR, INPUT_PULLUP);
  pinMode(PIN_BTN_URGENCE, INPUT_PULLUP);

  servo.attach(PIN_SERVO);
  servo.write(ANGLE_FERME);
  servo.detach();

}

// ============================================================
//  Loop
// ============================================================
void loop() {
  currentTime = millis();

  lireBoutons();

  switch (etatCourant) {
    case FERMEE:
      etatFermee();
      break;
    case OUVERTURE:
      etatOuverture();
      break;
    case OUVERTE:
      etatOuverte();
      break;
    case FERMETURE:
      etatFermeture();
      break;
    case URGENCE:
      // Rien, on attend le bouton
      break;
  }

  afficherLCD();

}
