#pragma once

#include <WiFiS3.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>

/*
  ArduinoMqttHelper - Beginner MQTT library for Arduino UNO R4 WiFi
  ----------------------------------------------------------------
  Hides WiFi and MQTT connection details so students can focus
  on the application logic.
*/

class ArduinoMqttHelper {
public:
  ArduinoMqttHelper();

  // Call in setup(): students pass WiFi credentials directly here.
  void connectWiFi(const char* ssid, const char* pass);

  // Call in setup(): students pass broker host and port directly here.
  void connectMqtt(const char* broker, int port);

  // Call every loop(): keeps connections alive, receives messages.
  void update();

  // Subscribe to a topic.
  void subscribe(const char* topic);

  // Publish a message.
  void send(const char* topic, const char* payload);

  // Register the function that handles received messages.
  void onMessage(void (*callback)(const String& topic, const String& payload));

private:
  WiFiClient _wifiClient;
  MqttClient _mqttClient;

  const char* _ssid;
  const char* _pass;
  const char* _broker;
  int _port;

  static const int MAX_TOPICS = 8;
  const char* _topics[MAX_TOPICS];
  int _topicCount;

  void (*_userCallback)(const String& topic, const String& payload);

  void _ensureWiFiConnected();
  void _ensureMqttConnected();

  static void _mqttThunk(int messageSize);
  static ArduinoMqttHelper* _instance;
};