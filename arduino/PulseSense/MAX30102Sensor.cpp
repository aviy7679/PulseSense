#include "MAX30102Sensor.h"

MAX30102Sensor::MAX30102Sensor() {
  ledPower = 0x1F;
  lastIR = 0;
  lastRed = 0;
  dataChanging = false;
  unchangedCount = 0;
  lastPulseValue = 0;
  lastPulseTime = 0;
  lastBPM = 0;
  lastSuccessfulRead = 0;
  consecutiveErrors = 0;
  sensorInitialized = false;
}

bool MAX30102Sensor::begin(int sda, int scl) {
  Serial.println("💓 MAX30102 אתחול");
  
  // בדיקת חיבור
  if (!testConnection()) {
    Serial.println("❌ כשל בבדיקת חיבור");
    return false;
  }
  
  // איפוס 
  Serial.println("🔄 מבצע איפוס ...");
  
  //01000000 = 0x40  מהdatasheet
  writeRegister(REG_MODE_CONFIG, 0x40);
  delay(300);
  
  // וודא שהאיפוס עבד
  int timeout = 0;
  while ((readRegister(REG_MODE_CONFIG) & 0x40) && timeout < 100) {
    delay(10);
    timeout++;
  }
  
  if (timeout >= 100) {
    Serial.println("❌ איפוס נכשל - חיישן לא מגיב");
    return false;
  }
  
  // ניקוי FIFO מלא
  if (!clearFIFO()) {
    Serial.println("❌ כשל בניקוי FIFO");
    return false;
  }
  
  // אתחול עם הגדרות מותאמות
  Serial.println("🔧 מגדיר פרמטרים...");
  
  writeRegister(REG_MODE_CONFIG, MODE_HR_SPO2); // HR + SpO2 mode
  delay(50);
  writeRegister(REG_SPO2_CONFIG, CONFIG_SPO2_DEFAULT); // Sample rate 100Hz, pulse width 411μs, ADC range 4096
  delay(50);
  writeRegister(0x08, 0x7C); // FIFO config: average 8 sample, rollover enabled
  delay(50);
  
  // הגדרת עוצמת LED
  writeRegister(REG_LED_RED, ledPower); // Red LED
  writeRegister(REG_LED_IR, ledPower); // IR LED
  delay(100);
  
  // וידוא הגדרות
  if (!verifySettings()) {
    Serial.println("❌ וידוא הגדרות נכשל");
    return false;
  }
  
  // בדיקות קריאה ראשוניות
  Serial.println("🧪 בדיקות ראשוניות...");
  for (int i = 0; i < 5; i++) {
    uint32_t testRed, testIR;
    if (readData(&testRed, &testIR)) {
      Serial.print("   ");
      Serial.print(i + 1);
      Serial.print(": IR=");
      Serial.print(testIR);
      Serial.print(", Red=");
      Serial.println(testRed);
    } else {
      Serial.print("   ");
      Serial.print(i + 1);
      Serial.println(": שגיאה בקריאה");
    }
    delay(100);
  }
  
  sensorInitialized = true;
  lastSuccessfulRead = millis();
  consecutiveErrors = 0;
  
  Serial.println("✅ MAX30102 מוכן לפעולה!");
  return true;
}

//בדיקת חיבור
bool MAX30102Sensor::testConnection() {
  Serial.println("🔍 בודק חיבור...");
  
  // בדיקה בסיסית
  for (int i = 0; i < 3; i++) {
    Wire.beginTransmission(MAX30105_ADDRESS);
    // קוד הצלחה = 0
    if (Wire.endTransmission() == 0) {
      Serial.println("✅ חיישן מגיב ב-I2C");
      break;
    } else {
      Serial.print("❌ ניסיון ");
      Serial.print(i + 1);
      Serial.println(" נכשל");
      delay(100);
      
      if (i == 2) {
        Serial.println("❌ החיישן לא מגיב לאחר 3 ניסיונות!");
        return false;
      }
    }
  }
  
  // בדיקת Part ID
  // לכל רכיב יש רגיסטר שונה שמכיל ערך קבוע ועוזר לזיהוי האם זה אותו רכיב באמת
  uint8_t partID = readRegister(0xFF);
  Serial.print("📋 Part ID: 0x");
  Serial.println(partID, HEX);
  
  if (partID != 0x15) {
    Serial.println("❌ Part ID שגוי - ציפיתי ל-0x15");
    return false;
  }
  
  Serial.println("✅ Part ID נכון - MAX30102 זוהה!");
  return true;
}

