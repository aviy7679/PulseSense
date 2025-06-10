#include "MPU6050Sensor.h"
#include "BuzzerESP.h"
#include "MAX30102Sensor.h"
#include "BLEHandler.h"

// הגדרות פינים
#define BUZZER_PIN 12

// יצירת אובייקטים
BuzzerESP buzzer(BUZZER_PIN);
MPU6050Sensor motionSensor;
MAX30102Sensor pulseSensor;
BLEHandler bluetooth;

// משתנים למצב הפעולה
enum OperationMode {
  MOTION_DETECTION,
  PULSE_MEASUREMENT,
  COMBINED_MODE
};

OperationMode currentMode = COMBINED_MODE;
unsigned long lastPrint = 0;
bool systemInitialized = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("🔥 PulseSense - מערכת מוניטורינג מתקדמת");
  Serial.println("==========================================");
  
  // אתחול רכיבים
  initializeSystem();
  
  // צפצוף התחלה
  startupSequence();
  
  Serial.println("📋 פקודות:");
  Serial.println("   1 - מצב זיהוי תנועה");
  Serial.println("   2 - מצב מדידת דופק");
  Serial.println("   3 - מצב משולב");
  Serial.println("   + - הגבר עוצמת LED");
  Serial.println("   - - הקטן עוצמת LED");
  Serial.println("   r - איפוס חיישן דופק");
  Serial.println();
  
  systemInitialized = true;
  lastPrint = millis();
}

void loop() {
  // עדכון BLE status
  bluetooth.update();
  
  // בדיקת פקודות מהמשתמש (Serial ו-Bluetooth)
  handleUserCommands();
  handleBluetoothCommands();
  
  // ביצוע פעולות לפי המצב הנוכחי
  switch (currentMode) {
    case MOTION_DETECTION:
      handleMotionDetection();
      break;
      
    case PULSE_MEASUREMENT:
      handlePulseMeasurement();
      break;
      
    case COMBINED_MODE:
      handleCombinedMode();
      break;
  }
  
  delay(200); // 5 מדידות לשנייה
}

void initializeSystem() {
  Serial.println("🔧 מאתחל מערכת...");
  
  // אתחול באזר
  buzzer.begin();
  Serial.println("   ✅ באזר מוכן");
  
  // אתחול Bluetooth
  if (bluetooth.begin()) {
    Serial.println("   ✅ Bluetooth מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול Bluetooth");
  }
  
  // אתחול חיישן תנועה
  if (motionSensor.begin()) {
    Serial.println("   ✅ חיישן תנועה MPU6050 מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול חיישן תנועה");
  }
  
  // אתחול חיישן דופק
  if (pulseSensor.begin()) {
    Serial.println("   ✅ חיישן דופק MAX30102 מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול חיישן דופק");
  }
  
  bluetooth.sendAlert("מערכת אותחלה בהצלחה!");
  Serial.println("✅ אתחול הושלם!");
}

void startupSequence() {
  Serial.println("🎵 רצף התחלה...");
  for (int i = 0; i < 3; i++) {
    buzzer.beep(200, 1500);
    delay(100);
  }
  buzzer.beep(500, 2000);
}

void handleUserCommands() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case '1':
        currentMode = MOTION_DETECTION;
        Serial.println("🏃 מצב: זיהוי תנועה בלבד");
        break;
        
      case '2':
        currentMode = PULSE_MEASUREMENT;
        Serial.println("💓 מצב: מדידת דופק בלבד");
        break;
        
      case '3':
        currentMode = COMBINED_MODE;
        Serial.println("🔄 מצב: משולב");
        break;
        
      case '+':
        pulseSensor.increasePower();
        Serial.print("💡 עוצמת LED: 0x");
        Serial.println(pulseSensor.getCurrentPower(), HEX);
        break;
        
      case '-':
        pulseSensor.decreasePower();
        Serial.print("💡 עוצמת LED: 0x");
        Serial.println(pulseSensor.getCurrentPower(), HEX);
        break;
        
      case 'r':
        Serial.println("🔄 מאפס חיישן דופק...");
        pulseSensor.resetSensor();
        Serial.println("✅ איפוס הושלם");
        break;
    }
  }
}

