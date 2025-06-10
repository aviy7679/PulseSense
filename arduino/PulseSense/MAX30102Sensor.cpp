#include "MAX30102Sensor.h"

MAX30102Sensor::MAX30102Sensor() {
  ledPower = 0x1F;
  lastIR = 0;
  lastRed = 0;
  unchangedCount = 0;
  lastPulseValue = 0;
  lastPulseTime = 0;
}

bool MAX30102Sensor::begin(int sda, int scl) {
  Wire.begin(sda, scl);
  delay(100);
  
  // בדיקת חיבור
  Wire.beginTransmission(MAX30105_ADDRESS);
  if (Wire.endTransmission() != 0) {
    return false;
  }
  
  // איפוס ואתחול
  initializeSettings();
  return true;
}

bool MAX30102Sensor::readData(uint32_t *red, uint32_t *ir) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(0x07); // FIFO Data register
  
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
  
  // בדיקה אם הנתונים משתנים
  if (*ir == lastIR && *red == lastRed) {
    unchangedCount++;
  } else {
    unchangedCount = 0;
  }
  
  lastIR = *ir;
  lastRed = *red;
  
  return true;
}

String MAX30102Sensor::getTouchStatus(uint32_t ir) {
  if (ir > 50000) {
    return "מגע חזק";
  } else if (ir > 30000) {
    return "מגע קל";
  } else if (ir > 15000) {
    return "מגע חלש";
  } else {
    return "אין מגע";
  }
}

int MAX30102Sensor::calculateBPM(uint32_t ir) {
  // זיהוי דופק פשוט
  if (ir > 40000 && ir > lastPulseValue + 5000) { // עלייה משמעותית
    unsigned long now = millis();
    if (now - lastPulseTime > 400 && now - lastPulseTime < 1500) {
      int bpm = 60000 / (now - lastPulseTime);
      if (bpm > 50 && bpm < 150) {
        lastPulseTime = now;
        lastPulseValue = ir;
        return bpm;
      }
    }
    lastPulseTime = now;
  }
  lastPulseValue = ir;
  return 0; // אין דופק מזוהה
}

void MAX30102Sensor::increasePower() {
  if (ledPower < 0x3F) {
    ledPower += 0x08;
    updateLEDs();
  }
}

void MAX30102Sensor::decreasePower() {
  if (ledPower > 0x08) {
    ledPower -= 0x08;
    updateLEDs();
  }
}

void MAX30102Sensor::resetSensor() {
  // איפוס I2C
  Wire.end();
  delay(100);
  Wire.begin(21, 22);
  delay(100);
  
  // איפוס החיישן
  writeRegister(0x09, 0x40); // Reset bit
  delay(200);
  
  // הגדרות מחדש
  initializeSettings();
  
  // איפוס משתנים
  lastIR = 0;
  lastRed = 0;
  unchangedCount = 0;
  lastPulseValue = 0;
  lastPulseTime = 0;
}

uint8_t MAX30102Sensor::getCurrentPower() {
  return ledPower;
}

bool MAX30102Sensor::isDataStable() {
  return unchangedCount <= 10;
}

void MAX30102Sensor::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MAX30105_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void MAX30102Sensor::updateLEDs() {
  writeRegister(0x0C, ledPower); // Red LED
  writeRegister(0x0D, ledPower); // IR LED
}

void MAX30102Sensor::initializeSettings() {
  writeRegister(0x09, 0x40); // Reset
  delay(200);
  writeRegister(0x09, 0x03); // HR + SpO2 mode
  writeRegister(0x0A, 0x27); // Settings
  updateLEDs();
}