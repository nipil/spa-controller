#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "wifi_prefs.h"
#include "wifi_ble.h"

WifiPrefs wifi_prefs;
WifiBLE wifi_ble("bubulle", wifi_prefs);

void setup() {
  Serial.begin(115200);
  Serial.println("Built on " + String(__DATE__) + " at " + String(__TIME__));
  wifi_prefs.load();
  wifi_ble.setup();  
}

void loop() {
  delay(100);
}
