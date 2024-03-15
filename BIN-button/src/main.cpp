#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;
uint8_t currentButtons = 0;
uint8_t lastButtons = 0;

const uint8_t numbers[10] = {
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

uint8_t buttonSort(uint8_t number) {
  uint8_t upper = (number & 0xf0) >> 4;
  uint8_t lower = number & 0x0f;
  uint8_t result = 0;
  for (int i = 0; i < 4; i++) {
    result <<= 2;
    result |= (lower & 1);
    lower >>= 1;
    result |= ((upper & 1) << 1);
    upper >>= 1;
  }
  return result;
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
}

void loop() {
  currentButtons = readButtons();
  if (currentButtons != lastButtons) {
    // uint8_t sortedButtons = buttonSort(currentButtons);
    uint8_t digit[4] = {0};
    digit[0] = currentButtons / 1000;
    digit[1] = (currentButtons % 1000) / 100;
    digit[2] = (currentButtons % 100) / 10;
    digit[3] = currentButtons % 10;
    bool suppressZero = true;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < i; j++) {
        if (digit[j] != 0) {
          suppressZero = false;
          break;
        }
      }
      if (suppressZero && digit[i] == 0) {
        sendData(0x00 | (2*i), 0x00);
      } else {
        sendData(0x00 | (2*i), numbers[digit[i]]);
      }
    }
  }
  lastButtons = currentButtons;
  delay(100);
}
