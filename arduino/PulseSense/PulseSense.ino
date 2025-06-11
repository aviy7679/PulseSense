// #include "MPU6050Sensor.h"
// #include "BuzzerESP.h"
// #include "MAX30102Sensor.h"
// #include "BLEHandler.h"
// #include "TemperatureSensor.h"

// // הגדרות פינים
// #define BUZZER_PIN 12

// // יצירת אובייקטים
// BuzzerESP buzzer(BUZZER_PIN);
// MPU6050Sensor motionSensor;
// MAX30102Sensor pulseSensor;
// BLEHandler bluetooth;
// TemperatureSensor tempSensor;

// // משתנים למצב הפעולה
// enum OperationMode {
//   MOTION_DETECTION,
//   PULSE_MEASUREMENT,
//   TEMPERATURE_READING,
//   COMBINED_MODE
// };

// OperationMode currentMode = COMBINED_MODE;
// unsigned long lastPrint = 0;
// bool systemInitialized = false;

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
  
//   Serial.println("🔥 PulseSense - מערכת מוניטורינג מתקדמת");
//   Serial.println("==========================================");
  
//   // אתחול רכיבים
//   initializeSystem();
  
//   // צפצוף התחלה
//   startupSequence();
  
//   Serial.println("📋 פקודות:");
//   Serial.println("   1 - מצב זיהוי תנועה");
//   Serial.println("   2 - מצב מדידת דופק");
//   Serial.println("   3 - מצב מדידת טמפרטורה");
//   Serial.println("   4 - מצב משולב");
//   Serial.println("   + - הגבר עוצמת LED");
//   Serial.println("   - - הקטן עוצמת LED");
//   Serial.println("   r - איפוס חיישן דופק");
//   Serial.println();
//   Serial.println("🔍 סורק I2C...");
//   for (int addr = 1; addr < 127; addr++) {
//     Wire.beginTransmission(addr);
//     if (Wire.endTransmission() == 0) {
//       Serial.print("✅ מכשיר נמצא: 0x");
//       Serial.println(addr, HEX);
//     }
// }
//   systemInitialized = true;
//   lastPrint = millis();
// }

// void loop() {
//   // עדכון BLE status
//   bluetooth.update();
  
//   // בדיקת פקודות מהמשתמש (Serial ו-Bluetooth)
//   handleUserCommands();
//   handleBluetoothCommands();
  
//   // ביצוע פעולות לפי המצב הנוכחי
//   switch (currentMode) {
//     case MOTION_DETECTION:
//       handleMotionDetection();
//       break;
      
//     case PULSE_MEASUREMENT:
//       handlePulseMeasurement();
//       break;
      
//     case TEMPERATURE_READING:
//       handleTemperatureReading();
//       break;
      
//     case COMBINED_MODE:
//       handleCombinedMode();
//       break;
//   }
  
//   delay(200); // 5 מדידות לשנייה
// }

// void initializeSystem() {
//   Serial.println("🔧 מאתחל מערכת...");
  
//   // אתחול באזר
//   buzzer.begin();
//   Serial.println("   ✅ באזר מוכן");
  
//   // אתחול Bluetooth
//   if (bluetooth.begin()) {
//     Serial.println("   ✅ Bluetooth מוכן");
//   } else {
//     Serial.println("   ❌ שגיאה באתחול Bluetooth");
//   }
  
//   // אתחול חיישן תנועה
//   if (motionSensor.begin()) {
//     Serial.println("   ✅ חיישן תנועה MPU6050 מוכן");
//   } else {
//     Serial.println("   ❌ שגיאה באתחול חיישן תנועה");
//   }
  
//   // אתחול חיישן דופק
//   if (pulseSensor.begin()) {
//     Serial.println("   ✅ חיישן דופק MAX30102 מוכן");
//   } else {
//     Serial.println("   ❌ שגיאה באתחול חיישן דופק");
//   }
  
