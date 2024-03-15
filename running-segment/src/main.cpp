#include <Arduino.h>

int Incr = 0;
const int strobe = 9;
const int clock = 7;
const int data = 8;
char display[8] = {0};
const uint8_t segment[2] = {
  0B01110110, // H
  0B10000110 // I.
};

void sendCommand(uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void sendData(uint8_t address, uint8_t value) {
  sendCommand(0x44); // sending command to set certain address
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | address);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void reset() {
  sendCommand(0x40); // sending command to set consecutive addresses to 0
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);
  for (uint8_t i = 0; i < 16; i++) {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}

void setup() {
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(0x8f); // sending init command to control the display
  reset();
  Serial.begin(9600);

  int length = sizeof(segment) / sizeof(segment[0]);

  for (int i = 0; i < length; i++) {
    display[i] = segment[i];
  }
}

void loop() {
  for (int i = 0; i < 8; i++) {
    sendData(0x00 | (2 * (i + (Incr % 8 ))), display[i]);
  }
  
  Incr++;
  delay(400);
  reset();
}