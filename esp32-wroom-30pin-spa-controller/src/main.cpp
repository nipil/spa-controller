#include <Arduino.h>

// custom defines for faster dev
#define NO_WIFI
// #define DO_SIPO_TEST
#define DO_COMMS_TEST

#define HOSTNAME "bubulle"
#define IDLE_MESSAGE_INTERVAL_MS 10000

#ifndef NO_WIFI
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>

#include "wifi/mgr.h"

WifiManager wifi_manager(HOSTNAME);
#endif

#ifdef DO_SIPO_TEST
#include "sipo/sn74hc595n.h"

SN74HC595N sipo (
  16,  // pin_ser
  17,  // pin_srclk
  18,  // pin_rclk
  26,  // pin_srclr
  25   // pin_oe
);
#endif

#ifdef DO_COMMS_TEST
#include "comms/panel.h"

CommsPanel panel (
  27, // pin_input_clock
  5, // pin_input_spa
  4 // pin_input_dsp
);

void IRAM_ATTR input_clock_pin_change_interrupt()
{
  panel.input_clock_pin_changed();
}
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("MAIN: Built on " + String(__DATE__) + " at " + String(__TIME__));

#ifndef NO_WIFI
  wifi_manager.setup();
  Serial.println("MAIN: Wifi started using stored credentials, with BLE configurator interface");
#endif

#ifdef DO_SIPO_TEST
  sipo.setup();
  Serial.println("MAIN: SN74HC595N serial-in-parallel-out extender initialized");
#endif

  panel.setup(input_clock_pin_change_interrupt);
  Serial.println("MAIN: Balboa GS501D/VL801D communication handler initialized");
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

#ifdef DO_COMMS_TEST
  panel.interval_loop();
#endif

  static ulong last_idle_message_ms = 0;
  ulong current_time_ms = millis();
  if (current_time_ms - last_idle_message_ms > IDLE_MESSAGE_INTERVAL_MS)
  {
      last_idle_message_ms = current_time_ms;
      Serial.printf("Main: idle (%lu ms)", current_time_ms);
      Serial.println();
  }
}
