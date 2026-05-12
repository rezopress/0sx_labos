#pragma once
#include <Arduino.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>

class MqttManager {
  private:
    WiFiClient wifiClient;
    PubSubClient client;

    const char* serveur;
    int port;
    const char* user;
    const char* password;
    const char* deviceName;
    const char* topicPub;
    const char* topicSub;

    unsigned long lastPublish;
    unsigned long intervallePublish;
    unsigned long lastReconnect;

  public:
    MqttManager(const char* serveur, int port, const char* user, const char* password, const char* deviceName, const char* topicPub, const char* topicSub);
    void init(void (*callback)(char*, byte*, unsigned int));
    bool reconnect();
    void publier(const char* direction, int vitesse, const char* porte, const char* statut, int clients);
    void update(unsigned long currentTime, const char* direction, int vitesse, const char* porte, const char* statut, int clients);
};
