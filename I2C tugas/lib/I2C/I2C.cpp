#include "I2C.h"

/*************************************************************************
  Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void myI2C::Init(void)
{
/* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
    TWSR = 0;                         /* no prescaler */
    TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */
}

/*************************************************************************
Issues a start condition and sends address and transfer direction.
return 0 = device accessible, 1= failed to access device
*************************************************************************/
uint8_t myI2C::Start(uint8_t address)
{   uint8_t   twst;
// send START condition
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
// wait until transmission completed
    while(!(TWCR & (1<<TWINT)));
// check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ( (twst != TW_START) && (twst != TW_REP_START)) return 1;
        // send device address
         TWDR = address;
         TWCR = (1<<TWINT) | (1<<TWEN);
         // wail until transmission completed and ACK/NACK has been received
         while(!(TWCR & (1<<TWINT)));
         // check value of TWI Status Register. Mask prescaler bits.
         twst = TW_STATUS & 0xF8;
         if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
         return 0;
}


/*************************************************************************
Issues a start condition and sends address and transfer direction.
If device is busy, use ack polling to wait until device is ready
Input:   address and transfer direction of I2C device
*************************************************************************/
void myI2C::Start_Wait(uint8_t address)
{
     uint8_t   twst;
     while ( 1 )
     {  // send START condition
        TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
        // wait until transmission completed
        while(!(TWCR & (1<<TWINT)));
         // check value of TWI Status Register. Mask prescaler bits.
         twst = TW_STATUS & 0xF8;
         if ( (twst != TW_START) && (twst != TW_REP_START)) continue;
         // send device address
         TWDR = address;
         TWCR = (1<<TWINT) | (1<<TWEN);
         // wail until transmission completed
         while(!(TWCR & (1<<TWINT)));
         // check value of TWI Status Register. Mask prescaler bits.
         twst = TW_STATUS & 0xF8;
         if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) )
         {
             /* device busy, send stop condition to terminate write operation */
                 TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

                 // wait until stop condition is executed and bus released
                 while(TWCR & (1<<TWSTO));
                 continue;
         }
         //if( twst != TW_MT_SLA_ACK) return 1;
         break;
      }
}

/*************************************************************************
  Issues a repeated start condition and sends address and transfer direction
  Input:   address and transfer direction of I2C device
  Return:  0 device accessible
           1 failed to access device
*************************************************************************/
uint8_t myI2C::Rep_Start(uint8_t address)
{
    return myI2C::Start( address );
}

/*************************************************************************
  Terminates the data transfer and releases the I2C bus
*************************************************************************/

void myI2C::Stop(void)
{
     // send stop condition
         TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
         // wait until stop condition is executed and bus released
         while(TWCR & (1<<TWSTO));
}

/*************************************************************************
Send one byte to I2C device
   Input:    byte to be transfered
   Return:   0 write successful
             1 write failed
*************************************************************************/
uint8_t myI2C::Write( uint8_t data )
{        uint8_t   twst;
         // send data to the previously addressed device
         TWDR = data;
         TWCR = (1<<TWINT) | (1<<TWEN);

         // wait until transmission completed
         while(!(TWCR & (1<<TWINT)));
         // check value of TWI Status Register. Mask prescaler bits
         twst = TW_STATUS & 0xF8;
         if( twst != TW_MT_DATA_ACK) return 1;
         return 0;
}

/*************************************************************************
  Read one byte from the I2C device, request more data from device
  Return:  byte read from I2C device
*************************************************************************/
uint8_t myI2C::ReadAck(void)
{    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
     while(!(TWCR & (1<<TWINT)));
     return TWDR;
}

/*************************************************************************
  Read one byte from the I2C device, read is followed by a stop condition
  Return:  byte read from I2C device
*************************************************************************/
uint8_t myI2C::ReadNak(void)
{    TWCR = (1<<TWINT) | (1<<TWEN);
     while(!(TWCR & (1<<TWINT)));
     return TWDR;
}

void myI2C::ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
    {   uint8_t i;
        if(length > 0)
        {   //request register
            myI2C::Start(devAddr | I2C_WRITE);
            myI2C::Write(regAddr);
            _delay_us(5);
            //read data
            myI2C::Start(devAddr | I2C_READ);
            length--;
            for(i=0; i<=length; i++)
            {   if(i==length)   data[i] = myI2C::ReadNak();
                else            data[i] = myI2C::ReadAck();
            }
            myI2C::Stop();
        }
    }

void   myI2C::ReadWord(uint8_t devAddr, uint8_t regAddr, Data16Bit *x16)
    {   myI2C::Start(devAddr | I2C_WRITE);
        myI2C::Write(regAddr);
        _delay_us(5);
        //read data
        myI2C::Start(devAddr | I2C_READ);
        x16->d8u[1] = myI2C::ReadAck();
        x16->d8u[0] = myI2C::ReadNak();
        myI2C::Stop();
    }

    int8_t myI2C::ReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t * data)
    {   //request register
        myI2C::Start(devAddr | I2C_WRITE);
        myI2C::Write(regAddr);
        _delay_us(5);
        //read data
        myI2C::Start(devAddr | I2C_READ);
        *data = myI2C::ReadNak();
        myI2C::Stop();
        return 1;
    }

    void myI2C::WriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
        myI2C::Start(devAddr | I2C_WRITE);
        myI2C::Write(regAddr); //reg
        myI2C::Write(data);
        myI2C::Stop();
    }

    void myI2C::WriteWord(uint8_t devAddr, uint8_t regAddr,int16_t x){
        myI2C::Start(devAddr | I2C_WRITE);
        myI2C::Write(regAddr); //reg
        myI2C::Write((uint8_t)(x>>8));
        myI2C::Write((uint8_t)x);
        myI2C::Stop();
    }


    void myI2C::WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
        uint8_t i;
        if(length > 0) {
            //write data
            myI2C::Start(devAddr | I2C_WRITE);
            myI2C::Write(regAddr); //reg
            for (i = 0; i < length; i++) {
                myI2C::Write((uint8_t) data[i]);
            }
            myI2C::Stop();
        }
    }

    

int8_t myI2C::ReadBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) 
{       int8_t count = 0;
        if(length > 0) {
            uint8_t b;
            if ((count = myI2C::ReadByte(devAddr, regAddr, &b)) != 0) {
                uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
                b &= mask;
                b >>= (bitStart - length + 1);
                *data = b;
            }
        }
        return count;
}

int8_t myI2C::ReadBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) 
{
        uint8_t b;
        uint8_t count = myI2C::ReadByte(devAddr, regAddr, &b);
        *data = b & (1 << bitNum);
        return count;
}

void myI2C::WriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) 
{
        if(length > 0) {
            uint8_t b = 0;
            if (myI2C::ReadByte(devAddr, regAddr, &b) != 0) { //get current data
                uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
                data <<= (bitStart - length + 1); // shift data into correct position
                data &= mask; // zero all non-important bits in data
                b &= ~(mask); // zero all important bits in existing byte
                b |= data; // combine data with existing byte
                myI2C::WriteByte(devAddr, regAddr, b);
            }
        }
 }

void myI2C::WriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) 
{
        uint8_t b;
        myI2C::ReadByte(devAddr, regAddr, &b);
        b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
        myI2C::WriteByte(devAddr, regAddr, b);
}


