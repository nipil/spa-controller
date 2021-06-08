#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <WiFi.h>

#include "wifi/prefs.h"
#include "wifi/sta.h"
#include "wifi/ble.h"

class WifiManager
{

private:
    WifiPrefs wifi_prefs;
    WifiSTA wifi_sta;
    WifiBLE wifi_ble;

    unsigned long last_print;
    wl_status_t last_state;

public:
    WifiManager(const char * hostname);
    void setup();

    wl_status_t getState() const;
    String getIp() const;

    void printStatusToSerial(unsigned long interval_ms, int connected_interval_mutliplier = 10);
};

#endif
