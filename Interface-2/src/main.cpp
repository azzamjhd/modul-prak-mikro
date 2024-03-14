#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;

// send 1 byte data
void sendCommand(uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

// send 2 bytes data, first byte is the address, second byte is the value
void sendData(uint8_t address, uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | address);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

// read the button states
uint8_t readButtons(void) {
  uint8_t buttons = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);
  pinMode(data, INPUT);
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }
  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);
  return buttons;
}

// reset all the address
void reset() {
  sendCommand(0x40);
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
  sendCommand(0x8f);
  reset();
  Serial.begin(9600);
}

uint8_t i = 1;

void loop() {
  sendCommand(0x44);
  if (i % 2 != 0) {
    sendData(0x0f & i, 0x01);
    Serial.println("hidup");
  }

  uint8_t buttons = readButtons();

  if (buttons == 0x01) {
    i--;
  } else {
    i++;
  }

  delay(100);
  reset();
}