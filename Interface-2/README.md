<h1 align='center'>TUGAS 2</h1>

Tampilkan running LED pada board I/O dengan rotasi satu buah LED dengan arah ke kanan saat S1 tidak ditekan dan ke-kiri saat S1 ditekan.

## Penggunaan
copy text [`src/main.cpp`](https://github.com/azzamjhd/modul-prak-mikro/blob/0762a84d8dca36791cf0b62773bdc8f7d36826d0/Interface-1/src/main.cpp) ke dalam folder `src/main.cpp` pada project PlatformIO
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

#### readButtons()

```cpp
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
```
Berfungsi untuk membaca tombol yang berada pada IC TM1638. Besar data yang dikirimkan yaitu 4 Byte data, namun pada modul ini hanya terhubung sebanyak 8 tombol saja, sehingga bentuk data yang dikirimkan yaitu,

|ke-7|ke-6|ke-5|ke-4|ke-3|ke-2|ke-1|ke-0|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|x|x|x|sw2|x|x|x|sw1|
|x|x|x|sw4|x|x|x|sw3|
|x|x|x|sw6|x|x|x|sw5|
|x|x|x|sw8|x|x|x|sw7|

- Mengirimkan perintah `0x42` untuk membaca tombol.
  ```cpp
  shiftOut(data, clock, LSBFIRST, 0x42);
  ```
- Mengubah mode pada pin `data` sebagai input untuk menerima data dari IC.
  ```cpp
  pinMode(data, INPUT);
  ```
- Mengubah data 4 Byte yang diterima menjadi 1 byte data. 
  <br>Data byte pertama disimpan pada variabel `buttons`, kemudian byte kedua di *shift* sebanyak `i` lalu disimpan pada variabel `buttons` dengan menggunakan operasi *or* sehingga tidak menghapus data yang sudah ada.
  ```cpp
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t v = shiftIn(data, clock, LSBFIRST) << i;
    buttons |= v;
  }
  ```
- Mengubah kembali mode pin `data` menjadi output.
  ```cpp
  pinMode(data, OUTPUT);
  ```
- Mengembalikan nilai `buttons` yang menunjukkan nilai tombol.
  ```cpp
  return buttons;
  ```

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
<br><image src="https://github.com/azzamjhd/modul-prak-mikro/blob/69ef185c262d7293858fe40e5f88ffe5d10a2927/assets/init.jpg" width="700">

|1|0|0|0|1|1|1|1|
|-|-|-|-|-|-|-|-|
|-|-|x|x|on/off|n|n|n|

- bit ke-1 hingga ke-3 mengatur kecerahan display. 
- bit ke-4 mengatur on/off dari display. 
- bit ke-5 dan ke-6 tidak termasuk.
- bit ke-7 dan ke-8 harus memiliki nilai tersebut.

#### Loop()
Fungsi `loop()` akan berjalan secara berulang-ulang setelah fungsi `setup()` telah berjalan.
```cpp
void loop() {
  // set mode single address
  sendCommand(0x44);
  // set the before and after current led to off
  sendData(0x0f & (i-2), 0x00);
  sendData(0x0f & (i+2), 0x00);
  // set olny the odd adress to on
  if (i % 2 != 0) {
    sendData(0x0f & i, 0x01);
  }
  uint8_t buttons = readButtons();
  // if button 1 is pressed, led runs to the left else to the right
  if (buttons == 0x01) {
    i -= 2;
  } else {
    i += 2;
  }
  delay(100);
}
```
Pertama akan menjalankan `sendCommand(0x44)` untuk mengirimkan data `0x44` kepada IC, yang memiliki fungsi memberikan perintah untuk menuliskan sebuah data pada alamat tertentu, yang kemudian akan dilakukan oleh perintah dibawahnya yaitu,
```cpp
sendData(0x0f & (i-2), 0x00);
sendData(0x0f & (i+2), 0x00);
```
Mengirimkan data `0x00` untuk mematikan led sebelum dan sesudah led saat ini. Sehingga disaat `i` bertambah pada loop berikutnya maupun itu *increment* ataupun *decrement* maka led yang menyala hanya 1. Kode pada baris ini dapat juga diartikan sebagai reset sebelum menyalakan led yang lain.
```cpp
if (i % 2 != 0) {
  sendData(0x0f & i, 0x01);
}
```
Menghidupkan led pada alamat ganjil dari `0xc1` hingga `0xcf`.`0x0f & i` merupakan alamat `i` yang dimasking sehingga hanya mendapatkan range `0x00 - 0x0f` dimana akan di increment setiap loop sebanyak `2` dimulai dari `0x01`.
```cpp
uint8_t buttons = readButtons();
if (buttons == 0x01) {
  i -= 2;
} else {
  i += 2;
}
```
Mengecek nilai dari button saat ini, jika bernilai `0x01` atau tombol ke-1 menyala maka arah led yang akan menyala ke kiri, sedangkan jika nilai button `0x00` / tidak ada tombol yang ditekan maka arah led yang akan menyala ke kanan.