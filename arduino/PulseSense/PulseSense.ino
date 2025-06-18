#include "MPU6050Sensor.h"
#include "VibrationMotor.h"
#include "MAX30102Sensor.h"
#include "BLEHandler.h"
#include "TemperatureSensor.h"

// הגדרות פינים
#define VIBRATION_PIN 13

// יצירת אובייקטים
VibrationMotor vibrationMotor(VIBRATION_PIN);
MPU6050Sensor motionSensor;
MAX30102Sensor pulseSensor;
BLEHandler bluetooth;
TemperatureSensor tempSensor;

// משתנים למצב הפעולה
enum OperationMode {
  MOTION_DETECTION,
  PULSE_MEASUREMENT,
  TEMPERATURE_READING,
  COMBINED_MODE,
  DIAGNOSTIC_MODE  // מצב חדש לאבחון
};

OperationMode currentMode = COMBINED_MODE;
unsigned long lastPrint = 0;
bool systemInitialized = false;

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("🔥 PulseSense - מערכת מוניטורינג מתקדמת V2.0");
  Serial.println("================================================");
  
  // אתחול רכיבים
  initializeSystem();
  
  // רצף התחלה
  startupSequence();
  
  Serial.println("📋 פקודות:");
  Serial.println("   1 - מצב זיהוי תנועה");
  Serial.println("   2 - מצב מדידת דופק");
  Serial.println("   3 - מצב מדידת טמפרטורה");  
  Serial.println("   4 - מצב משולב");
  Serial.println("   5 - מצב אבחון");
  Serial.println("   + - הגבר עוצמת LED דופק");
  Serial.println("   - - הקטן עוצמת LED דופק");
  Serial.println("   r - איפוס חיישן דופק");
  Serial.println("   d - אבחון חיישן דופק");
  Serial.println("   i - מידע רגיסטרים");
  Serial.println("   s - סריקת I2C");
  Serial.println();
  
  systemInitialized = true;
  lastPrint = millis();
  
  Serial.println("✅ מערכת מוכנה לפעולה!");
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
      
    case TEMPERATURE_READING:
      handleTemperatureReading();
      break;
      
    case COMBINED_MODE:
      handleCombinedMode();
      break;
      
    case DIAGNOSTIC_MODE:
      handleDiagnosticMode();
      break;
  }
  
  delay(200); // 5 מדידות לשנייה
}

void initializeSystem() {
  Serial.println("🔧 מאתחל מערכת...");
  
  // אתחול I2C ראשון - חשוב שזה יהיה לפני כל החיישנים!
  Wire.begin(21, 22);
  Wire.setClock(100000); // 100kHz לייצובות
  delay(100);
  Serial.println("   ✅ I2C מאותחל");
  
  // אתחול מנוע רטט
  vibrationMotor.begin();
  Serial.println("   ✅ מנוע רטט מוכן");
  vibrationMotor.vibrateAlert();
  
  // אתחול Bluetooth
  if (bluetooth.begin()) {
    Serial.println("   ✅ Bluetooth מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול Bluetooth");
  }
  
  // סריקת I2C לאבחון
  Serial.println("🔍 סורק מכשירי I2C...");
  scanI2CDevices();
  
  // אתחול חיישן תנועה
  if (motionSensor.begin()) {
    Serial.println("   ✅ חיישן תנועה MPU6050 מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול חיישן תנועה");
  }
  
  // אתחול חיישן טמפרטורה
  if (tempSensor.begin()) {
    Serial.println("   ✅ חיישן טמפרטורה AM-057 מוכן");
  } else {
    Serial.println("   ❌ שגיאה באתחול חיישן טמפרטורה");
  }
  
  // אתחול חיישן דופק - אחרון כי הוא הכי בעייתי
  Serial.println("💓 מאתחל חיישן דופק...");
  if (pulseSensor.begin()) {
    Serial.println("   ✅ חיישן דופק MAX30102 מוכן");
    bluetooth.sendAlert("מערכת אותחלה בהצלחה עם כל החיישנים!");
  } else {
    Serial.println("   ❌ שגיאה באתחול חיישן דופק");
    Serial.println("   💡 נסה:");
    Serial.println("      • בדוק חיבורים");
    Serial.println("      • לחץ 'r' לאיפוס");
    Serial.println("      • לחץ 'd' לאבחון מתקדם");
    bluetooth.sendAlert("מערכת אותחלה - בעיה בחיישן דופק");
  }
  
  Serial.println("✅ אתחול הושלם!");
}

