#ifndef WIFI_PREFS_H
#define WIFI_PREFS_H

#include <WString.h>

#define WIFI_PREFS_NAMESPACE "wifi"

#define WIFI_PREFS_SSID_NAME "ssid"
#define WIFI_PREFS_SSID_DEFAULT ""

#define WIFI_PREFS_PSK_NAME "psk"
#define WIFI_PREFS_PSK_DEFAULT ""

class WifiPrefs {

private:
    String ssid;
    String psk;

public:
    WifiPrefs();

    String getSSID() const;
    void setSSID(const String& _ssid);

    String getPSK() const;
    void setPSK(const String& _psk);

    void load();
    void save() const;
    void erase();
};

#endif