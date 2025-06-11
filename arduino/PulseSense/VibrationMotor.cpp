#include "VibrationMotor.h"

// Constructor
VibrationMotor::VibrationMotor(int vibrationPin) {
    pin = vibrationPin;
}

// אתחול הפין
void VibrationMotor::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // ודא שהרטט כבוי בהתחלה
}

// רטט בסיסי
void VibrationMotor::vibrate(int duration_ms) {
    digitalWrite(pin, HIGH); // הפעל רטט
    delay(duration_ms);
    digitalWrite(pin, LOW);  // כבה רטט
}

// רטט עם עוצמה מותאמת (0-255)
void VibrationMotor::vibratePWM(int power, int duration_ms) {
    analogWrite(pin, power);
    delay(duration_ms);
    analogWrite(pin, 0);
}

// התראת רטט - 3 רטטים קצרים
void VibrationMotor::vibrateAlert() {
    for(int i = 0; i < 3; i++) {
        vibrate(200);
        delay(150);
    }
}

// תבנית רטט מיוחדת (קצר-קצר-ארוך)
void VibrationMotor::vibratePattern() {
    vibrate(100); // קצר
    delay(100);
    vibrate(100); // קצר
    delay(100);
    vibrate(500); // ארוך
}

// רטט עדין (לחיסכון בזרם)
void VibrationMotor::vibrateGentle(int duration_ms) {
    vibratePWM(120, duration_ms); // כ-47% כוח
}

// עצירה מיידית
void VibrationMotor::stop() {
    digitalWrite(pin, LOW);
    analogWrite(pin, 0);
}