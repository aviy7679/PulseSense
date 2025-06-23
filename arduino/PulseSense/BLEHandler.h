#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <Arduino.h>
#include <BLEDevice.h>   //ספריית הבסיס של הבלוטוס
#include <BLEServer.h>   //יצירת השירות והמאפיינים
#include <BLEUtils.h>    //ספרייה להמרות, הגדרות וכדומה
#include <BLE2902.h>     //האפשרות לעשות NOTIFY מהבקר לאפליקציה

class BLEHandler {
  public:
    BLEHandler();
    bool begin();
    void sendData(float motion, int pulse, String status, float temperature = -999);
    void sendAlert(String message);
    bool isConnected();
    String getCommand();
    void update();  
    
  private:
    BLEServer* pServer;
    //שליחת נתונים מה-ESP32 אל האפליקציה
    BLECharacteristic* pCharacteristic;
    //קבלת פקודות מהאפליקציה ל־ESP32
    BLECharacteristic* pCommandCharacteristic;
    bool deviceConnected;
    String receivedCommand;
    
    // UUIDs
    const char* SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";
    const char* CHARACTERISTIC_UUID = "0000ffe1-0000-1000-8000-00805f9b34fb";
    const char* COMMAND_UUID = "0000ffe2-0000-1000-8000-00805f9b34fb";
};

#endif