#include <Arduino.h>
#include <Preferences.h>

#include "wifi/prefs.h"

WifiPrefs::WifiPrefs()
: ssid(WIFI_PREFS_SSID_DEFAULT), psk(WIFI_PREFS_PSK_DEFAULT)
{
}

String WifiPrefs::getSSID() const
{
    return ssid;
}

void WifiPrefs::setSSID(const String& _ssid)
{
    Serial.println("WifiPrefs: Setting " + String(WIFI_PREFS_NAMESPACE) + "::" + String(WIFI_PREFS_SSID_NAME) + " to " + _ssid);
    ssid = _ssid;
}

String WifiPrefs::getPSK() const
{
    return psk;
}

void WifiPrefs::setPSK(const String& _psk)
{
    Serial.println("WifiPrefs: Setting " + String(WIFI_PREFS_NAMESPACE) + "::" + String(WIFI_PREFS_PSK_NAME) + " to " + _psk);
    psk = _psk;
}

void WifiPrefs::load()
{
    Serial.println("WifiPrefs: Loading preferences from flash to memory " + String(WIFI_PREFS_NAMESPACE));
    Preferences preferences;
    preferences.begin(WIFI_PREFS_NAMESPACE,true);
    ssid = preferences.getString(WIFI_PREFS_SSID_NAME, WIFI_PREFS_SSID_DEFAULT);
    psk = preferences.getString(WIFI_PREFS_PSK_NAME, WIFI_PREFS_PSK_DEFAULT);
    preferences.end();
}

void WifiPrefs::save() const
{
    Serial.println("WifiPrefs: Saving preferences from memory to flash " + String(WIFI_PREFS_NAMESPACE));
    Preferences preferences;
    preferences.begin(WIFI_PREFS_NAMESPACE,false);
    preferences.clear();
    preferences.putString(WIFI_PREFS_SSID_NAME, ssid);
    preferences.putString(WIFI_PREFS_PSK_NAME, psk);
    preferences.end();
}

void WifiPrefs::erase()
{
    Serial.println("WifiPrefs: Erasing preferences " + String(WIFI_PREFS_NAMESPACE));
    Preferences preferences;
    preferences.begin(WIFI_PREFS_NAMESPACE,false);
    preferences.clear();
    preferences.end();
}
