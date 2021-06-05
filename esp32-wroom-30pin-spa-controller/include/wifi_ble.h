#ifndef WIFI_BLE_H
#define WIFI_BLE_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <WString.h>

#define BLE_SERVICE_UUID "c70dbaef-621d-4271-acc9-ee77588d866e"
#define BLE_SSID_UUID "403eb537-5f04-433c-a8fe-7cb8abe6f92f"
#define BLE_PSK_UUID "26ef191e-57cb-4026-92b0-bde16ce8da58"

class WifiBLE : public BLEServerCallbacks, public BLECharacteristicCallbacks {

private:
  const char * name;

  BLEServer *pServer;
  BLEService *pService;
  BLECharacteristic *pCharacteristicSSID;
  BLECharacteristic *pCharacteristicPSK;
  BLEAdvertising *pAdvertising;

public:
  WifiBLE(const char * _name);
  
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
};

#endif
