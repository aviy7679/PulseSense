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
    digitalWrite(pin, HIGH);
    startTime = millis();
    vibrateDuration = duration_ms;
    isVibrating = true;
}

// רטט עם עוצמה מותאמת (0-255)
void VibrationMotor::vibratePWM(int power, int duration_ms) {
    analogWrite(pin, power);
    startTime = millis();
    vibrateDuration = duration_ms;
    isVibrating = true;
}

// התראת רטט - 3 רטטים קצרים
void VibrationMotor::vibrateAlert() {
    patternStep = 0;
    lastPatternTime = millis() - 151; // תכף מתחיל
}

// תבנית רטט מיוחדת (קצר-קצר-ארוך)
void VibrationMotor::vibratePattern() {
    patternStep = 100; // סימון מיוחד לתבנית
    lastPatternTime = millis() - 101;
}

// רטט עדין (לחיסכון בזרם)
void VibrationMotor::vibrateGentle(int duration_ms) {
    vibratePWM(120, duration_ms); // כ-47% כוח
}

// עצירה מיידית
void VibrationMotor::stop() {
    digitalWrite(pin, LOW);
    analogWrite(pin, 0);
    isVibrating = false;

}

void VibrationMotor::update() {
    unsigned long currentTime = millis();

    // טיפול ברטט בסיסי
    if (isVibrating && currentTime - startTime >= vibrateDuration) {
        stop();
    }

    // התראת רטט (3 קצרים)
    if (patternStep >= 0 && patternStep < 6) {
        if (currentTime - lastPatternTime >= 150) {
            if (patternStep % 2 == 0) {
                vibrate(200); // ON
            } else {
                stop();       // OFF
            }
            lastPatternTime = currentTime;
            patternStep++;
        }
    }

    // תבנית קצר-קצר-ארוך
    if (patternStep >= 100 && patternStep < 106) {
        if (currentTime - lastPatternTime >= 100) {
            if (patternStep == 100 || patternStep == 102) vibrate(100);
            if (patternStep == 104) vibrate(500);
            if (patternStep % 2 == 1) stop();
            lastPatternTime = currentTime;
            patternStep++;
        }
    }
}