void scanI2CDevices() {
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("   🔍 מכשיר: 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      
      // זיהוי מכשירים ידועים
      switch(address) {
        case 0x48:
          Serial.print(" (AM-057 Temperature)");
          break;
        case 0x57:
          Serial.print(" (MAX30102 Pulse)");
          break;
        case 0x68:
          Serial.print(" (MPU6050 Motion)");
          break;
        default:
          Serial.print(" (Unknown)");
          break;
      }
      Serial.println();
      nDevices++;
    }
  }
  
  Serial.print("   📊 נמצאו ");
  Serial.print(nDevices);
  Serial.println(" מכשירים");
  
  if (nDevices == 0) {
    Serial.println("   ❌ לא נמצאו מכשירים - בדוק חיבורי I2C!");
  }
}

void startupSequence() {
  Serial.println("🎵 רצף התחלה...");
  for (int i = 0; i < 3; i++) {
    vibrationMotor.vibrate(200);
    delay(100);
  }
  vibrationMotor.vibrate(500);
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
        currentMode = TEMPERATURE_READING;
        Serial.println("🌡️ מצב: מדידת טמפרטורה בלבד");
        break;
        
      case '4':
        currentMode = COMBINED_MODE;
        Serial.println("🔄 מצב: משולב");
        break;
        
      case '5':
        currentMode = DIAGNOSTIC_MODE;
        Serial.println("🔍 מצב: אבחון");
        break;
        
      case '+':
        pulseSensor.increaseLEDPower();
        break;
        
      case '-':
        pulseSensor.decreaseLEDPower();
        break;
        
      case 'r':
      case 'R':
        Serial.println("🔄 מאפס חיישן דופק...");
        pulseSensor.resetSensor();
        break;
        
      case 'd':
      case 'D':
        Serial.println("🔍 מפעיל אבחון חיישן דופק...");
        pulseSensor.diagnosticMode();
        break;
        
      case 'i':
      case 'I':
        pulseSensor.readAllRegisters();
        break;
        
      case 's':
      case 'S':
        Serial.println("🔍 סורק I2C...");
        scanI2CDevices();
        break;
        
      case 'h':
      case 'H':
        showHelp();
        break;
    }
  }
}

void showHelp() {
  Serial.println("📋 עזרה - כל הפקודות:");
  Serial.println("מצבי פעולה:");
  Serial.println("  1-5 = שינוי מצב פעולה");
  Serial.println("חיישן דופק:");
  Serial.println("  + - = עוצמת LED");
  Serial.println("  r = איפוס");
  Serial.println("  d = אבחון מתקדם");
  Serial.println("  i = מידע רגיסטרים");
  Serial.println("מערכת:");
  Serial.println("  s = סריקת I2C");
  Serial.println("  h = עזרה זו");
}

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
      currentMode = TEMPERATURE_READING;
      bluetooth.sendAlert("מצב: מדידת טמפרטורה");
    }
    else if (command == "4") {
      currentMode = COMBINED_MODE;
      bluetooth.sendAlert("מצב: משולב");
    }
    else if (command == "5") {
      currentMode = DIAGNOSTIC_MODE;
      bluetooth.sendAlert("מצב: אבחון");
    }
    else if (command == "reset_pulse") {
      pulseSensor.resetSensor();
      bluetooth.sendAlert("חיישן דופק אופס");
    }
  }
}

void handleMotionDetection() {
  motionSensor.update();

  // בדיקת נפילה
  if (motionSensor.detectFall(12.0)) {
    Serial.println("🚨 זוהתה נפילה!");
    vibrationMotor.vibrateAlert();
    bluetooth.sendAlert("זוהתה נפילה!");
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
    if (!pulseSensor.dataIsChanging()) {
      Serial.println("⚠️ נתונים לא יציבים - עושה איפוס...");
      pulseSensor.resetSensor();
      return;
    }

    // הצגת נתונים מפורטת
    Serial.print("💓 IR: ");
    Serial.print(ir);
    Serial.print(" | Red: ");
    Serial.print(red);

    // זיהוי מגע
    String touchStatus = getTouchStatus(ir);
    Serial.print(" | ");
    Serial.print(touchStatus);

    // חישוב דופק
    int bpm = pulseSensor.calculateBPM(ir);
    if (bpm > 0) {
      Serial.print(" | 💓 ");
      Serial.print(bpm);
      Serial.print(" BPM");

      // רטט עם הדופק
      vibrationMotor.vibrateGentle(50);
    }

    Serial.println();

  } else {
    Serial.println("❌ שגיאה בקריאת חיישן דופק");
    
    // ספירת שגיאות רצופות
    static int errorCount = 0;
    errorCount++;
    
    if (errorCount > 10) {
      Serial.println("🔄 יותר מדי שגיאות - מבצע איפוס...");
      pulseSensor.resetSensor();
      errorCount = 0;
    }
  }
}

