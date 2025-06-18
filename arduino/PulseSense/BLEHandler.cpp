#include "BLEHandler.h"

// Global pointers לגישה מה-callbacks
BLEHandler* globalBLEHandler = nullptr;

// Simple callbacks
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      if (globalBLEHandler) {
        Serial.println("📱 אפליקציה התחברה!");
      }
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("📱 אפליקציה התנתקה");
      delay(500);
      pServer->startAdvertising();
      Serial.println("⏳ מחכה לחיבור חדש...");
    }
};

class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      if (globalBLEHandler) {
        String value = pCharacteristic->getValue().c_str();
        if (value.length() > 0) {
          Serial.println("📱 פקודה התקבלה: " + value);
          // כאן נשמור את הפקודה ב-globalBLEHandler
        }
      }
    }
};

BLEHandler::BLEHandler() {
  deviceConnected = false;
  receivedCommand = "";
  globalBLEHandler = this;  // הגדרת הpointer הגלובלי
}

bool BLEHandler::begin() {
  Serial.println("🔄 מאתחל BLE...");
  
  // אתחול BLE
  BLEDevice::init("PulseSense-ESP32");
  
  // יצירת Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  
  // יצירת Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // יצירת Characteristic לשליחת נתונים
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  // יצירת Characteristic לקבלת פקודות
  pCommandCharacteristic = pService->createCharacteristic(
                             COMMAND_UUID,
                             BLECharacteristic::PROPERTY_WRITE
                           );
  
  pCommandCharacteristic->setCallbacks(new CharacteristicCallbacks());
  
  // הוספת Descriptor לNotifications
  pCharacteristic->addDescriptor(new BLE2902());
  
  // התחלת השירות
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  
  Serial.println("🔵 BLE מוכן!");
  Serial.println("📱 שם המכשיר: PulseSense-ESP32");
  Serial.println("⏳ מחכה לחיבור מהאפליקציה...");
  
  return true;
}

void BLEHandler::update() {
  // בדיקה פשוטה של מצב החיבור
  deviceConnected = (pServer->getConnectedCount() > 0);
}

void BLEHandler::sendData(float motion, int pulse, String status, float temperature) {
  if (!isConnected()) return;
  
  // יצירת JSON עם טמפרטורה
  String message = "{";
  message += "\"motion\":" + String(motion, 1) + ",";
  message += "\"pulse\":" + String(pulse) + ",";
  message += "\"status\":\"" + status + "\",";
  
  if (temperature != -999) {
    message += "\"temperature\":" + String(temperature, 1) + ",";
    message += "\"temp_status\":\"" + String(temperature >= 35.0 && temperature <= 42.0 ? "גוף" : "סביבה") + "\",";  
  }
  
  message += "\"time\":" + String(millis());
  message += "}";
  
  pCharacteristic->setValue(message.c_str());
  pCharacteristic->notify();
  
  Serial.println("📤 נשלח: " + message);
}

void BLEHandler::sendAlert(String message) {
  if (!isConnected()) return;
  
  String alert = "{";
  alert += "\"type\":\"alert\",";
  alert += "\"message\":\"" + message + "\",";
  alert += "\"time\":" + String(millis());
  alert += "}";
  
  pCharacteristic->setValue(alert.c_str());
  pCharacteristic->notify();
  
  Serial.println("🚨 התראה נשלחה: " + message);
}

bool BLEHandler::isConnected() {
  update();  // עדכון מצב החיבור
  return deviceConnected;
}

String BLEHandler::getCommand() {
  // קריאה פשוטה של הפקודה האחרונה
  if (pCommandCharacteristic && isConnected()) {
    String value = pCommandCharacteristic->getValue().c_str();
    if (value.length() > 0 && value != receivedCommand) {
      receivedCommand = value;
      Serial.println("📱 פקודה מהאפליקציה: " + value);
      return value;
    }
  }
  return "";
}