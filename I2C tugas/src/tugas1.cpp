#include <Arduino.h>
#include <I2C.h>

#define DevAddr 0xC0

void setup() {
  // Tugas 1
  uint8_t data1, data2[6];
  i2c.Init();
  i2c.WriteByte(DevAddr, 0x00, 0x01);   // mengirim data 0x01 ke register 0x00 pada slave 0xC0
  i2c.WriteByte(DevAddr, 0x01, 0x02);   // mengirim data 0x02 ke register 0x01 pada slave 0xC0
  i2c.ReadByte(DevAddr, 0x02, &data1);  // membaca data pada register 0x02 pada slave 0xC0
  i2c.ReadBytes(DevAddr, 0x05, 6, data2);// membaca data dari register 0x05 sampai 0x0A
}

void loop() {
}
