#ifndef MAX30102_SENSOR_H
#define MAX30102_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class MAX30102Sensor {
  public:
    MAX30102Sensor();
    bool begin(int sda = 21, int scl = 22);
    bool readData(uint32_t *red, uint32_t *ir);
    void increasePower();
    void decreasePower();
    void resetSensor();
    
    // זיהוי מגע ודופק
    String getTouchStatus(uint32_t ir);
    int calculateBPM(uint32_t ir);
    
    // פונקציות עזר
    uint8_t getCurrentPower();
    bool isDataStable();
    
  private:
    static const uint8_t MAX30105_ADDRESS = 0x57;
    
    uint8_t ledPower;
    uint32_t lastIR, lastRed;
    int unchangedCount;
    
    // למדידת דופק
    uint32_t lastPulseValue;
    unsigned long lastPulseTime;
    
    void writeRegister(uint8_t reg, uint8_t value);
    void updateLEDs();
    void initializeSettings();
};

#endif