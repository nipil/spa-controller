
#include "wifi/mgr.h"

WifiManager::WifiManager(const char * hostname)
: wifi_sta(wifi_prefs), wifi_ble(hostname, wifi_prefs, wifi_sta)
{
    last_print = 0;
    last_state = WL_NO_SHIELD;
}

void WifiManager::setup()
{
    wifi_prefs.load();
    wifi_sta.setup();
    wifi_ble.setup();
}

wl_status_t WifiManager::getState() const
{
    return wifi_sta.getState();
}

String WifiManager::getIp() const
{
    return wifi_sta.getIp();
}

void WifiManager::printStatusToSerial(unsigned long interval_ms, int connected_interval_mutliplier)
{
    // get current state
    wl_status_t current_state = wifi_sta.getState();
    // base interval
    int max_interval = interval_ms;
    // expand interval if no change and connected
    if (current_state == last_state && current_state == WL_CONNECTED)
        max_interval *= connected_interval_mutliplier;
    // manage time since last print
    unsigned long current = millis();
    if (current - last_print < max_interval)
        return;
    // update
    last_state = current_state;
    last_print = current;
    Serial.print("WifiManager: state=");
    Serial.print(wifi_sta.statusToString(wifi_sta.getState()));
    Serial.print(" ip=");
    Serial.println(wifi_sta.getIp());
}