//   // אתחול חיישן טמפרטורה
//   if (tempSensor.begin()) {
//     Serial.println("   ✅ חיישן טמפרטורה AM-057 מוכן");
//   } else {
//     Serial.println("   ❌ שגיאה באתחול חיישן טמפרטורה");
//   }
  
//   bluetooth.sendAlert("מערכת אותחלה בהצלחה עם כל החיישנים!");
//   Serial.println("✅ אתחול הושלם!");
// }

// void startupSequence() {
//   Serial.println("🎵 רצף התחלה...");
//   for (int i = 0; i < 3; i++) {
//     buzzer.beep(200, 1500);
//     delay(100);
//   }
//   buzzer.beep(500, 2000);
// }

// void handleUserCommands() {
//   if (Serial.available()) {
//     char cmd = Serial.read();
    
//     switch (cmd) {
//       case '1':
//         currentMode = MOTION_DETECTION;
//         Serial.println("🏃 מצב: זיהוי תנועה בלבד");
//         break;
        
//       case '2':
//         currentMode = PULSE_MEASUREMENT;
//         Serial.println("💓 מצב: מדידת דופק בלבד");
//         break;
        
//       case '3':
//         currentMode = TEMPERATURE_READING;
//         Serial.println("🌡️ מצב: מדידת טמפרטורה בלבד");
//         break;
        
//       case '4':
//         currentMode = COMBINED_MODE;
//         Serial.println("🔄 מצב: משולב");
//         break;
        
//       case '+':
//         pulseSensor.increaseLEDPower();
//         Serial.print("💡 עוצמת LED: 0x");
//         Serial.println(pulseSensor.getLEDPower(), HEX);
//         break;
        
//       case '-':
//         pulseSensor.decreaseLEDPower();
//         Serial.print("💡 עוצמת LED: 0x");
//         Serial.println(pulseSensor.getLEDPower(), HEX);
//         break;
        
//       case 'r':
//         Serial.println("🔄 מאפס חיישן דופק...");
//         pulseSensor.resetSensor();
//         Serial.println("✅ איפוס הושלם");
//         break;
//     }
//   }
// }

// void handleBluetoothCommands() {
//   String command = bluetooth.getCommand();
//   if (command.length() > 0) {
//     if (command == "ping") {
//       bluetooth.sendAlert("pong - המערכת עובדת!");
//     }
//     else if (command == "1") {
//       currentMode = MOTION_DETECTION;
//       bluetooth.sendAlert("מצב: זיהוי תנועה");
//     }
//     else if (command == "2") {
//       currentMode = PULSE_MEASUREMENT;
//       bluetooth.sendAlert("מצב: מדידת דופק");
//     }
//     else if (command == "3") {
//       currentMode = TEMPERATURE_READING;
//       bluetooth.sendAlert("מצב: מדידת טמפרטורה");
//     }
//     else if (command == "4") {
//       currentMode = COMBINED_MODE;
//       bluetooth.sendAlert("מצב: משולב");
//     }
//   }
// }

// void handleMotionDetection() {
//   motionSensor.update();

//   // בדיקת נפילה
//   if (motionSensor.detectFall(12.0)) {
//     Serial.println("🚨 זוהתה נפילה!");
//     buzzer.beep(1000, 3000); // אזעקה חזקה
//   }

//   // בדיקת חוסר תנועה
//   if (motionSensor.detectNoMotion(5000)) {
//     Serial.println("😴 אין תנועה למשך 5 שניות");
//   }

//   // הצגת נתוני תנועה
//   if (millis() - lastPrint > 2000) {
//     float motion = motionSensor.getMotionMagnitude();
//     Serial.print("🏃 עוצמת תנועה: ");
//     Serial.println(motion, 2);
//     lastPrint = millis();
//   }
// }

// void handlePulseMeasurement() {
//   uint32_t red, ir;

//   if (pulseSensor.readData(&red, &ir)) {
//     // בדיקת יציבות נתונים
//     if (!pulseSensor.dataIsChanging()) {
//       Serial.println("⚠️ נתונים לא יציבים - עושה איפוס...");
//       pulseSensor.resetSensor();
//       return;
//     }

