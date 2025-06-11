#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLEHandler {
  public:
    BLEHandler();
    bool begin();
    void sendData(float motion, int pulse, String status, float temperature = -999);
    void sendAlert(String message);
    bool isConnected();
    String getCommand();
    void update();  // חדש - לבדיקת מצב חיבור
    
  private:
    BLEServer* pServer;
    BLECharacteristic* pCharacteristic;
    BLECharacteristic* pCommandCharacteristic;
    bool deviceConnected;
    String receivedCommand;
    
    // UUIDs
    const char* SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";
    const char* CHARACTERISTIC_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
    const char* COMMAND_UUID = "0000ffe2-0000-1000-8000-00805f9b34fb";
};

#endif