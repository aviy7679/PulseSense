#ifndef VIBRATION_MOTOR_H
#define VIBRATION_MOTOR_H

#include <Arduino.h>

class VibrationMotor {
private:
    int pin;
    
public:
    // Constructor - מאתחל את הפין
    VibrationMotor(int vibrationPin);
    
    // אתחול הפין (קוראים ב-setup)
    void begin();
    
    // פונקציה שמעדכנת את הרטט
    void update(); 

    
    // רטט בסיסי לזמן מסוים (מילישניות)
    void vibrate(int duration_ms);
    
    // רטט עם עוצמה מותאמת (0-255)
    void vibratePWM(int power, int duration_ms);
    
    // התראת רטט - 3 רטטים קצרים
    void vibrateAlert();
    
    // תבנית רטט מיוחדת (קצר-קצר-ארוך)
    void vibratePattern();
    
    // רטט עדין (לחיסכון בזרם)
    void vibrateGentle(int duration_ms);
    
    // עצירה מיידית של הרטט
    void stop();
};

#endif