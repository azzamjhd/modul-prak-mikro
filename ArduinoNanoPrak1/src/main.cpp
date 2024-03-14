#include <Arduino.h>

int pin_LED1 = 10;
int pin_LED2 = 9;
int pin_LED3 = 8;
int pin_Switch = 2;

int state = 0;
int lastState = 0;
int value = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_LED1, OUTPUT);
  pinMode(pin_LED2, OUTPUT);
  pinMode(pin_LED3, OUTPUT);
  pinMode(pin_Switch, INPUT);
  digitalWrite(pin_LED1, LOW);
  digitalWrite(pin_LED2, LOW);
  digitalWrite(pin_LED3, LOW);
}

void loop(){
  state = digitalRead(pin_Switch);
  if (state != lastState && state == 1) {
    value += 1;
  }

  // Tugas 1
  // digitalWrite(pin_LED1, LOW);
  // digitalWrite(pin_LED2, LOW);
  // digitalWrite(pin_LED3, LOW);

  // if (value % 2 == 0) {
  //   delay(100);
  // } else {
  //   delay(500);
  // }

  // digitalWrite(pin_LED1, HIGH);
  // digitalWrite(pin_LED2, HIGH);
  // digitalWrite(pin_LED3, HIGH);

  // if (value % 2 == 0) {
  //   delay(100);
  // } else {
  //   delay(500);
  // }


  // Tugas 2
  
  if (value % 3 == 0) {
    digitalWrite(pin_LED1, HIGH);
    digitalWrite(pin_LED2, LOW);
    digitalWrite(pin_LED3, LOW);
  } else if (value % 3 == 1) {
    digitalWrite(pin_LED1, LOW);
    digitalWrite(pin_LED2, HIGH);
    digitalWrite(pin_LED3, LOW);
  } else {
    digitalWrite(pin_LED1, LOW);
    digitalWrite(pin_LED2, LOW);
    digitalWrite(pin_LED3, HIGH);
  }

  lastState = state;
}