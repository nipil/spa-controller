#include <Arduino.h>
#include <WiFi.h>

#include "wifi_ble.h"

WifiBLE::WifiBLE(const char *_name, WifiPrefs& _wifi_prefs)
: name(_name), wifi_prefs(_wifi_prefs),
  pServer(NULL), pService(NULL), pAdvertising(NULL),
  pCharacteristicSSID(NULL), pCharacteristicPSK(NULL), pCharacteristicAction(NULL)
{
}

void WifiBLE::setup()
{
    BLEDevice::init(name);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    pService = pServer->createService(BLE_SERVICE_UUID);

    pCharacteristicSSID = pService->createCharacteristic(BLE_SSID_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristicSSID->setValue(wifi_prefs.getSSID().c_str());
    pCharacteristicSSID->setCallbacks(this);

    pCharacteristicPSK = pService->createCharacteristic(BLE_PSK_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristicPSK->setValue(wifi_prefs.getPSK().c_str());
    pCharacteristicPSK->setCallbacks(this);

    pCharacteristicAction = pService->createCharacteristic(BLE_IP_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    pCharacteristicAction->setValue("");
    pCharacteristicAction->setCallbacks(this);

    pService->start();

    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    start_advertising();
}

void WifiBLE::start_advertising()
{
    Serial.println("WifiBLE: start advertising");
    BLEDevice::startAdvertising();
}

// BLEServerCallbacks
void WifiBLE::onConnect(BLEServer *pServer)
{
    Serial.println("WifiBLE: onConnect server");
    start_advertising(); // connection disables advertising, so reenable it
}

void WifiBLE::onConnect(BLEServer *pServer, esp_ble_gatts_cb_param_t *param)
{
    Serial.println("WifiBLE: onConnect server param");
    Serial.println(param->connect.conn_id);
    for (int i = 0; i < sizeof(param->connect.remote_bda); i++)
    {
        if (i != 0)
            Serial.print(":");
        Serial.print(String(param->connect.remote_bda[i], 16));
    }
    Serial.println();
    start_advertising(); // connection disables advertising, so reenable it
}

void WifiBLE::onDisconnect(BLEServer *pServer)
{
    Serial.println("WifiBLE: onDisconnect server");
}

// BLECharacteristicCallbacks
void WifiBLE::onRead(BLECharacteristic *pCharacteristic)
{
    Serial.println("WifiBLE: onRead charac ");
    Serial.println(pCharacteristic->getUUID().toString().c_str());
    Serial.println(pCharacteristic->getValue().c_str());
    if (pCharacteristic == pCharacteristicSSID)
    {
        Serial.println("SSID");
        pCharacteristic->setValue(wifi_prefs.getSSID().c_str());
    }
    else if (pCharacteristic == pCharacteristicPSK)
    {
        Serial.println("PSK");
        pCharacteristic->setValue(wifi_prefs.getPSK().c_str());
    }
    else if (pCharacteristic == pCharacteristicAction)
    {
        Serial.println("Action");

        switch (WiFi.status())
        {
            case WL_NO_SHIELD:
                pCharacteristic->setValue("WL_NO_SHIELD");
                break;
            case WL_IDLE_STATUS:
                pCharacteristic->setValue("WL_IDLE_STATUS");
                break;
            case WL_NO_SSID_AVAIL:
                pCharacteristic->setValue("WL_NO_SSID_AVAIL");
                break;
            case WL_SCAN_COMPLETED:
                pCharacteristic->setValue("WL_SCAN_COMPLETED");
                break;
            case WL_CONNECTED:
                pCharacteristic->setValue(WiFi.localIP().toString().c_str());
                break;
            case WL_CONNECT_FAILED:
                pCharacteristic->setValue("WL_CONNECT_FAILED");
                break;
            case WL_CONNECTION_LOST:
                pCharacteristic->setValue("WL_CONNECTION_LOST");
                break;
            case WL_DISCONNECTED:
                pCharacteristic->setValue("WL_DISCONNECTED");
                break;
            default:
                String msg("WARNING: unknown characteristic, do nothing");
                Serial.println(msg);
                pCharacteristic->setValue(msg.c_str());
                break;
        }
    }
    else
    {
        Serial.println("WARNING: unknown characteristic, do nothing");
    }
}

void WifiBLE::onWrite(BLECharacteristic *pCharacteristic)
{
    Serial.println("WifiBLE: onWrite charac");
    Serial.println(pCharacteristic->getUUID().toString().c_str());
    Serial.println(pCharacteristic->getValue().c_str());
    if (pCharacteristic == pCharacteristicSSID)
    {
        Serial.println("SSID");
        wifi_prefs.setSSID(pCharacteristic->getValue().c_str());
    }
    else if (pCharacteristic == pCharacteristicPSK)
    {
        Serial.println("PSK");
        wifi_prefs.setPSK(pCharacteristic->getValue().c_str());
    }
    else if (pCharacteristic == pCharacteristicAction)
    {
        Serial.println("Action");
        if (pCharacteristic->getValue() == BLE_ACTION_LOAD)
        {
            wifi_prefs.load();
        }
        else if (pCharacteristic->getValue() == BLE_ACTION_SAVE)
        {
            wifi_prefs.save();
        }
        else if (pCharacteristic->getValue() == BLE_ACTION_ERASE)
        {
            wifi_prefs.erase();
        }
        else if (pCharacteristic->getValue() == BLE_ACTION_RECONNECT)
        {
            Serial.println("WifiBLE: disconnect from AP and erase AP information");
            WiFi.disconnect(false, true);
            Serial.println("WifiBLE: connect to AP with stored information");
            WiFi.begin(wifi_prefs.getSSID().c_str(), wifi_prefs.getPSK().c_str());
        }
        else
        {
            Serial.println("WARNING: unknown action, do nothing");
        }
    }
    else
    {
        Serial.println("WARNING: unknown characteristic, do nothing");
    }
}

void WifiBLE::onNotify(BLECharacteristic *pCharacteristic)
{
    Serial.println("WifiBLE: onNotify charac");
    Serial.println(pCharacteristic->getUUID().toString().c_str());
    Serial.println(pCharacteristic->getValue().c_str());
}

void WifiBLE::onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code)
{
    Serial.println("WifiBLE: onStatus status code");
    Serial.println(pCharacteristic->getUUID().toString().c_str());
    Serial.println(pCharacteristic->getValue().c_str());
    Serial.println(statusToString(s));
    Serial.println(code);
}

const String WifiBLE::statusToString(const Status &s)
{
    switch (s)
    {
    case SUCCESS_INDICATE:
        return "SUCCESS_INDICATE";
    case SUCCESS_NOTIFY:
        return "SUCCESS_NOTIFY";
    case ERROR_INDICATE_DISABLED:
        return "ERROR_INDICATE_DISABLED";
    case ERROR_NOTIFY_DISABLED:
        return "ERROR_NOTIFY_DISABLED";
    case ERROR_GATT:
        return "ERROR_GATT";
    case ERROR_NO_CLIENT:
        return "ERROR_NO_CLIENT";
    case ERROR_INDICATE_TIMEOUT:
        return "ERROR_INDICATE_TIMEOUT";
    case ERROR_INDICATE_FAILURE:
        return "ERROR_INDICATE_FAILURE";
    default:
        return "UNKNOWN";
    }
}
