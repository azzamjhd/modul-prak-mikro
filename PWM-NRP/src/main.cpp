#include <Arduino.h>
const int NRP = 21;

void setup() {
  TCCR1A = 0B10100010;          // Clear OC1A/OC1B on Compare Match when up-counting
  TCCR1B = 0B00011001;          // WGM13 = 1, WGM = 1, Prescaler 1
  ICR1 = 16000/NRP;
}

void loop() {
  OCR1A = ICR1/2; // 50% duty cycle
  OCR1B = ICR1/2; // 50% duty cycle
}