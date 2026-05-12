// ============================================================
// PROJET DE SESSION
// Etudiant : Lionel G. - DA 6357132
// Cours : 420-0SX - Developpement de systemes embarques
// ============================================================
#include <Wire.h>
#include <U8g2lib.h>
#include <IRremote.hpp>
#include <WiFiEspAT.h>
#include "Bouton.h"
#include "Porte.h"
#include "Convoyeur.h"
#include "Matrice.h"
#include "LCDManager.h"
#include "MqttManager.h"

// Pins
const int PIN_IR = 2;
const int PIN_SERVO = 3;
const int PIN_BTN_URGENCE = 4;
const int PIN_BTN_OUVRIR = 6;
const int PIN_LED_MARCHE = 44;
const int PIN_LED_URGENCE = 45;
const int PIN_TRIGGER = 9;
const int PIN_ECHO = 10;
const int PIN_MOTEUR_IN1 = 7;
const int PIN_MOTEUR_IN2 = 8;
const int PIN_JOY_X = A0;
const int PIN_JOY_Y = A1;

#define CLK_PIN 30
#define DIN_PIN 34
#define CS_PIN  32

// Codes IR
const uint16_t IR_TOUCHE_1 = 0xC;
const uint16_t IR_TOUCHE_2 = 0x18;
const uint16_t IR_TOUCHE_3 = 0x5E;
const uint16_t IR_TOUCHE_4 = 0x8;

// Configuration WiFi / MQTT
#define AT_BAUD_RATE 115200
const char* WIFI_SSID = "TechniquesInformatique-Etudiant";
const char* WIFI_PASS = "shawi123";

const char* MQTT_SERVEUR = "216.128.180.194";
const int   MQTT_PORT  = 1883;
const char* MQTT_USER = "etdshawi";
const char* MQTT_PASS = "shawi123";

const char* DEVICE_NAME = "LionelG_Mega";
const char* TOPIC_PUB = "magasin/5/state";
const char* TOPIC_SUB = "magasin/5/set";

// Etat global
enum Mode { NORMAL, RABAIS, ERREUR, FERMER, URGENCE };
Mode cMode = NORMAL;
Mode modePrecedent = NORMAL;
bool urgenceActive = false;


// Objets
Bouton btnUrgence(PIN_BTN_URGENCE);
Porte porte(PIN_SERVO, PIN_TRIGGER, PIN_ECHO, PIN_BTN_OUVRIR);
Convoyeur convoyeur(PIN_MOTEUR_IN1, PIN_MOTEUR_IN2, PIN_LED_MARCHE, PIN_JOY_X, PIN_JOY_Y);

U8G2_MAX7219_8X8_F_4W_SW_SPI u8g2(U8G2_R0, CLK_PIN, DIN_PIN, CS_PIN, U8X8_PIN_NONE, U8X8_PIN_NONE);
Matrice matrice(&u8g2);
LCDManager lcdManager(2000);
MqttManager mqtt(MQTT_SERVEUR, MQTT_PORT, MQTT_USER, MQTT_PASS, DEVICE_NAME, TOPIC_PUB, TOPIC_SUB);


// Setup
void setup() {
  Serial.begin(115200);

  pinMode(PIN_LED_URGENCE, OUTPUT);

  porte.init();
  convoyeur.init();
  delay(500);
  u8g2.begin();
  u8g2.setContrast(5);

  IrReceiver.begin(PIN_IR);
  lcdManager.init();

  wifiInit();
  mqtt.init(mqttEvent);

  Serial.println("Setup OK");
}

// Loop
void loop() {
  unsigned long currentTime = millis();

  // Bouton urgence
  if (btnUrgence.estAppuye()) {
    urgenceActive = !urgenceActive;
    porte.setUrgence(urgenceActive);
    convoyeur.setUrgence(urgenceActive);

    if (urgenceActive) {
      modePrecedent = cMode;
      cMode = URGENCE;
    } else {
      cMode = modePrecedent;
    }
  }
  digitalWrite(PIN_LED_URGENCE, urgenceActive ? HIGH : LOW);

  // Lecture IR (sauf  quand urgence active)
  if (!urgenceActive) {
    lireIR();
  }

  // Maj des modules
  IrReceiver.stop();
  porte.update(currentTime);
  IrReceiver.start();
  convoyeur.update(currentTime);
  matrice.draw((int)cMode);

  // LCD affichage par rotation avec les états
  bool convActif = (convoyeur.getEtat() != CONV_ARRET);
  bool convAvant = (convoyeur.getEtat() == CONV_AVANCE);
  lcdManager.update((int)porte.getEtat(), convActif, convAvant, convoyeur.getVitesse(), (int)cMode, urgenceActive);

  // MQTT
  mqtt.update(currentTime, directionMqtt(), convoyeur.getVitesse(), porteMqtt(), statutMqtt(), porte.getCompteurClients());
}

// Lecture telecommande IR
void lireIR() {
  if (!IrReceiver.decode()) return;

  uint16_t code = IrReceiver.decodedIRData.command;
  IrReceiver.resume();

  if (code == 0) return; 

  if (code == IR_TOUCHE_1) cMode = NORMAL;
  else if (code == IR_TOUCHE_2) cMode = RABAIS;
  else if (code == IR_TOUCHE_3) cMode = ERREUR;
  else if (code == IR_TOUCHE_4) cMode = FERMER;
}

// Reception serie pour la matrice
void serialEvent() {
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (urgenceActive) return;

    if (c == '1') cMode = NORMAL;
    else if (c == '2') cMode = RABAIS;
    else if (c == '3') cMode = ERREUR;
    else if (c == '4') cMode = FERMER;
  }
}

// WIFI
void wifiInit() {
  Serial1.begin(AT_BAUD_RATE);
  WiFi.init(&Serial1);

  if (WiFi.status() == WL_NO_MODULE) {
    return;
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  unsigned long debut = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - debut < 10000) {
    delay(1000);
  }
}

// MQTT - Mapping des états avec le broker
const char* directionMqtt() {
  if (convoyeur.getEtat() == CONV_AVANCE) return "droite";
  if (convoyeur.getEtat() == CONV_RECULE) return "gauche";
  return "stop";
}

const char* porteMqtt() {
  EtatPorte e = porte.getEtat();
  if (e == PORTE_OUVERTE) return "ouvert";
  if (e == PORTE_OUVERTURE || e == PORTE_FERMETURE) return "move";
  return "ferme";
}

const char* statutMqtt() {
  if (urgenceActive) return "urgence";
  if (cMode == RABAIS) return "rabais";
  if (cMode == ERREUR) return "erreur";
  return "ok";
}

// MQTT - reception des données du broker
void mqttEvent(char* topic, byte* payload, unsigned int length) {
  char buffer[100];
  unsigned int n = length < 99 ? length : 99;
  memcpy(buffer, payload, n);
  buffer[n] = '\0';

  Serial.print("MQTT recu [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(buffer);

  // Urgence depuis MQTT
  if (strstr(buffer, "urgence")) {
    if (!urgenceActive) {
      urgenceActive = true;
      porte.setUrgence(true);
      convoyeur.setUrgence(true);
      modePrecedent = cMode;
      cMode = URGENCE;
    }
    return;
  }

  // Sortie d'urgence si on recoit autre chose
  if (urgenceActive) {
    urgenceActive = false;
    porte.setUrgence(false);
    convoyeur.setUrgence(false);
  }

  if (strstr(buffer, "rabais")) cMode = RABAIS;
  else if (strstr(buffer, "erreur")) cMode = ERREUR;
  else if (strstr(buffer, "ok")) cMode = NORMAL;
}
