#include <Arduino.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

char data;
String str;
void loop() {
  if (Serial.available() > 0) {
    data = Serial.read();
    if (data != '\n') {
      str += data;
      // Serial.println(data);
      // Serial.println(str);
    } else {
      int num = str.toInt();
      Serial.println(str);
      // Serial.println(num);
      if (num % 2 == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
      str = "";
    }
  }
}