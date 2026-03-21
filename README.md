# ArduinoMqttHelper

Beginner-friendly MQTT helper for Arduino UNO R4 WiFi.

This small library wraps WiFi + MQTT connection handling so you can focus on your project logic:
- connect to WiFi
- connect to MQTT broker
- subscribe and publish topics
- handle reconnects automatically in `loop()`

## Features

- Simple API (`connectWiFi`, `connectMqtt`, `subscribe`, `send`, `update`)
- Automatic WiFi reconnect
- Automatic MQTT reconnect
- Topic re-subscription after reconnect
- Callback for incoming MQTT messages
- Works well for Home Assistant MQTT entities

## Requirements

- Board: Arduino UNO R4 WiFi
- Arduino IDE 2.x (or Arduino CLI)
- Libraries:
  - `ArduinoMqttClient` https://github.com/arduino-libraries/ArduinoMqttClient/tree/master/src
  - `WiFiS3`

## Installation

1. Copy the `ArduinoMqttHelper` folder into your Arduino libraries folder.
2. Restart Arduino IDE.
3. Open the example sketch: `arduino_sketch_mqtt_example.ino`.

Typical libraries path:
- macOS: `~/Documents/Arduino/libraries/`

## Quick Start

In your sketch:

```cpp
#include "ArduinoMqttHelper.h"

ArduinoMqttHelper mqtt;

void setup() {
  Serial.begin(9600);

  mqtt.onMessage(handleReceivedMessage);

  mqtt.connectWiFi("your-ssid", "your-password");
  mqtt.connectMqtt("your-mqtt-server-name-or-IP", 1883); //1883 is the standard MQTT port

  mqtt.subscribe("homeassistant/kitchen/light1/set");
  mqtt.send("homeassistant/kitchen", "board_started");
}

void loop() {
  mqtt.update();
  // your logic here
}

void handleReceivedMessage(const String& topic, const String& payload) {
  if (topic == "homeassistant/kitchen/light1/set") {
    // parse payload and control outputs
  }
}
```

## API

### `connectWiFi(const char* ssid, const char* pass)`
Stores credentials and connects to WiFi.

### `connectMqtt(const char* broker, int port)`
Stores broker settings and connects to MQTT.

### `update()`
Call on every loop cycle. Keeps WiFi/MQTT connected and processes incoming messages.

### `subscribe(const char* topic)`
Adds a topic to internal subscription list and subscribes immediately if connected.

### `send(const char* topic, const char* payload)`
Publishes a payload to topic (only when MQTT is connected).

### `onMessage(void (*callback)(const String&, const String&))`
Registers callback executed on each received MQTT message.

## Included Example

The example `arduino_sketch_mqtt_example.ino` shows:
- one output (`out1_pin`)
- one button input with debounce (`in1_pin`)
- toggle output from button press
- mirror state to MQTT (`.../state`)
- receive commands from MQTT (`.../set`)

Default topics used in the example:
- subscribe: `/homeassistant/kitchen/light1/set`
- publish: `/homeassistant/kitchen/light1/state`

## Home Assistant MQTT Light Example

```yaml
light:
  - platform: mqtt
    name: "Kitchen Light 1"
    command_topic: "homeassistant/kitchen/light1/set"
    state_topic: "homeassistant/kitchen/light1/state"
    payload_on: "ON"
    payload_off: "OFF"
```

## Troubleshooting

- If WiFi does not connect:
  - verify SSID/password
  - check you are using 2.4 GHz WiFi
- If MQTT does not connect:
  - verify broker IP/hostname and port
  - check broker is running and reachable
- If messages are not received:
  - make sure topic names match exactly
  - ensure `mqtt.update()` is called every loop

## Notes

- `MAX_TOPICS` is currently set to `8` in the library header.
- Message callback uses Arduino `String` for easier beginner usage.
