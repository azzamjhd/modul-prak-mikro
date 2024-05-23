#ifndef I2C_H
#define I2C_H
/*********************************************************
Project : ADROIT AVR Rev.3
Version : 1
Date    : 5/29/2015
Author  : Eko Henfri Binugroho - modified from twimaster.c made by pfleury@gmx.ch
Company : ER2C - PENS
Code    : I2C Access functions

Program ini didistribusikan secara gratis dengan menggunakan lisensi PNU/GPL
Program ini didistribusikan dengan harapan bisa dimanfaatkan dengan sebaik-baiknya Tetapi TANPA ADA GARANSI.Lihat GNU General Public License untuk lebih detilnya.
*********************************************************/
// Isi register TWCR --> TWI Control Register
#include <Arduino.h>

#define     TW_START                0x08
#define     TW_REP_START            0x10
#define     TW_MT_SLA_ACK           0x18
#define     TW_MT_SLA_NACK          0x20
#define     TW_MT_DATA_ACK          0x28
#define     TW_MT_DATA_NACK         0x30
#define     TW_MT_ARB_LOST          0x38
#define     TW_MR_ARB_LOST          0x38
#define     TW_MR_SLA_ACK           0x40
#define     TW_MR_SLA_NACK          0x48
#define     TW_MR_DATA_ACK          0x50
#define     TW_MR_DATA_NACK         0x58
#define     TW_ST_SLA_ACK           0xA8
#define     TW_ST_ARB_LOST_SLA_ACK  0xB0
#define     TW_ST_DATA_ACK          0xB8
#define     TW_ST_DATA_NACK         0xC0
#define     TW_ST_LAST_DATA         0xC8
#define     TW_SR_SLA_ACK           0x60
#define     TW_SR_ARB_LOST_SLA_ACK  0x68
#define     TW_SR_GCALL_ACK         0x70
#define     TW_SR_ARB_LOST_GCALL_ACK   0x78
#define     TW_SR_DATA_ACK          0x80
#define     TW_SR_DATA_NACK         0x88
#define     TW_SR_GCALL_DATA_ACK    0x90
#define     TW_SR_GCALL_DATA_NACK   0x98
#define     TW_SR_STOP              0xA0
#define     TW_NO_INFO              0xF8
#define     TW_BUS_ERROR            0x00
#define     TW_STATUS_MASK          0B11111000
#define     TW_STATUS   (TWSR & TW_STATUS_MASK)

// mendefinisikan arah dari data untuk membaca data dari I2C device
// pada fungsi i2c_start(),i2c_rep_start()
#define I2C_READ    1

// mendefinisikan arah data untuk prtoses tulis ke I2C device
// pada fungsi i2c_start(),i2c_rep_start()
#define I2C_WRITE   0

// mendefinisikan frek. Clock dari CPU frequency  (Mhz)  sesuai clock yang digunakan
#ifndef F_CPU
#define F_CPU (uint32_t) 16000000          // 16 Mhz
#endif

// mendefinisikan clock I2C (Hz)
#define SCL_CLOCK  (uint32_t) 400000      // Hz

struct Data16Bit {
    uint8_t d8u[2];
};

class myI2C  // mendefiniskan obyek I2C
{
    public:
        //fungsi untuk menginisalisasi I2C master (hanya dipanggil sekali)
        void    Init();
        
        //menghentikan transfer data dari bus I2C
        void    Stop(void);
        
        //  memanggil kondisi start dan alamat device serta arah transfer data
        //  @parameter address  = alamat dan arah data
        //  @return   0 = device berhasil diakses
        //  @retval   1 = device gagal diakses
        uint8_t Start(uint8_t address);
        
        //  memanggil kondisi start berulang dan alamat device serta arah transfer data
        //  @parameter address  = alamat dan arah data
        //  @return    0 = device berhasil diakses
        //  @retval    1 = device gagal diakses
        uint8_t Rep_Start(uint8_t address);
        
        //  memanggil kondisi start dan alamat device serta arah transfer data
        //  jika device sedang sibuk, maka akan dipooling data ack sampai dengan device siap diakses  
        //  @parameter address  = alamat dan arah data
        void    Start_Wait(uint8_t address);

        //  mengirim 1 byte data ke I2C device
        //  @parameter data = byte yang akan dituliskan
        //  @return    0 = sukses
        //  @return    1 = gagal
        uint8_t Write(uint8_t data);

        //membaca 1 byte data dari device, dan masih meminta data berikutnya
        //  @return   = byte data hasil pembacaaan device
        uint8_t ReadAck(void);

        // membaca 1 byte data dari device, tetapi sudah tidak mau membaca berikutnya (diakhiri kondisi STOP)
        //  @return   = byte data hasil pembacaaan device
        uint8_t ReadNak(void);

        void    ReadWord(uint8_t devAddr, uint8_t regAddr, Data16Bit *x16);
        void    ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        int8_t  ReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t * data);
        void    WriteWord(uint8_t devAddr, uint8_t regAddr, int16_t x);
        void    WriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        void    WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
        int8_t  ReadBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
        int8_t  ReadBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
        void    WriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        void    WriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
}i2c;  // deklarasi dari nama obyek I2C

#endif