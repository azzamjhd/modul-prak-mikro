#include <Arduino.h>
#include <U8g2lib.h>

// Define pins for the ultrasonic sensor
#define TRIGGER_PIN 26
#define ECHO_PIN 27

// Define pins for the rotary encoder
#define ENCODER_PIN_A 34
#define ENCODER_PIN_B 35
#define BUTTON_PIN 25

// Define pins for the OLED
#define OLED_SCL 32
#define OLED_SDA 33

// Variables for the rotary encoder
volatile int encoderPos = 0;
volatile bool buttonPressed = false;
const int menuItems = 2;
int menuIndex = 0;

float trashCanHeight = 30.0; // Height of the trashcan in cm
long duration;
float distanceCm;
const float sensorOffset = 5.0; // Offset for the sensor mounting

const int numSamples = 10;  // Number of samples for the moving average
float distanceSamples[numSamples];
int sampleIndex = 0;
float totalDistance = 0;
float averageDistance = 0;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ OLED_SCL, /* data=*/ OLED_SDA);

// Encoder state table for direction detection
static const int8_t encoderStateTable[4][4] = {
  {0, -1, 1, 0},
  {1, 0, 0, -1},
  {-1, 0, 0, 1},
  {0, 1, -1, 0}
};

volatile int encoderState = 0;

void IRAM_ATTR updateEncoder() {
  int pinA = digitalRead(ENCODER_PIN_A);
  int pinB = digitalRead(ENCODER_PIN_B);
  int newState = (pinB << 1) | pinA;
  int direction = encoderStateTable[encoderState][newState];
  encoderState = newState;
  if (direction != 0) {
    encoderPos += direction;
  }
}

void IRAM_ATTR updateButton() {
  buttonPressed = true;
}

void readUltrasonicDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = duration * 0.034 / 2;

  // Update the moving average filter
  totalDistance -= distanceSamples[sampleIndex];
  distanceSamples[sampleIndex] = distanceCm;
  totalDistance += distanceCm;
  sampleIndex = (sampleIndex + 1) % numSamples;
  averageDistance = totalDistance / numSamples;
}

void handleMenu() {
  static int lastEncoderPos = 0;
  if (encoderPos != lastEncoderPos) {
    if (encoderPos > lastEncoderPos) {
      menuIndex = (menuIndex + 1) % menuItems;
    } else {
      menuIndex = (menuIndex - 1 + menuItems) % menuItems;
    }
    lastEncoderPos = encoderPos;
  }

  if (buttonPressed) {
    buttonPressed = false;
    if (menuIndex == 0) {
      // Calibration mode
      trashCanHeight = averageDistance - sensorOffset;
    } else if (menuIndex == 1) {
      // Showing trash level
    }
  }
}

void drawMenu() {
  u8g2.clearBuffer();

  if (menuIndex == 0) {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 14, "Calibration");
    u8g2.setCursor(0, 30);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.print("Current Height:");
    u8g2.setCursor(0, 40);
    u8g2.print(averageDistance);
    u8g2.print(" cm");
    u8g2.setCursor(0, 50);
    u8g2.print("Set Height: ");
    u8g2.setCursor(0, 60);
    u8g2.print(trashCanHeight);
    u8g2.print(" cm");
  } else if (menuIndex == 1) {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 14, "Trash Level");
    int trashLevel = map(averageDistance, sensorOffset, trashCanHeight, 100, 0);  // Reverse the level calculation
    trashLevel = constrain(trashLevel, 0, 100);  // Ensure the value is within 0-100%
    u8g2.drawFrame(0, 20, 128, 10);
    u8g2.drawBox(0, 20, map(trashLevel, 0, 100, 0, 128), 10);
    u8g2.setCursor(50, 45);
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.print(trashLevel);
    u8g2.print(" %");
  }

  u8g2.sendBuffer();
}

void drawSplashScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(20, 14, "Trash Can");
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(10, 40, "Fullness Meter");
  u8g2.sendBuffer();
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), updateButton, FALLING);

  // Initialize distance samples
  for (int i = 0; i < numSamples; i++) {
    distanceSamples[i] = 0;
  }

  drawSplashScreen();
}

void loop() {
  readUltrasonicDistance();
  handleMenu();
  drawMenu();
  delay(100); // Small delay for debounce
}












// #ifdef U8X8_HAVE_HW_SPI
// #include <SPI.h>
// #endif
// #ifdef U8X8_HAVE_HW_I2C
// #include <Wire.h>
// #endif

// #define BUTTON_PIN 25
// #define ENCODER_PIN_A 34
// #define ENCODER_PIN_B 35
// // #define TRIGGER_PIN 5
// // #define ECHO_PIN 4

// // float duration, distance;

// #define ENCODER_STATES 4
// volatile int encoderPos = 0;

// /*
//   U8g2lib Example Overview:
//     Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
//     Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
//     U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
// */
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 32, /* data=*/ 33);   // ESP32 Thing, HW I2C with pin remapping


// // int16_t offset;				// current offset for the scrolling text
// // u8g2_uint_t width;			// pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined
// // const char *text = "Azzam";	// scroll this text from right to left

