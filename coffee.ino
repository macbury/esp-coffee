#include <EEPROM.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "credentials.h"

const byte TICK_DELAY = 33;
const int JSON_BUFFER_SIZE = JSON_OBJECT_SIZE(10);

WiFiClient espClient;
PubSubClient client(espClient);

#include "connection.h"

void on_mqtt_message(char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.println(topic);
  /**
   * Transform this to more normal stuff like array of char not a bytefuck
   */
  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);

  if (processJson(message)) {
    sendCurrentState();
    return;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(on_mqtt_message);
}


void loop() {
  if (client.connected()) {
    client.loop();
    ArduinoOTA.handle();
  } else {
    if (ensureMqttConnection()) {
      onConnect();
    }
  }

  delay(TICK_DELAY);
}
