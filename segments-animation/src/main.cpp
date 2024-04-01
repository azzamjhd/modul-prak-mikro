#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;
int mili_counter = 0, prev_mili_counter = 0;
int buttons = 0;

void sendCommand(uint8_t value);
void sendData(uint8_t address, uint8_t value);
void reset();

void display7segment(uint8_t index, uint8_t value) {
  sendCommand(0x44); // sending command to set certain address
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | (index*2));
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

void setup() {
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(0x8f); // sending init command to control the display
  reset();
}

void loop() {
  for (uint8_t i = 0; i < 8; i++) {
    display7segment(i, 0b00001000);
    delay(500);
  }
  display7segment(7, 0b00001100);
  delay(500);
  display7segment(7, 0b00001110);
  delay(500);
  display7segment(7, 0b00001111);
  delay(500);
  for (uint8_t i = 6; i >= 0; i--) {
    display7segment(i, 0b00001001);
    delay(500);
  }
  display7segment(0, 29);
  delay(500);
  display7segment(0, 39);
  delay(500);
  display7segment(0, 31);
  delay(500);
  for (int i = 1; i < 7; i++) {
    display7segment(i, 0b00000001);
    delay(500);
  }
  display7segment(7, 0b00000111);
  delay(500);
  display7segment(7, 0b00000011);
  delay(500);
  display7segment(7, 0b00000001);
  delay(500);
  for (int i = 7; i > 0; i--) {
    digitalWrite(i, 0);
    delay(500);
  }
  display7segment(0, 30);
  delay(500);
  display7segment(0, 10);
  delay(500);
  display7segment(0, 0);
  delay(500);
}


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