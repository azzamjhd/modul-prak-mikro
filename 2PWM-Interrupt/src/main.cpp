#include <Arduino.h>

void setup() {
  TCCR1A = 0B10100010;          // Clear OC1A/OC1B on Compare Match when up-counting
  TCCR1B = 0B00010001;          // PWM, Phase and Frequency Correct Mode
  ICR1 = 533;
}

void loop() {
  OCR1A = ICR1/4; // 25% duty cycle
  OCR1B = ICR1/2; // 50% duty cycle
}