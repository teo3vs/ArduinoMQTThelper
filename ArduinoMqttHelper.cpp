#include "ArduinoMqttHelper.h"

ArduinoMqttHelper* ArduinoMqttHelper::_instance = nullptr;

ArduinoMqttHelper::ArduinoMqttHelper()
  : _mqttClient(_wifiClient),
    _ssid(nullptr), _pass(nullptr),
    _broker(nullptr), _port(1883),
    _topicCount(0), _userCallback(nullptr) {
  _instance = this;
}

void ArduinoMqttHelper::connectWiFi(const char* ssid, const char* pass) {
  _ssid = ssid;
  _pass = pass;

  _mqttClient.onMessage(_mqttThunk);
  _ensureWiFiConnected();
}

void ArduinoMqttHelper::connectMqtt(const char* broker, int port) {
  _broker = broker;
  _port = port;

  _mqttClient.onMessage(_mqttThunk);
  _ensureWiFiConnected();
  _ensureMqttConnected();
}

void ArduinoMqttHelper::update() {
  if (WiFi.status() != WL_CONNECTED) {
    _ensureWiFiConnected();
    return;
  }

  if (!_mqttClient.connected()) {
    _ensureMqttConnected();
  }

  _mqttClient.poll();
}

void ArduinoMqttHelper::subscribe(const char* topic) {
  if (_topicCount < MAX_TOPICS) {
    _topics[_topicCount++] = topic;
  }

  if (_mqttClient.connected()) {
    _mqttClient.subscribe(topic);
    Serial.print("Subscribed: ");
    Serial.println(topic);
  }
}

void ArduinoMqttHelper::send(const char* topic, const char* payload) {
  if (_mqttClient.connected()) {
    _mqttClient.beginMessage(topic);
    _mqttClient.print(payload);
    _mqttClient.endMessage();

    Serial.print("Sent -> ");
    Serial.print(topic);
    Serial.print(": ");
    Serial.println(payload);
  }
}

void ArduinoMqttHelper::onMessage(void (*callback)(const String& topic, const String& payload)) {
  _userCallback = callback;
}

void ArduinoMqttHelper::_ensureWiFiConnected() {
  if (!_ssid || !_pass) return;
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Connecting to WiFi: ");
  Serial.println(_ssid);
  WiFi.begin(_ssid, _pass);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(300);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected - IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed");
  }
}

void ArduinoMqttHelper::_ensureMqttConnected() {
  if (!_broker) return;
  if (WiFi.status() != WL_CONNECTED) return;
  if (_mqttClient.connected()) return;

  Serial.print("Connecting to MQTT broker ");
  Serial.print(_broker);
  Serial.print(":");
  Serial.println(_port);

  if (!_mqttClient.connect(_broker, _port)) {
    Serial.print("MQTT connect failed, code: ");
    Serial.println(_mqttClient.connectError());
    return;
  }

  Serial.println("MQTT connected");

  for (int i = 0; i < _topicCount; i++) {
    _mqttClient.subscribe(_topics[i]);
    Serial.print("Subscribed: ");
    Serial.println(_topics[i]);
  }
}

void ArduinoMqttHelper::_mqttThunk(int messageSize) {
  if (!_instance || !_instance->_userCallback) return;

  String topic = _instance->_mqttClient.messageTopic();
  String payload = "";
  while (_instance->_mqttClient.available()) {
    payload += (char)_instance->_mqttClient.read();
  }

  Serial.print("Received -> ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  _instance->_userCallback(topic, payload);
}