bool MAX30102Sensor::clearFIFO() {
  // ניקוי מלא של FIFO
  writeRegister(REG_FIFO_WR_PTR, 0x00); // לאן תיכתב הדגימה הבאה
  writeRegister(0x05, 0x00); // כמה פעמים נדרסו נתונים בגלל גלישה  
  writeRegister(REG_FIFO_RD_PTR, 0x00); // המקום הבא ממנו תיקרא הדגימה
  
  delay(50);
  
  // וידוא שהניקוי עבד
  uint8_t writePtr = readRegister(REG_FIFO_WR_PTR);
  uint8_t readPtr = readRegister(REG_FIFO_RD_PTR);
  
  if (writePtr == 0 && readPtr == 0) {
    Serial.println("✅ FIFO נוקה בהצלחה");
    return true;
  } else {
    Serial.println("❌ כשל בניקוי FIFO");
    return false;
  }
}

//מוודא שההגדרות תקינות
bool MAX30102Sensor::verifySettings() {
  uint8_t mode = readRegister(REG_MODE_CONFIG);
  uint8_t spo2Config = readRegister(REG_SPO2_CONFIG);
  uint8_t redLED = readRegister(REG_LED_RED);
  uint8_t irLED = readRegister(REG_LED_IR);
  
  Serial.println("📊 וידוא הגדרות:");
  Serial.print("   Mode: 0x"); Serial.println(mode, HEX);
  Serial.print("   SpO2 Config: 0x"); Serial.println(spo2Config, HEX);
  Serial.print("   Red LED: 0x"); Serial.println(redLED, HEX);
  Serial.print("   IR LED: 0x"); Serial.println(irLED, HEX);
  
  bool success = true;
  if (mode != MODE_HR_SPO2) {
    Serial.println("❌ Mode שגוי");
    success = false;
  }
  if (spo2Config != CONFIG_SPO2_DEFAULT) {
    Serial.println("❌ SpO2 Config שגוי");
    success = false;
  }
  if (redLED != ledPower || irLED != ledPower) {
    Serial.println("❌ LED Power שגוי");
    success = false;
  }
  
  return success;
}

