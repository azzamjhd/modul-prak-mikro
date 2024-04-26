#include <Arduino.h>
const int NRP = 21;

void setup() {
  TCCR1A = 0B10100010;
  TCCR1B = 0B00011001;
  ICR1 = 160000/NRP;
}

void loop() {
  OCR1A = ICR1/4;
  OCR1B = ICR1/2;
}