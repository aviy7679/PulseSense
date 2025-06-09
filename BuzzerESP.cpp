#include "BuzzerESP.h"
#include <Arduino.h>

BuzzerESP::BuzzerESP(int buzzerPin) {
  pin = buzzerPin;
}

void BuzzerESP::begin() {
  pinMode(pin, OUTPUT);
  off();
}

void BuzzerESP::beep(unsigned int durationMs, unsigned int freq) {
  on(freq);
  delay(durationMs);
  off();
}

void BuzzerESP::on(unsigned int freq) {
  tone(pin, freq);
}

void BuzzerESP::off() {
  noTone(pin);
}