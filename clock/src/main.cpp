#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;
unsigned long int counter = 0;
int buttons = 0, prev_buttons = 0;
int current_time = 0, prev_time = 0;

const uint8_t data7Segment[22] = {
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
    0b00000000, // blank space
    0b10000000  // dot
};

uint8_t readButtons(void);
void sendCommand(uint8_t value);
void sendData(uint8_t address, uint8_t value);
void displaySegment(int counter);
void reset();

/*
 * Time = 1000 ms
 * Prescaler = 256
 * F_CPU = 16 MHz
 * Ticks Count = 62500
 */
ISR(TIMER1_COMPA_vect) { // interrupt service routine
  OCR1A += 62500;        // setting the next interrupt
  counter ++;        // incrementing the counter
}

void setup() {
  TCCR1A = 0;          // setting timer1 to normal mode
  TCCR1B = 0;          // setting timer1 to normal mode
  TCCR1B |= B00000100; // setting prescaler to 256
  OCR1A = 62500;       // setting the compare value
  TIMSK1 |= B00000010; // enabling the compare interrupt

  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(0x8f); // sending init command to control the display
  reset();
}

void loop() {
  displaySegment(counter);
  current_time = millis();
  buttons = readButtons();
  if (current_time - prev_time > 100 && prev_buttons != buttons) {
    switch (buttons) {
    case 1: // jam++
      counter += 3600;
      break;
    case 2: // jam--
      counter -= 3600;
      break;
    case 4: // menit++
      counter += 60;
      break;
    case 8: // detik--
      counter -= 60;
      break;
    case 16: // detik++
      counter += 1;
      break;
    case 32: // detik--
      counter -= 1;
      break;
    case 64: // stop
      TIMSK1 &= ~B00000010;
      break;
    case 128: // start
      TIMSK1 |= B00000010;
      break;
    default:
      break;
    }
  prev_time = current_time;
  prev_buttons = buttons;
  }
}

/*!
  @brief menampilkan menit pada bit ke 0 dan 1, detik pada bit ke 3 dan 4, dan
  milisecond pada bit ke 6 dan 7
  @param counter angaka yang akan ditampilkan
*/
void displaySegment(int counter) {
  int detik = counter % 60;
  int menit = (counter / 60) % 60;
  int jam = (counter / 3600) % 24;
  static int digits[8];
  digits[0] = jam / 10;
  digits[1] = (jam % 10) / 1;
  digits[2] = 21; // Show blank space
  digits[3] = menit / 10;
  digits[4] = (menit % 10) / 1;
  digits[5] = 21; // Show blank space
  digits[6] = (detik % 100) / 10;
  digits[7] = (detik % 10) / 1;

  for (int i = 0; i < 8; i++) {
    sendData(0x00 | (2 * i), data7Segment[digits[i]]);
  }
}

/*!
  @brief mengirimkan command ke TM1638
  @param value command yang akan dikirim
*/
void sendCommand(uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

/*!
  @brief mengirimkan data ke TM1638
  @param address alamat yang akan dikirim. 0x00 untuk alamat 0, 0x01 untuk
  alamat 1, dst
  @param value data yang akan dikirim
*/
void sendData(uint8_t address, uint8_t value) {
  sendCommand(0x44);
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | address);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

/*!
  @brief membaca tombol yang ditekan
  @return tombol yang ditekan. 1 byte data, setiap bit merepresentasikan tombol
  yang ditekan
*/
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

/*!
  @brief mereset seluruh alamat, baik itu led ataupun 7-segment
*/
void reset() {
  sendCommand(0x40);
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);
  for (uint8_t i = 0; i < 16; i++) {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}
