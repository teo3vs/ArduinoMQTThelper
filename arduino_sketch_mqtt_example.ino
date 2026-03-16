#include "ArduinoMqttHelper.h"

// Pins definitions
const int out1_pin = 3;
const int in1_pin  = 2;  // Button to GND, INPUT_PULLUP

// Output variables
bool out1 = false, prev_out1 = false;

//Input variables
bool in1 = false, prev_in1 = false;

// Debounce variables for input
bool in1_raw, prev_in1_raw;
unsigned long in1_DebounceTime  = 0;
const unsigned long debounceDelay = 50;

ArduinoMqttHelper mqtt; // Create an instance of the MQTT helper class.

void setup() {
  Serial.begin(9600);
  delay(1500);

  pinMode(out1_pin, OUTPUT);
  pinMode(in1_pin, INPUT_PULLUP);

  // Register the function that handles incoming messages.
  mqtt.onMessage(handleReceivedMessage);

  // Pass the connection data here.
  mqtt.connectWiFi("myWiFiSSID", "myWiFiPassword"); // EDIT HERE
  mqtt.connectMqtt("myMQTTBroker", 1883); // EDIT HERE (1883 is the default MQTT port)

  // Subscribe to the topics you want to receive.
  mqtt.subscribe("/homeassistant/kitchen/light1/set");

  // Announce the board is alive.
  mqtt.send("/homeassistant/kitchen", "board_started");
}

void loop() {
  
  mqtt.update();
  readInputs();

  // Toggle light state variable directly when the button is pressed.
  if (!prev_in1 && in1) {
    out1 = !out1;
  }

  // Publish output state to MQTT broker only when it changes.
  if (out1 != prev_out1) {
    mqtt.send("/homeassistant/kitchen/light1/state", out1 ? "ON" : "OFF");
  }

  writeOutputs();
  recordPrevStates();
  delay(10);
}

void handleReceivedMessage(const String& topic, const String& payload) {
  // If a message arrives on the topic "/homeassistant/kitchen/light1/set",
  // set the out1 state variable accordingly to the payload.
  if (topic == "/homeassistant/kitchen/light1/set") {
    if (payload == "ON") {
      out1 = true;
    } else if (payload == "OFF") {
      out1 = false;
    }
  }
}

void readInputs() {
  // read in1 with debounce
  in1_raw = digitalRead(in1_pin);
  if (in1_raw != prev_in1_raw) in1_DebounceTime = millis();
  if ((millis() - in1_DebounceTime) > debounceDelay) in1 = in1_raw;
  prev_in1_raw = in1_raw;
}

void writeOutputs() {
  digitalWrite(out1_pin, out1);
}

void recordPrevStates() {
  prev_in1 = in1;
  prev_out1 = out1;
}