#ifndef MAX30102_SENSOR_H
#define MAX30102_SENSOR_H

#include <Arduino.h>
#include <Wire.h>

class MAX30102Sensor {
  public:
    MAX30102Sensor();
    bool begin(int sda = 21, int scl = 22);
    bool readData(uint32_t *red, uint32_t *ir);
    void resetSensor();
    void increaseLEDPower();
    void decreaseLEDPower();
    void updateLEDs();
    uint8_t getLEDPower();
    bool isTouch();
    bool isStrongTouch();
    int calculateBPM(uint32_t irValue);
    bool dataIsChanging();
    void processSerialCommands();
    
  private:
    void writeRegister(uint8_t reg, uint8_t value);
    
    // קבועים
    static const uint8_t MAX30105_ADDRESS = 0x57;
    static const uint8_t MIN_LED_POWER = 0x08;
    static const uint8_t MAX_LED_POWER = 0x3F;
    static const uint8_t LED_POWER_STEP = 0x08;
    
    // משתנים פרטיים
    uint8_t ledPower;
    uint32_t lastIR;
    uint32_t lastRed;
    bool dataChanging;
    int unchangedCount;
    uint32_t lastPulseValue;
    unsigned long lastPulseTime;
    int lastBPM;
};

#endif