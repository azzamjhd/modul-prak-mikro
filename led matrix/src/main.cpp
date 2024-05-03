#include <Arduino.h>

uint8_t a = 1;

void setup() {
  DDRD = 0xFF;
  DDRB = 0x0F;
  PORTB = 0x00;
}

void loop() {
  PORTD = a;
  if (a == 0x80) {
    a = 1;
  } else {
    a = a << 1;
  }
  if (a > 0x08) {
    PORTB = (a >> 4);
  } else {
    PORTB = a;
  }
  delay(10);
}