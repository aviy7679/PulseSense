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
    
    /**
    * מחשב קצב לב (BPM) לפי ערכי IR מהחיישן.
    * מזהה פסגות, מודד זמן בין פעימות, מחליק רעש עם ממוצע נע.
    *
    * @param irValue ערך אינפרה-אדום נוכחי מהחיישן
    * @return BPM מחושב או אחרון ידוע
    */

    int calculateBPM(uint32_t irValue);
    bool dataIsChanging();
    void processSerialCommands();
    
    // פונקציות אבחון 
    void diagnosticMode();
    bool testConnection();
    void readAllRegisters();
    
  private:
    void writeRegister(uint8_t reg, uint8_t value);
    /**
    * קוראת בייט אחד מרגיסטר בכתובת נתונה בחיישן MAX30102 באמצעות I2C.
    *
    * @param reg כתובת רגיסטר פנימי (למשל 0x09)
    * @return הערך שנמצא ברגיסטר, או 0 במקרה של כשל
    */
    uint8_t readRegister(uint8_t reg);  
    bool clearFIFO();
    bool verifySettings();

    /**
      * בודקת אם הגדרות החיישן נמחקו (למשל אחרי ריסט) ומשחזרת אם צריך.
      *
      * @return true אם הכל תקין או השחזור הצליח, false אם השחזור נכשל
      */
    bool checkAndRestoreSensor();

    
    // קבועים
    static const uint8_t MAX30105_ADDRESS = 0x57;
    static const uint8_t MIN_LED_POWER = 0x08;
    static const uint8_t MAX_LED_POWER = 0x3F;
    static const uint8_t LED_POWER_STEP = 0x08;

    static const uint8_t REG_MODE_CONFIG = 0x09;
    static const uint8_t REG_SPO2_CONFIG = 0x0A;
    static const uint8_t REG_LED_RED = 0x0C;
    static const uint8_t REG_LED_IR = 0x0D;
    static const uint8_t REG_FIFO_WR_PTR = 0x04;
    static const uint8_t REG_FIFO_RD_PTR = 0x06;
    static const uint8_t MODE_HR_SPO2 = 0x03;
    static const uint8_t CONFIG_SPO2_DEFAULT = 0x27;

    
    // משתנים פרטיים
    uint8_t ledPower;
    uint32_t lastIR;
    uint32_t lastRed;
    bool dataChanging;
    int unchangedCount;
    uint32_t lastPulseValue;
    unsigned long lastPulseTime;
    int lastBPM;
    
    // משתנים חדשים לאבחון
    unsigned long lastSuccessfulRead;
    int consecutiveErrors;
    bool sensorInitialized;
};

#endif