//     // הצגת נתונים
//     Serial.print("💓 IR: ");
//     Serial.print(ir);
//     Serial.print(" | Red: ");
//     Serial.print(red);

//     // זיהוי מגע
//     String touchStatus = getTouchStatus(ir);
//     Serial.print(" | ");
//     Serial.print(touchStatus);

//     // חישוב דופק
//     int bpm = pulseSensor.calculateBPM(ir);
//     if (bpm > 0) {
//       Serial.print(" | 💓 ");
//       Serial.print(bpm);
//       Serial.print(" BPM");

//       // צפצוף עם הדופק
//       buzzer.beep(50, 2000);
//     }

//     Serial.println();

//   } else {
//     Serial.println("❌ שגיאה בקריאת חיישן דופק");
//   }
// }

// void handleTemperatureReading() {
//   float temperature = tempSensor.readTemperature();
  
//   // הצגת טמפרטורה
//   if (millis() - lastPrint > 2000) {
//     if (temperature != -999) {
//       Serial.print("🌡️ טמפרטורה: ");
//       Serial.print(temperature, 1);
//       Serial.print("°C - ");
//       Serial.println(tempSensor.evaluateTemperature(temperature));
      
//       // התראה על חום גבוה
//       if (temperature > 38.5) {
//         Serial.println("🚨 טמפרטורה גבוהה!");
//         buzzer.beep(300, 2500);
//       }
//     } else {
//       Serial.println("❌ שגיאה בקריאת טמפרטורה");
//     }
//     lastPrint = millis();
//   }
// }

// void handleCombinedMode() {
//   // עדכון חיישן תנועה
//   motionSensor.update();
  
//   // בדיקות חירום (תנועה)
//   // if (motionSensor.detectFall(12.0)) {
//   //   Serial.println("🚨 זוהתה נפילה!");
//   //   bluetooth.sendAlert("זוהתה נפילה!");
//   //   buzzer.beep(1000, 3000);
//   //   return; // דחיפות עליונה לנפילה
//   // }
  
//   // מדידת דופק
//   uint32_t red, ir;
//   if (pulseSensor.readData(&red, &ir)) {
    
//     // שליחת נתונים לאפליקציה כל שנייה
//     static unsigned long lastBluetoothSend = 0;
//     if (millis() - lastBluetoothSend > 1000) {
//       float motion = motionSensor.getMotionMagnitude();
//       String touchStatus = getTouchStatus(ir);
//       int bpm = pulseSensor.calculateBPM(ir);
      
//       // קריאת טמפרטורה
//       float temperature = tempSensor.readTemperature();
      
//       bluetooth.sendData(motion, bpm, touchStatus, temperature);
//       lastBluetoothSend = millis();
      
//       // בדיקת חוסר תנועה
//       if (motionSensor.detectNoMotion(10000)) {
//         bluetooth.sendAlert("אין תנועה כבר 10 שניות");
//       }
      
//       // בדיקת חום גבוה
//       if (temperature > 38.5 && temperature != -999) {
//         bluetooth.sendAlert("טמפרטורה גבוהה: " + String(temperature, 1) + "°C");
//       }
//     }
    
//     // הצגה מתמצתת ל-Serial
//     if (millis() - lastPrint > 3000) {
//       Serial.print("📊 תנועה: ");
//       Serial.print(motionSensor.getMotionMagnitude(), 1);
      
//       Serial.print(" | מגע: ");
//       Serial.print(getTouchStatus(ir));
      
//       int bpm = pulseSensor.calculateBPM(ir);
//       if (bpm > 0) {
//         Serial.print(" | דופק: ");
//         Serial.print(bpm);
//         Serial.print(" BPM");
        
//         // צפצוף עדין
//         buzzer.beep(30, 1500);
//       }
      
