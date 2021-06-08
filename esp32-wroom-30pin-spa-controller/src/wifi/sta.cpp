#include <Arduino.h>
#include <WiFi.h>

#include "wifi/sta.h"

WifiSTA::WifiSTA(const WifiPrefs& _wifi_prefs)
: wifi_prefs(_wifi_prefs)
{
}

void WifiSTA::setup()
{
    Serial.println("WifiSTA: put WiFi in STATION mode");
    WiFi.mode(WIFI_STA);
    connect();
}

void WifiSTA::connect() const
{
    Serial.println("WifiSTA: connect to AP with in-memory credentials");
    if (wifi_prefs.getSSID().length() == 0)
    {
        Serial.println("WifiSTA: missing SSID, canceling WiFi startup");
        return;
    }
    if (wifi_prefs.getPSK().length() == 0)
    {
        Serial.println("WifiSTA: missing PSK, canceling WiFi startup");
        return;
    }
    WiFi.begin(wifi_prefs.getSSID().c_str(), wifi_prefs.getPSK().c_str());
}

void WifiSTA::disconnect() const
{
    Serial.println("WifiSTA: disconnect from AP and erase AP information");
    WiFi.disconnect(false, true);
}

void WifiSTA::restart() const
{
    disconnect();
    connect();
}

String WifiSTA::getIp() const
{
    return WiFi.localIP().toString().c_str();
}

wl_status_t WifiSTA::getState() const
{
    return WiFi.status();
}

String WifiSTA::statusToString(const wl_status_t& status)
{
    switch(status)
    {
        case WL_NO_SHIELD:
            return "WL_NO_SHIELD";
        case WL_IDLE_STATUS:
            return "WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL:
            return "WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED:
            return "WL_SCAN_COMPLETED";
        case WL_CONNECTED:
            return "WL_CONNECTED";
        case WL_CONNECT_FAILED:
            return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST:
            return "WL_CONNECTION_LOST";
        case WL_DISCONNECTED:
            return "WL_DISCONNECTED";
        default:
            return String("UNKNOWN_") + String(status);
    }
}
