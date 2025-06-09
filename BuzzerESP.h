#ifndef BUZZER_ESP_H
#define BUZZER_ESP_H

#include <Arduino.h>

class BuzzerESP {
  private:
    int pin;

  public:
    BuzzerESP(int buzzerPin);
    void begin();
    void beep(unsigned int durationMs = 100, unsigned int freq = 2000);
    void on(unsigned int freq = 2000);
    void off();
};

#endif