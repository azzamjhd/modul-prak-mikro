<h1 align='center'>TUGAS 1</h1>

>Tampilkan running LED pada board I/O dengan rotasi satu buah LED dari kiri ke-kanan dengan memodifikasi program sebelumnya.

## Penggunaan
copy text pada <a href="/Interface-1/src/main.cpp">src/main.cpp</a> ke dalam folder `src/main.cpp` pada project PlatformIO
## Penjelasan Kode
#### Inisialisasi Pin GPIO
```cpp
const int strobe = 9; // pin strobe pada pin ke 9
const int clock = 7;  // pin clock pada pin ke 7
const int data = 8    // pin data pada pin ke 8
uint8_t i = 0x01;     // variabel i sebagai global loop
```
data bertipe `const int` karena nilai tidak akan berubah selama kode dijalankan.

#### sendCommand()
```cpp
Befungsi untuk mengirimkan 1 byte data kepada IC TM1638.
void sendCommand(uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}
```
Men-select chip dengan mengirimkan sinyal *Low* pada pin strobe lalu Mengirimkan 1 Byte data menggunakan `shiftOut` function, kemudian mengirimkan sinyal *High* pada pin strobe untuk me-unselect chip.

#### sendData()
Berfungsi untuk mengirimkan 2 byte data yang merupakan sebuah alamat dan data yang akan ditulis pada alamat tersebut.
```cpp
void sendData(uint8_t address, uint8_t value) {
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0 | address);
  shiftOut(data, clock, LSBFIRST, value);
  digitalWrite(strobe, HIGH);
}
```
Pertama diawali dengan men-select chip terlebih dahulu. Kemudian mengirimkan 2 byte data secara berurutan yaitu *address* sebagai byte ke-1 kemudian *data* yang akan diisi pada alamat tersebut sebagai byte ke-2. Dan yang terakhir me-unselect chip.

#### reset()
Berfungsi untuk menuliskan nilai 0 pada semua alamat pada IC TM1638 sehingga semua led dan 7-segment mati.
```cpp
void reset() {
  sendCommand(0x40); // sending command to set consecutive addresses to 0
  digitalWrite(strobe, LOW);
  shiftOut(data, clock, LSBFIRST, 0xc0);
  for (uint8_t i = 0; i < 16; i++) {
    shiftOut(data, clock, LSBFIRST, 0x00);
  }
  digitalWrite(strobe, HIGH);
}
```
Diawali dengan mengirimkan sebuah perintah `0x40` yang berfungsi untuk menuliskan nilai secara increment/berurutan dengan alamat awal yang diingikan. Kemudian mengirimakan alamat `0xc0` yang berarti operasi akan dimulai dari alamat `0`, kemudian mengirimkan nilai `0x00` sebanyak 16 kali untuk menuliskannya pada alamat `0xc0` hingga `0xcf`.


#### setup()
Fungsi ini akan berjalan 1 kali pada saat Mikrokontroler boot/reset. 
```cpp
void setup() {
  pinMode(strobe, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  sendCommand(0x8f); // sending init command to control the display
  reset();
}
```
`sendCommand(0x8f);` berfungsi untuk mengirimkan sebuah data bernilai `0x8f`, atau dalam bilangan biner `0b10001111` yang berarti dalam datasheet TM1638 yaitu,
<br><image src="/assets/init.jpg" width="700">

|1|0|0|0|1|1|1|1|
|-|-|-|-|-|-|-|-|
|-|-|x|x|on/off|n|n|n|

Dimana n pada bit ke-1 hingga ke-3 mengatur kecerahan display. Ke-4 mengatur on/off dari display. Dan bit ke-5 dan ke-6 tidak termasuk.

#### Loop()
Fungsi `loop()` akan berjalan secara berulang-ulang setelah fungsi `setup()` telah berjalan.
```cpp
void loop() {
  sendCommand(0x44); // sending command to set certain address
  if (i % 2 != 0) {
    sendData(0x0f & i, 0x01);
    Serial.println("hidup");
  }
  i += 2;
  delay(100);
  reset();
}
```
Pertama akan menjalankan `sendCommand(0x44)` untuk mengirimkan data `0x44` kepada IC, yang memiliki fungsi memberikan perintah untuk menuliskan sebuah data pada alamat tertentu, yang kemudian akan dilakukan oleh perintah dibawahnya yaitu, `sendData(0x0f & i, 0x01);` mengirimkan data `0x01` untuk menghidupkan led pada alamat ganjil dari `0xc1` hingga `0xcf`.`0x0f & i` merupakan alamat `i` yang dimasking sehingga hanya mendapatkan range `0x00 - 0x0f` dimana akan di increment setiap loop sebanyak `2` dimulai dari `0x01`.