bool MAX30102Sensor::readData(uint32_t *red, uint32_t *ir) {
  // בדיקה שהחיישן עדיין מגיב
  Wire.beginTransmission(MAX30105_ADDRESS);
  if (Wire.endTransmission() != 0) {
    consecutiveErrors++;
    Serial.println("❌ חיישן לא מגיב ב-I2C");
    if (consecutiveErrors > 5) {
      Serial.println("🔄 חיישן אבד - מאתחל מחדש...");
      begin(); // אתחול מחדש מלא
      consecutiveErrors = 0;
    }
    return false;
  }
  
  //בדיקה שהחיישן לא נכבה*
  uint8_t modeReg = readRegister(REG_MODE_CONFIG);
  if (modeReg == 0x00) {
    Serial.println("🚨 החיישן נכבה! מאתחל מחדש...");
    
    // אתחול מהיר
    writeRegister(REG_MODE_CONFIG, MODE_HR_SPO2); // HR + SpO2 mode
    delay(10);
    writeRegister(REG_SPO2_CONFIG, CONFIG_SPO2_DEFAULT); // SpO2 config
    delay(10);
    writeRegister(REG_LED_RED, ledPower); // Red LED
    writeRegister(REG_LED_IR, ledPower); // IR LED
    delay(50);
    
    // וירוף שהתיקון עבד
    modeReg = readRegister(REG_MODE_CONFIG);
    if (modeReg != MODE_HR_SPO2) {
      Serial.println("❌ כשל בתיקון - חיישן פגום או בעיית כוח");
      return false;
    } else {
      Serial.println("✅ חיישן הופעל מחדש");
    }
  }
  
  // בדיקת FIFO pointers
  uint8_t writePtr = readRegister(REG_FIFO_WR_PTR);
  uint8_t readPtr = readRegister(REG_FIFO_RD_PTR);
  
  // אם אין נתונים חדשים, נמתין
  if (writePtr == readPtr) {
    delay(10); // המתנה קצרה
    writePtr = readRegister(REG_FIFO_WR_PTR);
    readPtr = readRegister(REG_FIFO_RD_PTR);
    
    if (writePtr == readPtr) {
      return false; // עדיין אין נתונים
    }
  }
  
  // קריאת נתונים עם retry logic
  bool success = false;
  for (int attempt = 0; attempt < 3; attempt++) {
    Wire.beginTransmission(MAX30105_ADDRESS);
    Wire.write(0x07); // FIFO Data register
    
    if (Wire.endTransmission(false) == 0) {
      //קורא לבאפר פנימי של מחלקת Wire בתוך ESP32
      int received = Wire.requestFrom(MAX30105_ADDRESS, 6);
      
      if (received == 6) {
        // קריאת Red (3 bytes ראשונים)
        *red = 0;
        *red |= (uint32_t)Wire.read() << 16;
        *red |= (uint32_t)Wire.read() << 8;
        *red |= Wire.read();
        *red &= 0x3FFFF;
        
        // קריאת IR (3 bytes אחרונים)  
        *ir = 0;
        *ir |= (uint32_t)Wire.read() << 16;
        *ir |= (uint32_t)Wire.read() << 8;
        *ir |= Wire.read();
        *ir &= 0x3FFFF;
        
        success = true;
        break;
      }
    }
    
    delay(5); // המתנה קצרה בין ניסיונות
  }
  
  if (!success) {
    consecutiveErrors++;
    Serial.print("❌ כשל בקריאת FIFO (ניסיון ");
    Serial.print(consecutiveErrors);
    Serial.println(")");
    return false;
  }
  
  // איפוס מונה שגיאות אם הקריאה הצליחה
  consecutiveErrors = 0;
  lastSuccessfulRead = millis();
  
  // בדיקת תקינות נתונים
  if (*red == 0 && *ir == 0) {
    // נתונים אפסיים - אולי FIFO ריק
    return false;
  }
  
  // בדיקת oversaturation
  if (*red == 0x3FFFF || *ir == 0x3FFFF) {
    clearFIFO();
    Serial.println("⚠️ Oversaturation - מקטין עוצמת LED");
    if (ledPower > MIN_LED_POWER) {
      ledPower -= LED_POWER_STEP;
      updateLEDs();
    }
  }
  
  // בדיקת נתונים חלשים מדי (רק אם יש מגע)
  if (*ir < 1000 && *ir > 0) {
    Serial.println("⚠️ איתות חלש - מגביר עוצמת LED");
    if (ledPower < MAX_LED_POWER) {
      ledPower += LED_POWER_STEP;
      updateLEDs();
    }
  }
  
  // בדיקה משופרת של נתונים תקועים
  if (*ir == lastIR && *red == lastRed && (*ir > 0 || *red > 0)) {
    unchangedCount++;
    
    if (unchangedCount > 15) {
      Serial.println("🔄 נתונים תקועים - מנקה FIFO...");
      clearFIFO();
      unchangedCount = 0;
      return false;
    }
  } else {
    unchangedCount = 0;
    dataChanging = true;
  }
  
  lastIR = *ir;
  lastRed = *red;
  
  return true;
}

void MAX30102Sensor::resetSensor() {
  Serial.println("🔄 איפוס מלא של החיישן...");
  
  sensorInitialized = false;
  
  // איפוס קשיח
  writeRegister(REG_MODE_CONFIG, 0x40);
  delay(500);
  
  // המתנה לאיפוס מלא
  int timeout = 0;
  while ((readRegister(REG_MODE_CONFIG) & 0x40) && timeout < 100) {
    delay(10);
    timeout++;
  }
  
  if (timeout >= 100) {
    Serial.println("⚠️ איפוס ארך יותר מהצפוי");
  }
  
  // ניקוי FIFO מלא
  clearFIFO();
  
  // אתחול מחדש עם הגדרות
  writeRegister(REG_MODE_CONFIG, MODE_HR_SPO2); // HR + SpO2 mode
  delay(50);
  writeRegister(REG_SPO2_CONFIG, CONFIG_SPO2_DEFAULT); // הגדרות בסיסיות
  delay(50);
  writeRegister(0x08, 0x0F); // FIFO config
  delay(50);
  
  // איפוס עוצמת LED לערך בסיסי
  ledPower = 0x1F;
  updateLEDs();
  
  // איפוס משתנים
  lastIR = 0;
  lastRed = 0;
  unchangedCount = 0;
  lastPulseValue = 0;
  lastPulseTime = 0;
  lastBPM = 0;
  consecutiveErrors = 0;
  
  // קריאות "dummy" לניקוי פייפליין
  for (int i = 0; i < 5; i++) {
    uint32_t dummy1, dummy2;
    readData(&dummy1, &dummy2);
    delay(50);
  }
  
  sensorInitialized = true;
  lastSuccessfulRead = millis();
  
  Serial.println("✅ איפוס הושלם");
}

