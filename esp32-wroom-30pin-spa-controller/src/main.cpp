#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "wifi_ble.h"

WifiBLE wifi_ble("bubulle");

void setup() {
  Serial.begin(74880);
  wifi_ble.setup();  
}

void loop() {
  delay(100);
}


