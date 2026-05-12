#include "MqttManager.h"

MqttManager::MqttManager(const char* serveur, int port, const char* user, 
const char* password, const char* deviceName, const char* topicPub, const char* topicSub) : client(wifiClient) {
  this->serveur = serveur;
  this->port = port;
  this->user = user;
  this->password = password;
  this->deviceName = deviceName;
  this->topicPub = topicPub;
  this->topicSub = topicSub;
  lastPublish = 0;
  intervallePublish = 10000;
  lastReconnect = 0;
}

void MqttManager::init(void (*callback)(char*, byte*, unsigned int)) {
  client.setServer(serveur, port);
  client.setCallback(callback);
  client.setSocketTimeout(2);
  reconnect();
}

bool MqttManager::reconnect() {
  if (client.connected()) return true;
  if (WiFi.status() != WL_CONNECTED) return false;

  bool ok = client.connect(deviceName, user, password);
  if (!ok) {
    Serial.print("Echec MQTT, code : ");
    Serial.println(client.state());
    return false;
  }

  Serial.println("Connecte au serveur MQTT");
  client.subscribe(topicSub, 0);
  return true;
}

void MqttManager::publier(const char* direction, int vitesse, const char* porte, const char* statut, int clients) {
  static char message[150];

  sprintf(message, "{\"direction\":\"%s\",\"vitesse\":%d,\"porte\":\"%s\",\"statut\":\"%s\",\"clients\":%d}", direction, vitesse, porte, statut, clients);

  Serial.print("Publish : ");
  Serial.println(message);

  if (!client.publish(topicPub, message)) {
    Serial.println("Echec publication");
  }
}

void MqttManager::update(unsigned long currentTime, const char* direction, int vitesse, const char* porte, const char* statut, int clients) {
  // Reconnexion espacee de 5 sec pour ne pas bloquer le loop
  if (!client.connected()) {
    if (currentTime - lastReconnect >= 5000) {
      lastReconnect = currentTime;
      reconnect();
    }
    return;
  }
  client.loop();

  if (currentTime - lastPublish >= intervallePublish) {
    lastPublish = currentTime;
    publier(direction, vitesse, porte, statut, clients);
  }
}
