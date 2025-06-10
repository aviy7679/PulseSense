#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor() {
  calibrationOffset = 55.0; // ערך ברירת מחדל
}

bool TemperatureSensor::begin(int sda, int scl) {
  Wire.begin(sda, scl);
  Wire.setClock(100000);
  delay(100);
  
  // בדיקת חיבור
  Wire.beginTransmission(AM057_ADDRESS);
  return (Wire.endTransmission() == 0);
}

float TemperatureSensor::readTemperature() {
  float rawTemp = readRawTemperature();
  if (rawTemp == -999.0) {
    return -999.0; // שגיאה
  }
  
  return rawTemp + calibrationOffset;
}

float TemperatureSensor::readRawTemperature() {
  Wire.beginTransmission(AM057_ADDRESS);
  Wire.write(0x00);
  if (Wire.endTransmission(false) != 0) {
    return -999.0;
  }
  
  if (Wire.requestFrom(AM057_ADDRESS, 2) != 2) {
    return -999.0;
  }
  
  uint8_t msb = Wire.read();
  uint8_t lsb = Wire.read();
  uint16_t raw = (msb << 8) | lsb;
  
  int16_t signed_raw = (int16_t)raw;
  float temperature = signed_raw / 256.0;
  
  return temperature;
}

String TemperatureSensor::evaluateTemperature(float temp) {
  if (temp == -999.0) {
    return "שגיאה בקריאה";
  }
  
  if (temp >= 36.1 && temp <= 37.2) {
    return "טמפרטורת גוף תקינה";
  } else if (temp >= 37.3 && temp <= 38.0) {
    return "חום קל";
  } else if (temp > 38.0 && temp <= 39.0) {
    return "חום - יש להתייעץ עם רופא";
  } else if (temp > 39.0 && temp <= 42.0) {
    return "חום גבוה! פנייה מיידית לרופא";
  } else if (temp < 36.1 && temp >= 35.0) {
    return "טמפרטורה נמוכה מהרגיל";
  } else if (temp < 35.0 && temp >= 25.0) {
    return "לא במגע - הצמד את החיישן למצח";
  } else if (temp < 25.0) {
    return "טמפרטורת חדר - החיישן לא במגע";
  } else {
    return "ערך לא סביר";
  }
}

bool TemperatureSensor::isBodyTemperature(float temp) {
  return (temp >= 35.0 && temp <= 42.0);
}

void TemperatureSensor::setCalibration(float offset) {
  calibrationOffset = offset;
}

float TemperatureSensor::getCalibrationOffset() {
  return calibrationOffset;
}