#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "wifi_prefs.h"
#include "wifi_ble.h"

WifiPrefs wifi_prefs;
WifiBLE wifi_ble("bubulle", wifi_prefs);

void setup() {
  Serial.begin(115200);
  Serial.println("Built on " + String(__DATE__) + " at " + String(__TIME__));

  wifi_prefs.load();
  Serial.println("Wifi credentials loaded from flash");

  wifi_ble.setup();  
  Serial.println("Bluetooth Low Energy interface for Wifi credentials management started");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_prefs.getSSID().c_str(), wifi_prefs.getPSK().c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(WiFi.status());
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(100);
}