//       // הצגת טמפרטורה
//       float temp = tempSensor.readTemperature();
//       if (temp != -999) {
//         Serial.print(" | טמפ': ");
//         Serial.print(temp, 1);
//         Serial.print("°C");
//       }
      
//       // בדיקת חוסר תנועה
//       if (motionSensor.detectNoMotion(10000)) {
//         Serial.print(" | 😴 חוסר תנועה");
//       }
      
//       Serial.println();
//       lastPrint = millis();
//     }
//   }
// }

// void emergencyAlert() {
//   for (int i = 0; i < 5; i++) {
//     buzzer.beep(200, 3000);
//     delay(100);
//   }
// }

// // פונקציה חדשה להערכת מצב המגע
// String getTouchStatus(uint32_t irValue) {
//   if (pulseSensor.isStrongTouch()) {
//     return "מגע חזק";
//   } else if (pulseSensor.isTouch()) {
//     return "מגע";
//   } else {
//     return "אין מגע";
//   }
// }

#include <Arduino.h>

#define VIBRATION_PIN 13  

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // הגדרת הפין כיציאה
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW); // ודא שהרטט כבוי
  
  Serial.println("=== מנוע רטט DEBUG ===");
  Serial.print("משתמש בפין: ");
  Serial.println(VIBRATION_PIN);
  Serial.println("כובה את הרטט...");
  
  // ודא שהפין כבוי
  digitalWrite(VIBRATION_PIN, LOW);
  delay(2000);
  
  Serial.println("האם הרטט נעצר עכשיו? (אמור להיות שקט)");
  delay(3000);
  
  Serial.println("עכשיו בדיקה חד-פעמית...");
  vibrate(500); 
  Serial.println("זה היה הרטט היחיד!");
  
  Serial.println("=== מוכן לפקודות ===");
  Serial.println("הקלידי 'v' לרטט");
}

void loop() {
  // DEBUG: בדיקה שהפין אכן LOW
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) { // כל 5 שניות
    Serial.print("מצב פין ");
    Serial.print(VIBRATION_PIN);
    Serial.print(": ");
    Serial.println(digitalRead(VIBRATION_PIN) ? "HIGH (רטט)" : "LOW (שקט)");
    lastCheck = millis();
  }
  
  // בדיקת פקודות מהמשתמש
  if (Serial.available()) {
    char command = Serial.read();
    
    if (command == 'v' || command == 'V') {
      Serial.println("רטט ידני!");
      vibrate(300);
    }
    else if (command == 'a' || command == 'A') {
      Serial.println("התראת רטט!");
      vibrateAlert();
    }
    else if (command == 'p' || command == 'P') {
      Serial.println("תבנית רטט!");
      vibratePattern();
    }
    else if (command == 's' || command == 'S') {
      Serial.println("עצירה מלאה!");
      digitalWrite(VIBRATION_PIN, LOW);
      analogWrite(VIBRATION_PIN, 0);
    }
  }
  
  delay(100);
}

// פונקציה לרטט בסיסי
void vibrate(int duration_ms) {
  digitalWrite(VIBRATION_PIN, HIGH); // הפעל רטט
  delay(duration_ms);
  digitalWrite(VIBRATION_PIN, LOW);  // כבה רטט
}

// רטט עם עוצמה מותאמת (0-255)
void vibratePWM(int power, int duration_ms) {
  analogWrite(VIBRATION_PIN, power);
  delay(duration_ms);
  analogWrite(VIBRATION_PIN, 0);
}

// התראת רטט - 3 רטטים קצרים
void vibrateAlert() {
  for(int i = 0; i < 3; i++) {
    vibrate(200);
    delay(150);
  }
}

// תבנית רטט מיוחדת
void vibratePattern() {
  vibrate(100); // קצר
  delay(100);
  vibrate(100); // קצר
  delay(100);
  vibrate(500); // ארוך
}

// רטט עדין (לחיסכון בזרם)
void vibrateGentle(int duration_ms) {
  vibratePWM(120, duration_ms); // כ-47% כוח
}