void handleMotionDetection() {
  motionSensor.update();
  
  // בדיקת נפילה
  if (motionSensor.detectFall(12.0)) {
    Serial.println("🚨 זוהתה נפילה!");
    buzzer.beep(1000, 3000); // אזעקה חזקה
  }
  
  // בדיקת חוסר תנועה
  if (motionSensor.detectNoMotion(5000)) {
    Serial.println("😴 אין תנועה למשך 5 שניות");
  }
  
  // הצגת נתוני תנועה
  if (millis() - lastPrint > 2000) {
    float motion = motionSensor.getMotionMagnitude();
    Serial.print("🏃 עוצמת תנועה: ");
    Serial.println(motion, 2);
    lastPrint = millis();
  }
}

void handlePulseMeasurement() {
  uint32_t red, ir;
  
  if (pulseSensor.readData(&red, &ir)) {
    // בדיקת יציבות נתונים
    if (!pulseSensor.isDataStable()) {
      Serial.println("⚠️ נתונים לא יציבים - עושה איפוס...");
      pulseSensor.resetSensor();
      return;
    }
    
    // הצגת נתונים
    Serial.print("💓 IR: ");
    Serial.print(ir);
    Serial.print(" | Red: ");
    Serial.print(red);
    
    // זיהוי מגע
    String touchStatus = pulseSensor.getTouchStatus(ir);
    Serial.print(" | ");
    Serial.print(touchStatus);
    
    // חישוב דופק
    int bpm = pulseSensor.calculateBPM(ir);
    if (bpm > 0) {
      Serial.print(" | 💓 ");
      Serial.print(bpm);
      Serial.print(" BPM");
      
      // צפצוף עם הדופק
      buzzer.beep(50, 2000);
    }
    
    Serial.println();
    
  } else {
    Serial.println("❌ שגיאה בקריאת חיישן דופק");
  }
}

void handleCombinedMode() {
  // עדכון חיישן תנועה
  motionSensor.update();
  
  // בדיקות חירום (תנועה)
  if (motionSensor.detectFall(12.0)) {
    Serial.println("🚨 זוהתה נפילה!");
    bluetooth.sendAlert("זוהתה נפילה!");
    buzzer.beep(1000, 3000);
    return; // דחיפות עליונה לנפילה
  }
  
  // מדידת דופק
  uint32_t red, ir;
  if (pulseSensor.readData(&red, &ir)) {
    
    // שליחת נתונים לאפליקציה כל שנייה
    static unsigned long lastBluetoothSend = 0;
    if (millis() - lastBluetoothSend > 1000) {
      float motion = motionSensor.getMotionMagnitude();
      String touchStatus = pulseSensor.getTouchStatus(ir);
      int bpm = pulseSensor.calculateBPM(ir);
      
      bluetooth.sendData(motion, bpm, touchStatus);
      lastBluetoothSend = millis();
      
      // בדיקת חוסר תנועה
      if (motionSensor.detectNoMotion(10000)) {
        bluetooth.sendAlert("אין תנועה כבר 10 שניות");
      }
    }
    
    // הצגה מתמצתת ל-Serial
    if (millis() - lastPrint > 3000) {
      Serial.print("📊 תנועה: ");
      Serial.print(motionSensor.getMotionMagnitude(), 1);
      
      Serial.print(" | מגע: ");
      Serial.print(pulseSensor.getTouchStatus(ir));
      
      int bpm = pulseSensor.calculateBPM(ir);
      if (bpm > 0) {
        Serial.print(" | דופק: ");
        Serial.print(bpm);
        Serial.print(" BPM");
        
        // צפצוף עדין
        buzzer.beep(30, 1500);
      }
      
      // בדיקת חוסר תנועה
      if (motionSensor.detectNoMotion(10000)) {
        Serial.print(" | 😴 חוסר תנועה");
      }
      
      Serial.println();
      lastPrint = millis();
    }
  }
}

// פונקציות עזר נוספות
void handleBluetoothCommands() {
  String command = bluetooth.getCommand();
  if (command.length() > 0) {
    if (command == "ping") {
      bluetooth.sendAlert("pong - המערכת עובדת!");
    }
    else if (command == "1") {
      currentMode = MOTION_DETECTION;
      bluetooth.sendAlert("מצב: זיהוי תנועה");
    }
    else if (command == "2") {
      currentMode = PULSE_MEASUREMENT;
      bluetooth.sendAlert("מצב: מדידת דופק");
    }
    else if (command == "3") {
      currentMode = COMBINED_MODE;
      bluetooth.sendAlert("מצב: משולב");
    }
  }
}

void emergencyAlert() {
  for (int i = 0; i < 5; i++) {
    buzzer.beep(200, 3000);
    delay(100);
  }
}