#include <Arduino.h>

const int strobe = 9;
const int clock = 7;
const int data = 8;
int milisecond = 0, second = 0, menit = 0;
int buttons = 0;
bool timerRunning = true;

const int MAX_STORED_TIME = 5;
int storedTimes[MAX_STORED_TIME][3] = { 0 };

const char data7Segment[11] = {
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
  0b00000000, // blank space
};


uint8_t readButtons(void); 
void storeTime();
void displayStoredTime(int index);
void sendCommand(uint8_t value);
void sendData(uint8_t address, uint8_t value); 
void displaySegment(int menit, int detik, int mili); 
void reset();
void startStopTimer();


ISR (TIMER1_COMPA_vect) 
{   // interrupt service routine
  OCR1A += 20000;           // setting the next interrupt
  milisecond++;             // incrementing the milisecond
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
  sendCommand(0x8f);   // sending init command to control the display
  reset();
  Serial.begin(9600);
}

void loop() 
{
  displaySegment(menit, second, milisecond);
  second = milisecond / 100;
  menit = milisecond / 6000;
  if (second > 60)
  {
    second = 0;
  }
  if (menit > 60) 
  {
    menit = 0;
  }

  buttons = readButtons();
  switch (buttons) 
  {
    case 1: // Reset
      milisecond = 0;
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
}

/*!
  @brief start atau stop timer secara flipflop
*/
void startStopTimer() 
{
  timerRunning = !timerRunning;
  if (timerRunning) 
  {
    TIMSK1 |= B00000010;
  } else 
  {
    TIMSK1 &= ~B00000010;
  }
}

/*!
  @brief mereset seluruh alamat, baik itu led ataupun 7-segment
*/
void reset() 
{
  sendCommand(0x40); // sending command to set consecutive addresses to 0
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);
  for (uint8_t i = 0; i < 16; i++) 
  {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}

/*!
  @brief menampilkan menit pada bit ke 0 dan 1, detik pada bit ke 3 dan 4, dan milisecond pada bit ke 6 dan 7
  @param menit menit yang akan ditampilkan
  @param detik detik yang akan ditampilkan
  @param mili milisecond yang akan ditampilkan
*/
void displaySegment(int menit, int detik, int mili) 
{
  int digits[8];
  digits[0] = menit / 10;
  digits[1] = (menit % 10) / 1;
  digits[2] = 11;   // Show blank space
  digits[3] = detik / 10;
  digits[4] = (detik % 10) / 1;
  digits[5] = 11;   // Show blank space
  digits[6] = (mili % 100) / 10;
  digits[7] = (mili % 10) / 1;
  
  for (int i = 0; i < 8; i++) 
  {
    sendData(0x00 | (2*i), data7Segment[digits[i]]);
  }
}

/*!
  @brief membaca tombol yang ditekan
  @return tombol yang ditekan
*/
uint8_t readButtons(void) 
{
  uint8_t buttons = 0;
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0x42);
  pinMode(data, INPUT);
  for (uint8_t i = 0; i < 4; i++) 
  {
    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }
  pinMode(data, OUTPUT);
  digitalWrite(strobe, HIGH);
  return buttons;
}

/*!
  @brief menyimpan waktu yang telah dijalankan sebanyak 5 kali, terbaru selalu diawal.
*/
void storeTime() 
{
  for (int i = MAX_STORED_TIME - 1; i > 0; i--) 
  {
    storedTimes[i][0] = storedTimes[i - 1][0];
    storedTimes[i][1] = storedTimes[i - 1][1];
    storedTimes[i][2] = storedTimes[i - 1][2];
  }
  storedTimes[0][0] = menit;
  storedTimes[0][1] = second;
  storedTimes[0][2] = milisecond;
}

/*!
  @brief menampilkan waktu yang telah disimpan
  @param index index waktu yang akan ditampilkan
*/
void displayStoredTime(int index)
{
  startStopTimer();
  reset();
  switch (index)
  {
  case 0:
    sendData(7, 1);
    break;
  case 1:
    sendData(9, 1);
    break;
  case 2:
    sendData(11, 1);
    break;
  case 3:
    sendData(13, 1);
    break;
  case 4:
    sendData(15, 1);
    break;
  default:
    break;
  }
  if (index >= 0 && index < MAX_STORED_TIME) 
  {
    displaySegment(storedTimes[index][0], storedTimes[index][1], storedTimes[index][2]);
    milisecond = storedTimes[index][2];
  } else 
  {
    displaySegment(0, 0, 0);
  }
}

/*!
  @brief mengirimkan command ke TM1638
  @param value command yang akan dikirim
*/
void sendCommand(uint8_t value) 
{
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}

/*!
  @brief mengirimkan data ke TM1638
  @param address alamat yang akan dikirim
  @param value data yang akan dikirim
*/
void sendData(uint8_t address, uint8_t value) 
{
  sendCommand(0x44); // sending command to set certain address
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | address);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}