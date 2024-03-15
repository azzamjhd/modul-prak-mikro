#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;

const char data7Segment[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
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

void displaySegment(int numIncr, int numDecr) {
  int digits[8];
  // 4 segment left
  digits[0] = numIncr / 1000;
  digits[1] = (numIncr % 1000) / 100;
  digits[2] = (numIncr % 100) / 10;
  digits[3] = numIncr % 10;
  // 4 segment right
  digits[4] = numDecr / 1000;
  digits[5] = (numDecr % 1000) / 100;
  digits[6] = (numDecr % 100) / 10;
  digits[7] = numDecr % 10;
  
  bool suppressZeroLeft = true;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < i; j++) {
      if (digits[j] != 0) {
        suppressZeroLeft = false;
        break;
      }
    }
    if (suppressZeroLeft && digits[i] == 0) {
      sendData(0x00 | (2*i), 0x00);
    } else {
      sendData(0x00 | (2*i), data7Segment[digits[i]]);}
  }

  bool suppressZeroRight = true;
  for (int i = 4; i < 8; i++) {
    for (int j = 4; j < i; j++) {
      if (digits[j] != 0) {
        suppressZeroRight = false;
        break;
      }
    }
    if (suppressZeroRight && digits[i] == 0) {
      sendData(0x00 | (2*i), 0x00);
    } else {
      sendData(0x00 | (2*i), data7Segment[digits[i]]);}
  }
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

int Up = 0;
int Down = 9999;

void loop() {
  displaySegment(Up, Down);
  
  Up++;
  Down--;  
  
  if (Up > 9999) {
    Up = 0;
  }
  if (Down < 0) {
    Down = 9999;
  }

  delay(100);
}