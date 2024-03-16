# Tugas 1
Tampilkan running LED pada board I/O dengan rotasi satu buah LED dari kiri ke-kanan dengan memodifikasi program sebelumnya.
## Instalasi
copy text [`src/main.cpp`](https://github.com/azzamjhd/modul-prak-mikro/blob/0762a84d8dca36791cf0b62773bdc8f7d36826d0/Interface-1/src/main.cpp) ke dalam folder `src/main.cpp` pada project PlatformIO
## Penjelasan Kode
#### Inisialisasi Pin GPIO
```cpp
const int strobe = 9; // pin strobe pada pin ke 9
const int clock = 7;  // pin clock pada pin ke 7
const int data = 8    // pin data pada pin ke 8
```
data bertipe `const int` karena nilai tidak akan berubah selama kode dijalankan.

#### Setup
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
