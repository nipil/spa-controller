#include <Arduino.h>

// custom defines for faster dev
//#define NO_WIFI
//#define DO_SIPO_TEST

#ifndef NO_WIFI
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "wifi/mgr.h"
#endif

#include "sipo/sn74hc595n.h"

#define HOSTNAME "bubulle"

#ifndef NO_WIFI
WifiManager wifi_manager(HOSTNAME);
#endif

SN74HC595N sipo (
  16,  // pin_ser
  17,  // pin_srclk
  18,  // pin_rclk
  26,  // pin_srclr
  25   // pin_oe
);

void setup() {
  Serial.begin(115200);
  Serial.println("MAIN: Built on " + String(__DATE__) + " at " + String(__TIME__));

#ifndef NO_WIFI
  wifi_manager.setup();
  Serial.println("MAIN: Wifi started using stored credentials, with BLE configurator interface");
#endif

  sipo.setup();
  Serial.println("MAIN: SN74HC595N serial-in-parallel-out extender initialized");
}

#ifdef DO_SIPO_TEST
SN74HC595N_TestLooper sipo_test(sipo, 3000);
#endif

void loop() {
  delay(100);

#ifndef NO_WIFI
  wifi_manager.printStatusToSerial(1000, 60);
#endif

#ifdef DO_SIPO_TEST
  sipo_test.interval_loop();
#endif
}
