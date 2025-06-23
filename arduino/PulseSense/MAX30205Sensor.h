#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class MAX30205Sensor {
  public:
    MAX30205Sensor();
    bool begin(int sda = 21, int scl = 22);
    float readTemperature();
    String evaluateTemperature(float temp);
    bool isBodyTemperature(float temp);
    
    // פונקציות קיבול
    void setCalibration(float offset);
    float getCalibrationOffset();
    
  private:
    static const uint8_t MAX30205_ADDRESS = 0x48;
    float calibrationOffset;
    
    float readRawTemperature();
};

#endif