void MAX30102Sensor::increaseLEDPower() {
  if (ledPower < MAX_LED_POWER) {
    ledPower += LED_POWER_STEP;
    updateLEDs();
    Serial.print("💡 עוצמה: 0x");
    Serial.println(ledPower, HEX);
  } else {
    Serial.println("💡 עוצמה במקסימום");
  }
}

void MAX30102Sensor::decreaseLEDPower() {
  if (ledPower > MIN_LED_POWER) {
    ledPower -= LED_POWER_STEP;
    updateLEDs();
    Serial.print("💡 עוצמה: 0x");
    Serial.println(ledPower, HEX);
  } else {
    Serial.println("💡 עוצמה במינימום");
  }
}

void MAX30102Sensor::updateLEDs() {
  writeRegister(REG_LED_RED, ledPower); // Red LED
  writeRegister(REG_LED_IR, ledPower); // IR LED
  delay(10); // המתנה קצרה לעדכון
}

uint8_t MAX30102Sensor::getLEDPower() {
  return ledPower;
}

bool MAX30102Sensor::isTouch() {
  return lastIR > 15000;
}

bool MAX30102Sensor::isStrongTouch() {
  return lastIR > 50000;
}

int MAX30102Sensor::calculateBPM(uint32_t irValue) {
  // אלגוריתם דופק
  static uint32_t lastValue = 0;
  static unsigned long lastTime = 0;
  static int bpmBuffer[5] = {0};
  static int bufferIndex = 0;
  static bool bufferFull = false;
  
  // זיהוי עליה חדה - מדד לדופק
  if (irValue > 40000 && irValue > lastValue + 3000) {
    unsigned long now = millis();

    // 40-150 BPM - סינון ערכים לא הגיוניים
    if (now - lastTime > 400 && now - lastTime < 1500) { 
      int currentBPM = 60000 / (now - lastTime);
      
      if (currentBPM > 50 && currentBPM < 150) {
        // הוספה לbuffer לחישוב ממוצע
        bpmBuffer[bufferIndex] = currentBPM;
        bufferIndex = (bufferIndex + 1) % 5;
        if (bufferIndex == 0) bufferFull = true;
        
        // חישוב ממוצע
        if (bufferFull) {
          int sum = 0;
          for (int i = 0; i < 5; i++) {
            sum += bpmBuffer[i];
          }
          lastBPM = sum / 5;
        } else {
          lastBPM = currentBPM;
        }
        
        lastTime = now;
        return lastBPM;
      }
    }
    lastTime = now;
  }
  
  lastValue = irValue;
  return lastBPM; // מחזיר את הדופק האחרון הידוע
}

bool MAX30102Sensor::dataIsChanging() {
  return unchangedCount < 10;
}

void MAX30102Sensor::processSerialCommands() {
  if (Serial.available()) {
    char cmd = Serial.read();
    
    switch(cmd) {
      case '+':
        increaseLEDPower();
        break;
        
      case '-':
        decreaseLEDPower();
        break;
        
      case 'r':
      case 'R':
        resetSensor();
        break;
        
      case 'd':
      case 'D':
        diagnosticMode();
        break;
        
      case 'i':
      case 'I':
        readAllRegisters();
        break;
        
      case 'h':
      case 'H':
        Serial.println("📋 פקודות זמינות:");
        Serial.println("  + = הגבר עוצמת LED");
        Serial.println("  - = הקטן עוצמת LED");
        Serial.println("  r = איפוס חיישן");
        Serial.println("  d = מצב אבחון");
        Serial.println("  i = מידע רגיסטרים");
        Serial.println("  h = עזרה");
        break;
        
      default:
        Serial.println("❓ פקודה לא מוכרת. לחץ 'h' לעזרה.");
        break;
    }
  }
}

