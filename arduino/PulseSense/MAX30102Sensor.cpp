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
}

bool MAX30102Sensor::begin(int sda, int scl) {
  Serial.println("💓 MAX30102 - מאתחל...");
  
  // לא מאתחלים I2C - כבר מאותחל על ידי חיישנים אחרים!
  
  // בדיקה שהחיישן מגיב
  Wire.beginTransmission(MAX30105_ADDRESS);
  if (Wire.endTransmission() != 0) {
    Serial.println("❌ שגיאה: החיישן לא מגיב בכתובת 0x57!");
    return false;
  }
  
  // איפוס ואתחול פשוט - בדיוק כמו בקוד שעבד
  Serial.println("🔧 מאתחל...");
  
  writeRegister(0x09, 0x40); // Reset
  delay(200);
  writeRegister(0x09, 0x03); // HR + SpO2 mode
  writeRegister(0x0A, 0x27); // Settings
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
  
  Serial.println("✅ מוכן!");
  return true;
}

bool MAX30102Sensor::readData(uint32_t *red, uint32_t *ir) {
  // קריאה ישירה וברורה - בדיוק כמו בקוד הפשוט שעבד
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(0x07); // FIFO Data
  
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  
  int received = Wire.requestFrom(MAX30105_ADDRESS, 6);
  if (received < 6) {
    return false;
  }
  
  // קריאת Red (3 bytes ראשונים)
  *red = 0;
  *red |= (uint32_t)Wire.read() << 16;
  *red |= (uint32_t)Wire.read() << 8;
  *red |= Wire.read();
  *red &= 0x3FFFF; // 18 bit mask
  
  // קריאת IR (3 bytes אחרונים)
  *ir = 0;
  *ir |= (uint32_t)Wire.read() << 16;
  *ir |= (uint32_t)Wire.read() << 8;
  *ir |= Wire.read();
  *ir &= 0x3FFFF; // 18 bit mask

  // בדיקה אם הנתונים משתנים - בדיוק כמו בקוד הפשוט
  if (*ir == lastIR && *red == lastRed) {
    unchangedCount++;
  } else {
    unchangedCount = 0;
    dataChanging = true;
  }
  
  lastIR = *ir;
  lastRed = *red;
  
  return true;
}

void MAX30102Sensor::resetSensor() {
  Serial.println("🔄 איפוס...");
  
  // לא מאפסים I2C - רק את החיישן עצמו
  
  // בדיוק כמו בקוד הפשוט שעבד
  writeRegister(0x09, 0x40); // Reset
  delay(200);
  writeRegister(0x09, 0x03); // HR + SpO2 mode
  writeRegister(0x0A, 0x27); // Settings
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
  
  // איפוס משתנים
  lastIR = 0;
  lastRed = 0;
  unchangedCount = 0;
  lastPulseValue = 0;
  lastPulseTime = 0;
  lastBPM = 0;
  
  Serial.println("✅ איפוס הושלם");
}

void MAX30102Sensor::increaseLEDPower() {
  if (ledPower < MAX_LED_POWER) {
    ledPower += LED_POWER_STEP;
    updateLEDs();
    Serial.print("💡 עוצמה: 0x");
    Serial.println(ledPower, HEX);
  }
}

void MAX30102Sensor::decreaseLEDPower() {
  if (ledPower > MIN_LED_POWER) {
    ledPower -= LED_POWER_STEP;
    updateLEDs();
    Serial.print("💡 עוצמה: 0x");
    Serial.println(ledPower, HEX);
  }
}

void MAX30102Sensor::updateLEDs() {
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
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
  // זיהוי דופק פשוט מאוד - בדיוק כמו בקוד שעבד
  static uint32_t lastValue = 0;
  static unsigned long lastTime = 0;
  
  if (irValue > 40000 && irValue > lastValue + 5000) { // עלייה משמעותית
    unsigned long now = millis();
    if (now - lastTime > 400 && now - lastTime < 1500) {
      int bpm = 60000 / (now - lastTime);
      if (bpm > 50 && bpm < 150) {
        lastBPM = bpm;
        lastTime = now;
        return bpm;
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
        
      case 'h':
      case 'H':
        Serial.println("📋 פקודות זמינות:");
        Serial.println("  + = הגבר עוצמת LED");
        Serial.println("  - = הקטן עוצמת LED");
        Serial.println("  r = איפוס חיישן");
        Serial.println("  h = עזרה");
        break;
        
      default:
        Serial.println("❓ פקודה לא מוכרת. לחץ 'h' לעזרה.");
        break;
    }
  }
}

void MAX30102Sensor::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}