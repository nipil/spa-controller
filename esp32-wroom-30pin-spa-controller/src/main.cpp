#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "wifi/mgr.h"

#define HOSTNAME "bubulle"

WifiManager wifi_manager(HOSTNAME);

void setup() {
  Serial.begin(115200);
  Serial.println("Built on " + String(__DATE__) + " at " + String(__TIME__));

  wifi_manager.setup();
  Serial.println("Wifi started using stored credentials, with BLE configurator interface");
}

void loop() {
  delay(100);
  wifi_manager.printStatusToSerial(1000, 60);
}