void MAX30102Sensor::diagnosticMode() {
  Serial.println("🔍 מצב אבחון - 30 שניות");
  
  unsigned long startTime = millis();
  int readCount = 0;
  int errorCount = 0;
  uint32_t minIR = 0xFFFFFFFF, maxIR = 0;
  uint32_t lastValue = 0;
  int sameCount = 0;
  
  while (millis() - startTime < 30000) {
    uint32_t red, ir;
    
    if (readData(&red, &ir)) {
      readCount++;
      
      if (ir == lastValue) sameCount++;
      if (ir < minIR) minIR = ir;
      if (ir > maxIR) maxIR = ir;
      
      lastValue = ir;
      
      // הדפסה מתונה
      if (readCount % 10 == 0) {
        Serial.print("📊 קריאה ");
        Serial.print(readCount);
        Serial.print(": IR=");
        Serial.print(ir);
        Serial.print(", Red=");
        Serial.println(red);
      }
      
    } else {
      errorCount++;
      Serial.print("X");
    }
    
    delay(100);
  }
  
  Serial.println("\n📊 תוצאות אבחון:");
  Serial.print("  קריאות מוצלחות: ");
  Serial.println(readCount);
  Serial.print("  שגיאות: ");
  Serial.println(errorCount);
  Serial.print("  ערכים זהים: ");
  Serial.print(sameCount);
  Serial.print(" (");
  if (readCount > 0) {
    Serial.print((sameCount * 100) / readCount);
  } else {
    Serial.print("0");
  }
  Serial.println("%)");
  Serial.print("  טווח IR: ");
  Serial.print(minIR);
  Serial.print(" - ");
  Serial.println(maxIR);
  
  if (readCount == 0) {
    Serial.println("🚨 אין קריאות מוצלחות - בעיה חמורה!");
  } else if (sameCount > (readCount * 0.8)) {
    Serial.println("🚨 יותר מ-80% ערכים זהים - נתונים תקועים!");
  } else if (errorCount > (readCount * 0.5)) {
    Serial.println("⚠️ יותר מ-50% שגיאות - בעיה בחיבור");
  } else {
    Serial.println("✅ החיישן עובד תקין");
  }
}

void MAX30102Sensor::readAllRegisters() {
  Serial.println("📊 מצב רגיסטרים:");
  
  struct RegisterInfo {
    uint8_t addr;
    const char* name;
  };
  
  RegisterInfo regs[] = {
    {0x00, "INT_STAT1"},
    {0x01, "INT_STAT2"}, 
    {0x02, "INT_EN1"},
    {REG_FIFO_WR_PTR, "FIFO_WR"},
    {0x05, "FIFO_OV"},
    {REG_FIFO_RD_PTR, "FIFO_RD"},
    {0x08, "FIFO_CFG"},
    {REG_MODE_CONFIG, "MODE_CFG"},
    {REG_SPO2_CONFIG, "SPO2_CFG"},
    {REG_LED_RED, "LED1_PA"},
    {REG_LED_IR, "LED2_PA"},
    {0xFF, "PART_ID"}
  };
  
  for (int i = 0; i < 12; i++) {
    uint8_t val = readRegister(regs[i].addr);
    Serial.print("  0x");
    if (regs[i].addr < 0x10) Serial.print("0");
    Serial.print(regs[i].addr, HEX);
    Serial.print(" (");
    Serial.print(regs[i].name);
    Serial.print("): 0x");
    Serial.println(val, HEX);
  }
  
  // בדיקות נוספות
  uint8_t writePtr = readRegister(REG_FIFO_WR_PTR);
  uint8_t readPtr = readRegister(REG_FIFO_RD_PTR);
  Serial.print("📊 FIFO Status: ");
  Serial.print(writePtr - readPtr);
  Serial.println(" samples available");
}

uint8_t MAX30102Sensor::readRegister(uint8_t reg) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  
  Wire.requestFrom(MAX30105_ADDRESS, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

void MAX30102Sensor::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

bool MAX30102Sensor::checkAndRestoreSensor() {
  uint8_t modeReg = readRegister(REG_MODE_CONFIG);
  uint8_t ledRed = readRegister(REG_LED_RED);
  uint8_t ledIR = readRegister(REG_LED_IR);
  
  // בדיקה שהחיישן עדיין מוגדר נכון
  if (modeReg != MODE_HR_SPO2 || ledRed == 0x00 || ledIR == 0x00) {
    Serial.println("🚨 החיישן איבד הגדרות - משחזר...");
    
    // שחזור הגדרות
    writeRegister(REG_MODE_CONFIG, MODE_HR_SPO2); // HR + SpO2 mode
    delay(10);
    writeRegister(REG_SPO2_CONFIG, CONFIG_SPO2_DEFAULT); // SpO2 config  
    delay(10);
    writeRegister(REG_LED_RED, ledPower); // Red LED
    writeRegister(REG_LED_IR, ledPower); // IR LED
    delay(50);
    
    // וידוא
    modeReg = readRegister(REG_MODE_CONFIG);
    return (modeReg == MODE_HR_SPO2);
  }
  
  return true; // הכל תקין
}