void handleTemperatureReading() {
  float temperature = tempSensor.readTemperature();
  
  // הצגת טמפרטורה
  if (millis() - lastPrint > 2000) {
    if (temperature != -999) {
      Serial.print("🌡️ טמפרטורה: ");
      Serial.print(temperature, 1);
      Serial.print("°C - ");
      Serial.println(tempSensor.evaluateTemperature(temperature));
      
      // התראה על חום גבוה
      if (temperature > 38.5) {
        Serial.println("🚨 טמפרטורה גבוהה!");
        vibrationMotor.vibrate(300);
        bluetooth.sendAlert("טמפרטורה גבוהה: " + String(temperature, 1) + "°C");
      }
    } else {
      Serial.println("❌ שגיאה בקריאת טמפרטורה");
    }
    lastPrint = millis();
  }
}

void handleCombinedMode() {
  // עדכון חיישן תנועה
  motionSensor.update();
  
  // בדיקות חירום (תנועה)
  if (motionSensor.detectFall(12.0)) {
    Serial.println("🚨 זוהתה נפילה!");
    bluetooth.sendAlert("זוהתה נפילה!");
    vibrationMotor.vibrateAlert();
    return; // דחיפות עליונה לנפילה
  }
  
  // מדידת דופק
  uint32_t red, ir;
  bool pulseDataValid = pulseSensor.readData(&red, &ir);
  
  // שליחת נתונים לאפליקציה כל שנייה
  static unsigned long lastBluetoothSend = 0;
  if (millis() - lastBluetoothSend > 1000) {
    float motion = motionSensor.getMotionMagnitude();
    String touchStatus = getTouchStatus(ir);
    int bpm = 0;
    
    if (pulseDataValid) {
      bpm = pulseSensor.calculateBPM(ir);
    }
    
    // קריאת טמפרטורה
    float temperature = tempSensor.readTemperature();
    
    bluetooth.sendData(motion, bpm, touchStatus, temperature);
    lastBluetoothSend = millis();
    
    // בדיקת חוסר תנועה
    if (motionSensor.detectNoMotion(10000)) {
      bluetooth.sendAlert("אין תנועה כבר 10 שניות");
    }
    
    // בדיקת חום גבוה
    if (temperature > 38.5 && temperature != -999) {
      bluetooth.sendAlert("טמפרטורה גבוהה: " + String(temperature, 1) + "°C");
    }
  }
  
  // הצגה מתמצתת ל-Serial
  if (millis() - lastPrint > 3000) {
    Serial.print("📊 תנועה: ");
    Serial.print(motionSensor.getMotionMagnitude(), 1);
    
    if (pulseDataValid) {
      Serial.print(" | מגע: ");
      Serial.print(getTouchStatus(ir));
      
      int bpm = pulseSensor.calculateBPM(ir);
      if (bpm > 0) {
        Serial.print(" | דופק: ");
        Serial.print(bpm);
        Serial.print(" BPM");
        
        // רטט עדין
        vibrationMotor.vibrateGentle(30);
      }
    } else {
      Serial.print(" | דופק: שגיאה");
    }
    
    // הצגת טמפרטורה
    float temp = tempSensor.readTemperature();
    if (temp != -999) {
      Serial.print(" | טמפ': ");
      Serial.print(temp, 1);
      Serial.print("°C");
    }
    
    // בדיקת חוסר תנועה
    if (motionSensor.detectNoMotion(10000)) {
      Serial.print(" | 😴 חוסר תנועה");
    }
    
    Serial.println();
    lastPrint = millis();
  }
}

void handleDiagnosticMode() {
  // מצב אבחון מתקדם
  static unsigned long lastDiagnostic = 0;
  
  if (millis() - lastDiagnostic > 5000) {
    Serial.println("🔍 מצב אבחון פעיל");
    Serial.println("   לחץ 'd' לאבחון מלא של חיישן הדופק");
    Serial.println("   לחץ 'i' למידע רגיסטרים");
    Serial.println("   לחץ 's' לסריקת I2C");
    Serial.println("   לחץ '4' לחזרה למצב משולב");
    
    lastDiagnostic = millis();
  }
  
  // קריאה בסיסית
  uint32_t red, ir;
  if (pulseSensor.readData(&red, &ir)) {
    static int diagCount = 0;
    diagCount++;
    
    if (diagCount % 5 == 0) {
      Serial.print("🔍 IR: ");
      Serial.print(ir);
      Serial.print(", Red: ");
      Serial.print(red);
      Serial.print(", Touch: ");
      Serial.println(getTouchStatus(ir));
    }
  }
  
  delay(200);
}

// פונקציה חדשה להערכת מצב המגע
String getTouchStatus(uint32_t irValue) {
  if (pulseSensor.isStrongTouch()) {
    return "מגע חזק";
  } else if (pulseSensor.isTouch()) {
    return "מגע";
  } else {
    return "אין מגע";
  }
}