#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "wifi/prefs.h"
#include "wifi/sta.h"
#include "wifi/ble.h"

#define HOSTNAME "bubulle"

WifiPrefs wifi_prefs;

WifiSTA wifi_sta(wifi_prefs);

WifiBLE wifi_ble(HOSTNAME, wifi_prefs, wifi_sta);

int last_change = 0, last_wifi_display = 0, current;

void setup() {
  Serial.begin(115200);
  Serial.println("Built on " + String(__DATE__) + " at " + String(__TIME__));

  wifi_prefs.load();
  Serial.println("Wifi credentials loaded from flash");

  wifi_sta.setup();
  Serial.println("Wifi started");

  wifi_ble.setup();  
  Serial.println("Bluetooth Low Energy interface for Wifi credentials management started");
}

wl_status_t last_wifi_status = WL_NO_SHIELD, current_wifi_status;

void display_wifi_status()
{
  wl_status_t current_status = wifi_sta.getState();
  int max_interval = (current_status == WL_CONNECTED) ? 10000 : 1000;
  current = millis();
  if (current - last_wifi_display < max_interval)
    return;
  last_wifi_display = current;
  Serial.print(wifi_sta.getIp());
  Serial.print(" ");
  Serial.println(wifi_sta.statusToString(wifi_sta.getState()));
}

void loop() {
  delay(100);
  display_wifi_status();
}
