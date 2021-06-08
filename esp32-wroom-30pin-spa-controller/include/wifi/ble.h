#ifndef WIFI_BLE_H
#define WIFI_BLE_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <WString.h>

#include "wifi/prefs.h"
#include "wifi/sta.h"

#define BLE_SERVICE_UUID "c70dbaef-621d-4271-acc9-ee77588d866e"
#define BLE_SSID_UUID "403eb537-5f04-433c-a8fe-7cb8abe6f92f"
#define BLE_PSK_UUID "26ef191e-57cb-4026-92b0-bde16ce8da58"
#define BLE_IP_UUID "c473b94e-ca95-4a09-9c0c-ffb08a3476c0"

#define BLE_ACTION_LOAD "load"
#define BLE_ACTION_SAVE "save"
#define BLE_ACTION_ERASE "erase"
#define BLE_ACTION_RECONNECT "reconnect"

class WifiBLE : public BLEServerCallbacks, public BLECharacteristicCallbacks {

private:
  const char * name;

  WifiPrefs& wifi_prefs;
  const WifiSTA& wifi_sta;

  BLEServer *pServer;
  BLEService *pService;
  BLEAdvertising *pAdvertising;

  BLECharacteristic *pCharacteristicSSID;
  BLECharacteristic *pCharacteristicPSK;
  BLECharacteristic *pCharacteristicAction;

public:
  WifiBLE(const char * _name, WifiPrefs& _wifi_prefs, const WifiSTA& _wifi_sta);
  
  void setup();
  void start_advertising();

  // BLEServerCallbacks
	virtual void onConnect(BLEServer* pServer);
	virtual void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param);
	virtual void onDisconnect(BLEServer* pServer);

  // BLECharacteristicCallbacks
	virtual void onRead(BLECharacteristic* pCharacteristic);
	virtual void onWrite(BLECharacteristic* pCharacteristic);
	virtual void onNotify(BLECharacteristic* pCharacteristic);
	virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code);

  static const String statusToString(const Status& s);

private:
  void setCharacteristic(const char * name, BLECharacteristic *pCharacteristic, const String& new_value);
  void printEventCharacteristicDebug(const char * event, BLECharacteristic *pCharacteristic);
};

#endif