// // const uint8_t tile_area_x_pos = 2;	// Update area left position (in tiles)
// // const uint8_t tile_area_y_pos = 3;	// Update area upper position (distance from top in tiles)
// // const uint8_t tile_area_width = 12;
// // const uint8_t tile_area_height = 4;	// this will allow cour18 chars to fit into the area

// // const u8g2_uint_t pixel_area_x_pos = tile_area_x_pos*8;
// // const u8g2_uint_t pixel_area_y_pos = tile_area_y_pos*8;
// // const u8g2_uint_t pixel_area_width = tile_area_width*8;
// // const u8g2_uint_t pixel_area_height = tile_area_height*8;

// void updateButton() {
//   encoderPos = 0;
// }

// // float GetDistance() {
// //   digitalWrite(TRIGGER_PIN, HIGH);
// //   delayMicroseconds(10);
// //   digitalWrite(TRIGGER_PIN, LOW);

// //   duration = pulseIn(ECHO_PIN, HIGH);
// //   distance = (duration * 0.0343) / 2;
// //   return distance;
// // }

// static const int8_t encoderStateTable[ENCODER_STATES][ENCODER_STATES] = {
//   {0, -1, 1, 0},
//   {1, 0, 0, -1},
//   {-1, 0, 0, 1},
//   {0, 1, -1, 0}
// };

// volatile int encoderState = 0;
// static void updateEncoder() {
//   int pinA = digitalRead(ENCODER_PIN_A);
//   int pinB = digitalRead(ENCODER_PIN_B);
//   int newState = (pinB << 1) | pinA;
//   int direction = encoderStateTable[encoderState][newState];
//   encoderState = newState;
//   if ((encoderState == 1 || encoderState == 2) && direction != 0) {
//     encoderPos += direction;
//   }
// }

// void setup(void) {
//   Serial.begin(115200);
//   u8g2.begin();
    
//   // u8g2.clearBuffer();					// clear the internal memory
//   // u8g2.setFont(u8g2_font_helvR10_tr);	// choose a suitable font
//   // u8g2.drawStr(0,12,"Level Sampah");	// write something to the internal memory
  
//   // // draw a frame, only the content within the frame will be updated
//   // // the frame is never drawn again, but will stay on the display
//   // u8g2.drawBox(pixel_area_x_pos-1, pixel_area_y_pos-1, pixel_area_width+2, pixel_area_height+2);
  
//   // u8g2.sendBuffer();					// transfer internal memory to the display
  
//   // u8g2.setFont(u8g2_font_courB18_tr);	// set the target font for the text width calculation
//   // width = u8g2.getUTF8Width(text);		// calculate the pixel width of the text
//   // offset = width+pixel_area_width;

//   // Set encoder pins as inputs
//   pinMode(ENCODER_PIN_A, INPUT_PULLUP);
//   pinMode(ENCODER_PIN_B, INPUT_PULLUP);
//   pinMode(BUTTON_PIN, INPUT_PULLUP);
//   // pinMode(TRIGGER_PIN, OUTPUT);
//   // pinMode(ECHO_PIN, INPUT);
  
//   // Attach interrupts to encoder pins
//   attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), updateEncoder, CHANGE);
//   attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), updateEncoder, CHANGE);
//   attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), updateButton, FALLING);  
  
// }

// void loop(void) {
//   // ------------------------ Display
//   // u8g2.clearBuffer();						// clear the complete internal memory
//   // // draw the scrolling text at current offset
//   // u8g2.setFont(u8g2_font_courB18_tr);		// set the target font
//   // u8g2.drawUTF8(
//   //   pixel_area_x_pos-width+offset, 
//   //   pixel_area_y_pos+pixel_area_height+u8g2.getDescent()-1, 
//   //   text);								// draw the scolling text
  
//   // // now only update the selected area, the rest of the display content is not changed
//   // u8g2.updateDisplayArea(tile_area_x_pos, tile_area_y_pos, tile_area_width, tile_area_height);
      
//   // offset--;								// scroll by one pixel
//   // if ( offset == 0 )	offset = width+pixel_area_width;			// start over again
//   static int pixelX = 0;
//   static int pixelY = 0;
//   static bool drawPixel = true;

//   u8g2.drawPixel(pixelX, pixelY);
//   u8g2.sendBuffer(); // transfer internal memory to the display

//   pixelX++;
//   if (pixelX >= 128) {
//     pixelX = 0;
//     pixelY++;
//     if (pixelY >= 64) {
//       pixelY = 0;
//       u8g2.clearBuffer();
//     }
//   }

//   u8g2.clearBuffer();
//   u8g2.setFont(u8g2_font_courB18_tr);
//   u8g2.setCursor(10, 20);
//   u8g2.print(encoderPos);
//   u8g2.updateDisplayArea(10, 20, 128, 20);

//   // ------------------------ Rotary Encoder
//   static int previousPos = 0;
//   if (encoderPos != previousPos) {
//     Serial.println(encoderPos);
//     previousPos = encoderPos;
//   }
//   // ------------------------

//   delay(1);							// do some small delay
// }