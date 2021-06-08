#ifndef WIFI_STA_H
#define WIFI_STA_H

#include "wifi_prefs.h"

class WifiSTA {

private:
    const WifiPrefs& wifi_prefs;
    void connect() const;
    void disconnect() const;

public:
    WifiSTA(const WifiPrefs& _wifi_prefs);

    void setup();
    void restart() const;

    String getIp() const;
    wl_status_t getState() const;
    static String statusToString(const wl_status_t& status);
};

#endif
