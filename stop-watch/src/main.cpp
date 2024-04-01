#include <Arduino.h>

/*!
  name  : Stopwatch
  Desc  : Aplikasi timer sederhana dengan 7-segment display dan tombol
  Detail: Aplikasi ini menggunakan TM1638 sebagai driver 7-segment display dan
  tombol. Aplikasi ini memiliki beberapa fitur:
  - Menampilkan waktu dalam format menit:detik:mili
  - Reset timer
  - Menyimpan waktu yang telah dijalankan
  - Menampilkan waktu yang telah disimpan
  - Start
*/

const int strobe = 9;
const int clock = 7;
const int data = 8;
int mili_counter = 0, prev_mili_counter = 0;
int buttons = 0;
bool timerRunning = true;
int currentTime = 0, prevTime = 0;

const int MAX_STORED_TIME = 5;
int storedTimes[MAX_STORED_TIME] = {0};

const uint8_t data7Segment[21] = {
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
    // with dot
    0b10111111, // 0
    0b10000110, // 1
    0b11011011, // 2
    0b11001111, // 3
    0b11100110, // 4
    0b11101101, // 5
    0b11111101, // 6
    0b10000111, // 7
    0b11111111, // 8
    0b11101111, // 9
    // Blank space
    0b00000000 // blank space
};

uint8_t readButtons(void);
void storeTime();
void displayStoredTime(int index);
void sendCommand(uint8_t value);
void sendData(uint8_t address, uint8_t value);
void displaySegment(int value);
void reset();
void startStopTimer();

ISR(TIMER1_COMPA_vect) { // interrupt service routine
  OCR1A += 20000;        // setting the next interrupt
  mili_counter++;        // incrementing the mili_counter
}

void setup() {
  TCCR1A = 0;          // setting timer1 to normal mode
  TCCR1B = 0;          // setting timer1 to normal mode
  TCCR1B |= B00000010; // setting prescaler to 8
  OCR1A = 20000;       // setting the compare value
  TIMSK1 |= B00000010; // enabling the compare interrupt

  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(0x8f); // sending init command to control the display
  Serial.begin(9600);
  reset();
}

void loop() {
  displaySegment(mili_counter);
  buttons = readButtons();
  Serial.println(buttons);
  currentTime = millis();
  if (currentTime - prevTime > 400) {
    switch (buttons) {
    case 1: // Reset
      mili_counter = 0;
      reset();
      break;
    case 2: // Step : take the current time and store it
      storeTime();
      break;
    case 4: // Start and Stop
      startStopTimer();
      break;
    case 8: // step 1: show the first stored time
      displayStoredTime(0);
      break;
    case 16: // step 2: show the second stored time
      displayStoredTime(1);
      break;
    case 32: // step 3: show the third stored time
      displayStoredTime(2);
      break;
    case 64: // step 4: show the fourth stored time
      displayStoredTime(3);
      break;
    case 128: // step 5: show the fifth stored time
      displayStoredTime(4);
      break;
    }
  prevTime = currentTime;
  }
}

void startStopTimer() {
  timerRunning = !timerRunning;
  if (timerRunning) {
    TIMSK1 |= B00000010;
  } else {
    TIMSK1 &= ~B00000010;
  }
}

void displaySegment(int value) {
  int mili = value % 100;
  int detik = (value / 100) % 60;
  int menit = (value / 6000) % 60;
  static int digits[8];
  static int prev_digits[8];
  digits[0] = menit / 10;
  digits[1] =(menit % 10) + 10; // with dot
  digits[2] = 21;
  digits[3] = detik / 10;
  digits[4] =(detik % 10) + 10; // with dot
  digits[5] = 21;
  digits[6] = mili / 10;
  digits[7] = mili % 10;

  for (int i = 0; i < 8; i++) {
    sendData(0x00 | (2 * i), data7Segment[digits[i]]);
  }
}

void storeTime() {
  for (int i = MAX_STORED_TIME - 1; i > 0; i--) {
    storedTimes[i] = storedTimes[i - 1];
  }
storedTimes[0] = mili_counter;
}

void displayStoredTime(int index) {
  TIMSK1 &= ~B00000010;
  reset();
  if (index >= 0 && index < MAX_STORED_TIME) {
    mili_counter = storedTimes[index];
  } else {
    displaySegment(0);
  }
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
  digitalWrite(strobe, HIGH);
  pinMode(data, OUTPUT);
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