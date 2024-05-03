#include <Arduino.h>

void setup() {
  TCCR1A = 0B10100010;          // Clear OC1A/OC1B on Compare Match when up-counting
  TCCR1B = 0B00010001;          // PWM, Phase and Frequency Correct Mode
  ICR1 = 7620;
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {
  OCR1A = 1905; // 25% duty cycle
  OCR1B = 3810; // 50% duty cycle
}