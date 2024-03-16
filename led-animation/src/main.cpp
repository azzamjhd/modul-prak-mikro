#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;
uint8_t a = 0;
uint8_t left = 0x88;
uint8_t right = 0x11;

void sendCommand(uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void sendData(uint8_t address, uint8_t value) {
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
}

uint8_t i = 0x01;

void loop() {
  sendCommand(0x44); // sending command to set certain address
  for (int i = 0; i < 8; i += 2) {
    sendData(0x0f & (15-((2*i))), (((a&(1<<i))==0)?0x00:0x01));
  }
  right = right << 1 | right >> 7;
  left = left >> 1 | left << 7;
  a = (left & 0xf0) | (right & 0x0f);
  delay(100);
}
